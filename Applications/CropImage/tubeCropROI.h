/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/

#ifndef __tubeCropROI_h
#define __tubeCropROI_h

#include "tubeCLIFilterWatcher.h"
#include "tubeCLIProgressReporter.h"
#include "tubeMessage.h"

#include <itkCropImageFilter.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkTimeProbesCollectorBase.h>

namespace tube
{

// Your code should be within the DoIt function...
template< class TPixel, unsigned int VDimension >
class CropROI
{
public:

  typedef TPixel                                       PixelType;
  typedef itk::Image< PixelType, VDimension>           ImageType;
  typedef itk::CropImageFilter< ImageType, ImageType > CropFilterType;

  CropROI( void );
  ~CropROI( void );

  void SetInput( typename ImageType::Pointer volImage );

  void SetMin( typename ImageType::IndexType roiMin );
  void SetUseMin( bool useMin );

  void SetMax( typename ImageType::IndexType roiMax );
  void SetUseMax( bool useMax );

  void SetSize( typename ImageType::SizeType roiSize );
  void SetUseSize( bool useSize );

  void SetCenter( typename ImageType::IndexType roiCenter );
  void SetUseCenter( bool useCenter );

  void SetBoundary( typename ImageType::IndexType roiBoundary );
  void SetUseBoundary( bool useBoundary );

  void SetTimeCollector( itk::TimeProbesCollectorBase * timeCollector );

  void SetProgressReporter( CLIProgressReporter * progessReporter,
                            float progressStart,
                            float progressRange );

  void Update( void );

  typename ImageType::Pointer GetOutput( void );

  void PrintSelf( void );

private:

  typename ImageType::Pointer    m_InputImage;
  typename ImageType::Pointer    m_OutputImage;

  typename ImageType::IndexType  m_ROIMin;
  bool                           m_UseROIMin;
  typename ImageType::IndexType  m_ROIMax;
  bool                           m_UseROIMax;
  typename ImageType::SizeType   m_ROISize;
  bool                           m_UseROISize;
  typename ImageType::IndexType  m_ROICenter;
  bool                           m_UseROICenter;
  typename ImageType::IndexType  m_ROIBoundary;
  bool                           m_UseROIBoundary;

  itk::TimeProbesCollectorBase * m_TimeCollector;
  CLIProgressReporter          * m_ProgressReporter;
  float                          m_ProgressStart;
  float                          m_ProgressRange;

}; // End class CropROI

} // End namespace tube

#include "tubeCropROI.hxx"

#endif // End !defined(__tubeCropROI_h)
