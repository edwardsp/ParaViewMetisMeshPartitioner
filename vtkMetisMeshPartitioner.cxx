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
#include "vtkDataObject.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGrid.h"

// C/C++ includes
#include <cassert>

#include "vtkMetisKernel.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMetisMeshPartitioner);

//------------------------------------------------------------------------------
vtkMetisMeshPartitioner::vtkMetisMeshPartitioner()
{
  this->NumberOfPartitions = 5;
  this->SetNumberOfInputPorts( 1 );
  this->SetNumberOfOutputPorts( 1 );

  this->MetisKernel = vtkMetisKernel::New();
}

//------------------------------------------------------------------------------
vtkMetisMeshPartitioner::~vtkMetisMeshPartitioner()
{
  this->MetisKernel->Delete();
}

//------------------------------------------------------------------------------
void vtkMetisMeshPartitioner::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//------------------------------------------------------------------------------
int vtkMetisMeshPartitioner::FillOutputPortInformation(
      int vtkNotUsed(port), vtkInformation *info)
{
  assert("pre: input information is NULL!" && (info != NULL) );
  info->Set(vtkDataObject::DATA_TYPE_NAME(),"vtkUnstructuredGrid");
  return 1;
}

//------------------------------------------------------------------------------
int vtkMetisMeshPartitioner::FillInputPortInformation(
      int vtkNotUsed(port), vtkInformation *info)
{
  assert("pre: input information is NULL!" && (info != NULL) );
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(),"vtkUnstructuredGrid");
  return 1;
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
  vtkUnstructuredGrid *inputGrid =
      vtkUnstructuredGrid::SafeDownCast(
          input->Get( vtkDataObject::DATA_OBJECT() ) );
  assert("pre: input grid is NULL!" && (inputGrid != NULL) );

  // STEP 1: Get output object
  vtkInformation *output = outputVector->GetInformationObject( 0 );
  assert("pre: output information object is NULL!" && (output != NULL) );
  vtkUnstructuredGrid *outputGrid =
      vtkUnstructuredGrid::SafeDownCast(
          output->Get(vtkDataObject::DATA_OBJECT() ) );

  // STEP 2: Shallow copy input object to output
  outputGrid->ShallowCopy( inputGrid );

  // STEP 3: Short-circuit here if we are not partitioning
  if( this->NumberOfPartitions < 2 )
    {
    return 1;
    }

  this->MetisKernel->Partition(
      outputGrid,this->NumberOfPartitions, true);

  return 1;
}
