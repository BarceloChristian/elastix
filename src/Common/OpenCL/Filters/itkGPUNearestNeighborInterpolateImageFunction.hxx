/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/
#ifndef __itkGPUNearestNeighborInterpolateImageFunction_hxx
#define __itkGPUNearestNeighborInterpolateImageFunction_hxx

#include "itkGPUNearestNeighborInterpolateImageFunction.h"
#include "itkGPUImageFunction.h"
#include <iomanip>

namespace itk
{
template< class TInputImage, class TCoordRep >
GPUNearestNeighborInterpolateImageFunction< TInputImage, TCoordRep >
::GPUNearestNeighborInterpolateImageFunction()
{
  // Add GPUImageFunction implementation
  const std::string sourcePath0(
    GPUImageFunctionKernel::GetOpenCLSource() );

  m_Sources.push_back( sourcePath0 );

  // Add GPUNearestNeighborInterpolateImageFunction implementation
  const std::string sourcePath1(
    GPUNearestNeighborInterpolateImageFunctionKernel::GetOpenCLSource() );
  m_Sources.push_back( sourcePath1 );

  m_SourcesLoaded = true; // we set it to true, sources are loaded from strings
}

//------------------------------------------------------------------------------
template< class TInputImage, class TCoordRep >
bool GPUNearestNeighborInterpolateImageFunction< TInputImage, TCoordRep >
::GetSourceCode( std::string & _source ) const
{
  if ( !m_SourcesLoaded )
  {
    return false;
  }

  // Create the source code
  std::ostringstream source;
  for ( size_t i = 0; i < m_Sources.size(); i++ )
  {
    source << m_Sources[i] << std::endl;
  }

  _source = source.str();
  return true;
}

//------------------------------------------------------------------------------
template< class TInputImage, class TCoordRep >
void GPUNearestNeighborInterpolateImageFunction< TInputImage, TCoordRep >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  CPUSuperclass::PrintSelf( os, indent );
  GPUSuperclass::PrintSelf( os, indent );
}
} // end namespace itk

#endif /* __itkGPUNearestNeighborInterpolateImageFunction_hxx */