#include "vtkMetisKernel.h"

// VTK includes
#include "vtkCellData.h"
#include "vtkCellType.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGrid.h"

// C/C++ includes
#include <cassert>
#include <iostream>
#include <vector>

// Metis includes
#include "metis.h" // For Metis API

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMetisKernel);

//------------------------------------------------------------------------------
vtkMetisKernel::vtkMetisKernel()
{
  this->Element2PartitionId = NULL;
}

//------------------------------------------------------------------------------
vtkMetisKernel::~vtkMetisKernel()
{
  if( this->Element2PartitionId != NULL )
    {
    delete [] this->Element2PartitionId;
    }
}

//------------------------------------------------------------------------------
void vtkMetisKernel::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//------------------------------------------------------------------------------
void vtkMetisKernel::Partition(
    vtkUnstructuredGrid *grid, const int NP, bool attachCellArray)
{
  assert("pre: input grid is NULL!" && (grid != NULL));

  // STEP 0: Setup metis data-structures
  idx_t ne = grid->GetNumberOfCells();
  idx_t nn = grid->GetNumberOfPoints();

  // METIS mesh data-structure (see METIS manual Section 5.6)
  std::vector< idx_t > eptr;
  eptr.resize( ne+1 );
  std::vector< idx_t > eind;

  // vectors used for weighted partitioning
  idx_t *vwgt    = NULL;
  idx_t *vsize   = NULL;
  real_t *tpwgts = NULL;

  idx_t ncommon = 2;
  idx_t nparts  = NP;

  // Return values
  idx_t objval = 0; // stores edgecut or total communication volume upon
                    // successful completion.

  std::vector< idx_t > epart; // the element partition vector
  epart.resize( ne );
  std::vector< idx_t > npart; // the node partition vector
  npart.resize( nn );

  // STEP 1: Initi
  if( this->Element2PartitionId != NULL )
    {
    delete [] this->Element2PartitionId;
    }
  this->Element2PartitionId = new int[ ne ];

  // STEP 2: Setup the metis options vector
  idx_t options[METIS_NOPTIONS];
  METIS_SetDefaultOptions(options);
  options[METIS_OPTION_NUMBERING] = 0;
  if( NP > 8 )
    {
    options[METIS_OPTION_PTYPE]=METIS_PTYPE_KWAY;
    }
  else
    {
    options[METIS_OPTION_PTYPE]=METIS_PTYPE_RB;
    }

  // STEP 3: Convert unstructured grid to METIS mesh
  this->BuildMetisMeshDataStructure(grid,eptr,eind);

  // STEP 4: Partition METIS mesh
  int rc = METIS_PartMeshNodal(
      &ne, &nn, &eptr[0],&eind[0],
      vwgt,vsize,&nparts,tpwgts,options,
      &objval,&epart[0],&npart[0]);
//  int rc = METIS_PartMeshDual(
//      &ne, &nn, &eptr[0], &eind[0],
//      vwgt, vsize, &ncommon, &nparts, tpwgts, options,
//      &objval, &epart[0], &npart[0]);
//  if( rc != METIS_OK )
//    {
//    switch( rc )
//      {
//      case METIS_ERROR_INPUT:
//        std::cerr << "ERROR: metis input error!\n";
//        break;
//      case METIS_ERROR_MEMORY:
//        std::cerr << "ERROR: metis cannot allocate required memory!\n";
//        break;
//      default:
//        std::cerr << "ERROR: metis internal error occured!\n";
//      } // END switch
//    } // END if

  // STEP 5: Construct output arrays
  for(vtkIdType cellIdx=0; cellIdx < grid->GetNumberOfCells(); ++cellIdx)
    {
    this->Element2PartitionId[ cellIdx ] = epart[ cellIdx ];
    } // END for all cells

  // STEP 6: Attach cell array if requested
  if( attachCellArray )
    {
    this->AttachPartitionIdToGrid(grid);
    }

  // STEP 7: Clean up
  eptr.clear();
  eind.clear();
  epart.clear();
  npart.clear();
}

//------------------------------------------------------------------------------
void vtkMetisKernel::BuildMetisMeshDataStructure(
      vtkUnstructuredGrid *grid,
      std::vector< idx_t >& eptr,
      std::vector< idx_t >& eind)
{
  // Sanity checks
  assert("pre: input grid is NULL!" && (grid!=NULL));
  assert("pre: eptr is not initialized" &&
          (eptr.size()==grid->GetNumberOfCells()+1) );

  eptr[0] = 0;
  vtkIdList *cellIds = vtkIdList::New();
  for(vtkIdType cellIdx=0; cellIdx < grid->GetNumberOfCells(); ++cellIdx)
    {
    cellIds->Reset();
    grid->GetCellPoints(cellIdx,cellIds);

    for(vtkIdType nodeIdx=0; nodeIdx < cellIds->GetNumberOfIds(); ++nodeIdx )
      {
      eind.push_back( cellIds->GetId(nodeIdx) );
      } // END for all cell nodes

    eptr[cellIdx+1] = eind.size();
    } // END for all grid cells

  cellIds->Delete();
}

//------------------------------------------------------------------------------
void vtkMetisKernel::AttachPartitionIdToGrid(vtkUnstructuredGrid *grid)
{
  assert("pre: input grid is NULL!" && (grid != NULL));

  vtkIdTypeArray* partitionIdArray = vtkIdTypeArray::New();
  partitionIdArray->SetName("PartitionID");
  partitionIdArray->SetNumberOfComponents( 1 );
  partitionIdArray->SetNumberOfTuples( grid->GetNumberOfCells() );

  vtkIdType* arrayPtr =
      static_cast<vtkIdType*>(partitionIdArray->GetVoidPointer(0));

  for(vtkIdType idx=0; idx < grid->GetNumberOfCells(); ++idx)
    {
    arrayPtr[ idx ] = this->Element2PartitionId[ idx ];
    } // END for all cells

  grid->GetCellData()->AddArray( partitionIdArray );
  partitionIdArray->Delete();
}
