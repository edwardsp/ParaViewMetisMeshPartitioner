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
// .NAME vtkMetisKernel.h -- Wrapper around the METIS.
//
// .SECTION Description
//  A concrete instance of vtkUnstructuredGridAlgorithm that provides
//  functionality for partitioning a VTK unstructured grid.

#ifndef VTKMETISKERNEL_H_
#define VTKMETISKERNEL_H_

#include "vtkObject.h" // Base class

// Other includes
#include "metis.h" // For metis definitions, i.e, for idx_t
#include <vector>  // For STL vector

// Forward declarations
class vtkUnstructuredGrid;
class vtkIndent;

class VTK_EXPORT vtkMetisKernel : public vtkObject
{
public:
  static vtkMetisKernel* New();
  vtkTypeMacro(vtkMetisKernel,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Uses METIS to Partition the user-supplied unstructured grid domain to the
  // requested number of partitions, NP. If attachCellArray is true, then
  // a PartitionID array will be added to the given grid instance.
  void Partition(
      vtkUnstructuredGrid *grid,const int NumberOfPartitions,
      bool attachCellArray=false);

protected:
  vtkMetisKernel();
  virtual ~vtkMetisKernel();

  // Description:
  // Attaches partitioin ID information to the user-supplied grid.
  void AttachPartitionIdToGrid(vtkUnstructuredGrid *grid);

  // Description:
  // Given a VTK unstructured grid, this method builds the Metis mesh
  // representation. The metis mesh data structure is defined by two
  // integer arrays eptr and eind wherein eind holds the nodeIds of each
  // element sequentially and eptr[i] holds the index of the first node of
  // the ith element in the eind array and eptr[i+1] holds the last
  // (non-inclusive). For details, see the METIS manual section 5.6.
  void BuildMetisMeshDataStructure(
      vtkUnstructuredGrid *grid,
      std::vector< idx_t >& eptr,
      std::vector< idx_t >& eind);

  int* Element2PartitionId;  // For each element in the input mesh, stores
                             // the corresponding partition ID.
private:
  vtkMetisKernel(const vtkMetisKernel&); // Not implemented
  void operator=(const vtkMetisKernel&); // Not implemented
};

#endif /* VTKMETISKERNEL_H_ */
