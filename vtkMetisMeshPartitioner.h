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
// .NAME vtkMetisMeshPartitioner.h -- Mesh partitioner based on METIS.
//
// .SECTION Description
//  A concrete instance of vtkUnstructuredGridAlgorithm that provides
//  functionality for partitioning a VTK unstructured grid.
//
// .SECTION Caveats
//  In the current implementation, mixed element grids are not supported
//
// .SECTION See Also
//  vtkUnstructuredGridAlgorithm vtkUnstructuredGrid

#ifndef VTKMETISMESHPARTITIONER_H_
#define VTKMETISMESHPARTITIONER_H_

#include "vtkUnstructuredGridAlgorithm.h" // Base class

// Forward VTK Declarations
class vtkIndent;
class vtkInformation;
class vtkInformationVector;
class vtkUnstructuredGrid;

class vtkMetisKernel;


class VTK_EXPORT vtkMetisMeshPartitioner : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkMetisMeshPartitioner* New();
  vtkTypeMacro(vtkMetisMeshPartitioner,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set NumberOfPartitions property
  vtkGetMacro(NumberOfPartitions,int);
  vtkSetMacro(NumberOfPartitions,int);

protected:
  vtkMetisMeshPartitioner();
  virtual ~vtkMetisMeshPartitioner();

  // Standard pipeline operations
  virtual int RequestData( vtkInformation *request,
      vtkInformationVector **inputVector,
      vtkInformationVector *outputVector );
  virtual int FillInputPortInformation(int port, vtkInformation *info);
  virtual int FillOutputPortInformation( int port, vtkInformation *info );

  // Description:
  // Partitions the given grid to the requested number of partitions.
  void Partition(vtkUnstructuredGrid *grid);

  // Class ivars
  int NumberOfPartitions; // The total number of partitions requested by the user

  vtkMetisKernel *MetisKernel;
private:
  vtkMetisMeshPartitioner(const vtkMetisMeshPartitioner&); // Not implemented
  void operator=(const vtkMetisMeshPartitioner&); // Not implemented
};

#endif /* VTKMETISMESHPARTITIONER_H_ */
