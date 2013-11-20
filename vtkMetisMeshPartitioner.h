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
//  A concrete instance of vtkPointSetAlgorithm that provides
//  functionality for partitioning a VTK dataset.
//
// .SECTION Caveats
//  In the current implementation, mixed element grids are not supported
//
// .SECTION See Also
//  vtkPointSetAlgorithm

#ifndef VTKMETISMESHPARTITIONER_H_
#define VTKMETISMESHPARTITIONER_H_

#include "vtkPointSetAlgorithm.h"

class VTK_EXPORT vtkMetisMeshPartitioner : public vtkPointSetAlgorithm
{
public:
  static vtkMetisMeshPartitioner* New();
  vtkTypeMacro(vtkMetisMeshPartitioner,vtkPointSetAlgorithm);
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

  // Class ivars
  int NumberOfPartitions; // The total number of partitions requested by the user

private:
  vtkMetisMeshPartitioner(const vtkMetisMeshPartitioner&); // Not implemented
  void operator=(const vtkMetisMeshPartitioner&); // Not implemented
};

#endif /* VTKMETISMESHPARTITIONER_H_ */
