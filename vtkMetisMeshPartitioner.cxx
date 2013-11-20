/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vtkCosmoDensityProfile.h

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
#include "vtkMetisMeshPartitioner.h"

// VTK includes
#include "vtkAlgorithm.h"
#include "vtkCellData.h"
#include "vtkDataObject.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkStructuredGrid.h"
#include "vtkUnstructuredGrid.h"

// C/C++ includes
#include <cassert>
#include <vector>

#include "metis.h"

namespace { // private namespace

template<typename T>
void Partition(T* data, idx_t nparts)
{
  // STEP 1: Setup metis data-structures
  idx_t ne = data->GetNumberOfCells();
  idx_t nn = data->GetNumberOfPoints();

  // METIS mesh data-structure (see METIS manual Section 5.6)
  std::vector< idx_t > eptr;
  eptr.resize( ne+1 );
  std::vector< idx_t > eind;

  // vectors used for weighted partitioning
  idx_t *vwgt = NULL;
  idx_t *vsize = NULL;
  real_t *tpwgts = NULL;

  // Return values
  idx_t objval = 0; // stores edgecut or total communication volume upon
                    // successful completion.

  std::vector< idx_t > epart; // the element partition vector
  epart.resize( ne );
  std::vector< idx_t > npart; // the node partition vector
  npart.resize( nn );

  // STEP 2: Setup the metis options vector
  idx_t options[METIS_NOPTIONS];
  METIS_SetDefaultOptions(options);
  options[METIS_OPTION_NUMBERING] = 0;
  if( nparts > 8 )
    {
    options[METIS_OPTION_PTYPE]=METIS_PTYPE_KWAY;
    }
  else
    {
    options[METIS_OPTION_PTYPE]=METIS_PTYPE_RB;
    }

  // STEP 3: Convert unstructured grid to METIS mesh
  eptr[0] = 0;
  vtkIdList *cellIds = vtkIdList::New();
  for(vtkIdType cellIdx=0; cellIdx < data->GetNumberOfCells(); ++cellIdx)
    {
    cellIds->Reset();
    data->GetCellPoints(cellIdx,cellIds);

    for(vtkIdType nodeIdx=0; nodeIdx < cellIds->GetNumberOfIds(); ++nodeIdx )
      {
      eind.push_back( cellIds->GetId(nodeIdx) );
      } // END for all cell nodes

    eptr[cellIdx+1] = eind.size();
    } // END for all grid cells
  cellIds->Delete();

  // STEP 4: Partition METIS mesh
  int rc = METIS_PartMeshNodal(
      &ne, &nn, &eptr[0],&eind[0],
      vwgt,vsize,&nparts,tpwgts,options,
      &objval,&epart[0],&npart[0]);

  // STEP 5: Add output array
  vtkNew<vtkIntArray> partitionIds;
  partitionIds->SetName("PartitionID");
  partitionIds->SetNumberOfValues(data->GetNumberOfCells());
  for(vtkIdType cellIdx=0; cellIdx < data->GetNumberOfCells(); ++cellIdx)
    {
    partitionIds->SetValue(cellIdx, epart[ cellIdx ]);
    }
  data->GetCellData()->AddArray( partitionIds.GetPointer() );
}

} // end of private namespace

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMetisMeshPartitioner);

//------------------------------------------------------------------------------
vtkMetisMeshPartitioner::vtkMetisMeshPartitioner()
{
  this->NumberOfPartitions = 5;
}

//------------------------------------------------------------------------------
vtkMetisMeshPartitioner::~vtkMetisMeshPartitioner()
{
}

//------------------------------------------------------------------------------
void vtkMetisMeshPartitioner::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//------------------------------------------------------------------------------
int vtkMetisMeshPartitioner::RequestData(
          vtkInformation* vtkNotUsed(request),
          vtkInformationVector **inputVector,
          vtkInformationVector *outputVector)
{
  // STEP 0: Get input object
  vtkInformation *input = inputVector[0]->GetInformationObject( 0 );
  assert("pre: input information object is NULL!" && (input != NULL) );
  vtkDataObject *inputData = input->Get( vtkDataObject::DATA_OBJECT() );
  assert("pre: input grid is NULL!" && (inputData != NULL) );

  // STEP 1: Get output object
  vtkInformation *output = outputVector->GetInformationObject( 0 );
  assert("pre: output information object is NULL!" && (output != NULL) );
  vtkDataObject *outputData = output->Get(vtkDataObject::DATA_OBJECT() );

  // STEP 2: Shallow copy input object to output
  outputData->ShallowCopy( inputData );

  // STEP 3: Short-circuit here if we are not partitioning
  if( this->NumberOfPartitions < 2 )
    {
    return 1;
    }

  if(vtkUnstructuredGrid::SafeDownCast(outputData))
    {
    Partition(vtkUnstructuredGrid::SafeDownCast(outputData), this->NumberOfPartitions);
    }
  else if(vtkPolyData::SafeDownCast(outputData))
    {
    Partition(vtkPolyData::SafeDownCast(outputData), this->NumberOfPartitions);
    }
  else if(vtkStructuredGrid::SafeDownCast(outputData))
    {
      Partition(vtkUnstructuredGrid::SafeDownCast(outputData), this->NumberOfPartitions);
    }
  else
    {
      vtkErrorMacro(<<"Unsupported data type");
      return 0;
    }
  return 1;
}
