/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itktubeSyntheticTubeImageGenerationTest.cxx,v $
  Language:  C++
  Date:      $Date: 2012/03/19
  Version:   $Revision: 1.0 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itktubeImageToTubeRigidMetric.h"
#include "itktubeTubeToTubeTransformFilter.h"

#include <itkEuler3DTransform.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
#include <itkRecursiveGaussianImageFilter.h>
#include <itkSpatialObjectReader.h>
#include <itkSpatialObjectToImageFilter.h>
#include <itkSpatialObjectWriter.h>
#include <itkTubeSpatialObjectPoint.h>
#include <itkVesselTubeSpatialObjectPoint.h>

/**
 *  This test is a base to generate images and spatial objects for the
 *  registration/metric testing process.
 */

int itktubeSyntheticTubeImageGenerationTest( int argc, char * argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Missing Parameters: "
              << argv[0]
              << " Output_BlurredTubeImage "
              << "Output_VesselTube "
              << "Output_VesselTubeImage "
              << "Input_VesselTubeManuallyModified "
              << "Output_TransformedVesselTubeImage."
              << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image<double, 3>                             Image3DType;
  typedef itk::ImageRegionIteratorWithIndex< Image3DType >  Image3DIteratorType;
  typedef itk::TubeSpatialObject<3>                         TubeType;
  typedef itk::VesselTubeSpatialObjectPoint<3>              TubePointType;
  typedef itk::GroupSpatialObject<3>                        TubeNetType;

  typedef itk::SpatialObjectToImageFilter< TubeNetType, Image3DType >
    SpatialObjectToImageFilterType;
  typedef itk::Euler3DTransform<double> TransformType;
  typedef itk::tube::TubeToTubeTransformFilter<TransformType, 3>
    TubeTransformFilterType;

  typedef itk::ImageFileWriter<Image3DType>                 ImageWriterType;
  typedef itk::SpatialObjectWriter<3>                       TubeWriterType;

  Image3DType::SizeType imageSize;
  imageSize[0] = 32;
  imageSize[1] = 32;
  imageSize[2] = 32;

  //------------------------------------------------------------------
  // Generate a simple tube image using Gaussian Filter
  //------------------------------------------------------------------
  std::cout << "Generate a tube blured image..." << std::endl;
  Image3DType::Pointer fixedImage = Image3DType::New();
  fixedImage->SetRegions(imageSize);
  fixedImage->Allocate();
  fixedImage->FillBuffer(0);
  fixedImage->Update();

  std::cout << "Start Filling Images (Square Tube)..." << std::endl;
  Image3DIteratorType fixedIt( fixedImage, fixedImage->GetBufferedRegion() );
  int pixelIndex = 1;
  for( fixedIt.GoToBegin(); !fixedIt.IsAtEnd(); ++fixedIt, ++pixelIndex )
    {
    Image3DType::IndexType index = fixedIt.GetIndex();
    if((index[0]>=15)&&(index[0]<=25)&&(index[1]>=15)&&(index[1]<=25))
      {
        fixedIt.Set(255 - 20 * (pixelIndex % 5)); // Brighter center
      }
    }

  // guassian blur the images to increase the likelihood of vessel
  // spatial object overlapping.
  std::cout << "Apply gaussian blur..." << std::endl;
  typedef itk::RecursiveGaussianImageFilter<Image3DType, Image3DType>
    GaussianBlurFilterType;
  GaussianBlurFilterType::Pointer blurFilters[3];
  for(int i = 0; i < 3; i++)
    {
    blurFilters[i] = GaussianBlurFilterType::New();
    blurFilters[i]->SetSigma(3.0);
    blurFilters[i]->SetZeroOrder();
    blurFilters[i]->SetDirection(i);
    }
  blurFilters[0]->SetInput(fixedImage);
  blurFilters[1]->SetInput(blurFilters[0]->GetOutput());
  blurFilters[2]->SetInput(blurFilters[1]->GetOutput());
  try
    {
    blurFilters[0]->Update();
    blurFilters[1]->Update();
    blurFilters[2]->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }

  // write image
  ImageWriterType::Pointer imageWriter = ImageWriterType::New();
  imageWriter->SetFileName( argv[1] );
  imageWriter->SetUseCompression( true );
  std::cout << "Write imageFile: " << argv[1] << std::endl;
  imageWriter->SetInput( blurFilters[2]->GetOutput() );
  try
    {
    imageWriter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------
  // Generate a simple spatial object tube
  //------------------------------------------------------------------
  std::cout << "Create spatial object tube..." << std::endl;
  TubeType::Pointer tube = TubeType::New();

  // Try to add the metaData about the vessel object subtype
  // There is currently some issues on it with ITK.
  // See:
  // http://www.itk.org/Wiki/ITK/Examples/Broken/SimpleOperations/MetaDataDictionary
  // http://public.kitware.com/Bug/view.php?id=12329#bugnotes
  itk::MetaDataDictionary& tubeMetaDictionary = tube->GetMetaDataDictionary();
  itk::EncapsulateMetaData<std::string>( tubeMetaDictionary,
                                         "ObjectSubType",
                                         "Vessel" );

  TubePointType point;
  point.SetRadius( 2.0 );

  for(int i = -550; i < 550; ++i)
    {
    point.SetPosition( 15, 15, i / 10.);
    tube->GetPoints().push_back(point);
    }

  TubeNetType::Pointer group = TubeNetType::New();
  group->AddSpatialObject( tube );

  std::cout << "Write tubeFile: " << argv[2] << std::endl;
  TubeWriterType::Pointer tubeWriter = TubeWriterType::New();
  tubeWriter->SetFileName( argv[2] );
  tubeWriter->SetInput( group );
  try
    {
    tubeWriter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------
  // Write the tube as an image without transformation
  //------------------------------------------------------------------
  std::cout << "Convert the tube into an Image..." << std::endl;
  SpatialObjectToImageFilterType::Pointer imageFilter =
    SpatialObjectToImageFilterType::New();
  imageFilter->SetInput( group );
  imageFilter->SetSize( imageSize );

  Image3DType::PointType origin;
  origin[0] = 0;
  origin[1] = 0;
  origin[2] = 0;
  imageFilter->SetOrigin( origin );
  imageFilter->Update();

  // write image
  ImageWriterType::Pointer imageTubeWriter = ImageWriterType::New();
  imageTubeWriter->SetFileName( argv[3] );
  std::cout << "Write tubeAsImageFile: " << argv[3] << std::endl;
  imageTubeWriter->SetInput( imageFilter->GetOutput() );
  try
    {
    imageTubeWriter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }

  //------------------------------------------------------------------
  // Write the tube as an image with a transformation
  //------------------------------------------------------------------
  std::cout << "Transform and Convert the tube into an Image..." << std::endl;

  // read tube (spatialObject)
  typedef itk::SpatialObjectReader<3> TubeNetReaderType;
  TubeNetReaderType::Pointer tubeReader = TubeNetReaderType::New();
  std::cout << "Read VesselTube: " << argv[4] << std::endl;
  tubeReader->SetFileName( argv[4] );
  try
    {
    tubeReader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }

  TransformType::Pointer transformTube = TransformType::New();

  TubeType::VectorType translateT;
  translateT[0] = 2.5;
  translateT[1] = 2.5;
  translateT[2] = 2.5;
  transformTube->Translate( translateT );

  TubeType::ScalarType angleX = 0;
  TubeType::ScalarType angleY = 5;
  TubeType::ScalarType angleZ = 0;

  transformTube->Translate( translateT );
  transformTube->SetRotation( angleX, angleY, angleZ );

  itk::Matrix<double,3,3> rotationMatrix;
  itk::Vector<double,3> translation;

  rotationMatrix = transformTube->GetMatrix();
  translation = transformTube->GetTranslation();

  std::cout << rotationMatrix(0,0) << " " << rotationMatrix(0,1)
            << " " << rotationMatrix(0,2) << std::endl;
  std::cout << rotationMatrix(1,0) << " " << rotationMatrix(1,1)
            << " " << rotationMatrix(1,2) << std::endl;
  std::cout << rotationMatrix(2,0) << " " << rotationMatrix(2,1)
            << " " << rotationMatrix(2,2) << std::endl;
  std::cout << translation[0] << " " << translation[1]
            << " " << translation[2] << std::endl;

  // create transform filter
  TubeTransformFilterType::Pointer transformFilter =
    TubeTransformFilterType::New();
  transformFilter->SetScale( 1.0 );
  transformFilter->SetInput( tubeReader->GetGroup() );
  transformFilter->SetTransform( transformTube );

  try
    {
    transformFilter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }

  SpatialObjectToImageFilterType::Pointer imageFilterTransform =
    SpatialObjectToImageFilterType::New();
  imageFilterTransform->SetInput( transformFilter->GetOutput() );
  imageFilterTransform->SetSize( imageSize );
  imageFilterTransform->SetOrigin( origin );
  imageFilterTransform->Update();

  // write image
  ImageWriterType::Pointer imageTubeWriterT = ImageWriterType::New();
  imageTubeWriterT->SetFileName( argv[5] );
  std::cout << "Write transformedTubeAsImageFile: " << argv[5] << std::endl;
  imageTubeWriterT->SetInput( imageFilterTransform->GetOutput() );
  try
    {
    imageTubeWriterT->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "Exception caught: " << err << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
