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

#include "tubeBrentOptimizer1D.h"
#include "tubeParabolicFitOptimizer1D.h"
#include "tubeSplineApproximation1D.h"
#include "tubeSplineND.h"
#include "tubeUserFunction.h"

#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkMersenneTwisterRandomVariateGenerator.h>

#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

#include <cstdlib>
#include <iostream>

class MySANDFunc : public tube::UserFunction< vnl_vector< int >, double >
{
private:
  double cVal;

public:
  MySANDFunc( void )
    {
    cVal = 0;
    }
  const double & Value( const vnl_vector<int> & x )
    {
    cVal = vcl_sin((double)x[0]/2);
    cVal += vcl_cos((double)x[1]/2);
    return cVal;
    }

}; // End class MySANDFunc

class MySANDFuncV : public tube::UserFunction< vnl_vector< double >, double >
{
private:
  double cVal;

public:
  MySANDFuncV( void )
    {
    cVal = 0;
    }
  const double & Value( const vnl_vector<double> & x )
    {
    cVal = vcl_sin((double)x[0]/2);
    cVal += vcl_cos((double)x[1]/2);
    return cVal;
    }

}; // End class MySANDFuncV

class MySANDFuncD : public tube::UserFunction< vnl_vector< double >,
                                               vnl_vector< double > >
{
private:
  vnl_vector<double> cDeriv;

public:
  MySANDFuncD( void )
    {
    cDeriv.set_size(2);
    cDeriv.fill( 0 );
    }
  const vnl_vector<double> & Value( const vnl_vector<double> & x )
    {
    cDeriv[0] = vcl_cos((double)x[0]/2);
    cDeriv[1] = -vcl_sin((double)x[1]/2);
    return cDeriv;
    }

}; // End class MySANDFuncD

int tubeSplineNDTest( int argc, char * argv[] )
{
  if( argc != 2 )
    {
    std::cout << "usage: run <outImFile>" << std::endl;
    return EXIT_FAILURE;
    }

  MySANDFunc * myFunc = new MySANDFunc();
  MySANDFuncV * myFuncV = new MySANDFuncV();
  MySANDFuncD * myFuncD = new MySANDFuncD();

  tube::SplineApproximation1D * spline1D = new tube::SplineApproximation1D();

  //tube::ParabolicFitOptimizer1D * opt = new tube::ParabolicFitOptimizer1D();
  tube::BrentOptimizer1D * opt = new tube::BrentOptimizer1D();

  tube::SplineND spline( 2, myFunc, spline1D, opt );

  int returnStatus = EXIT_SUCCESS;

  spline.SetClip( true );

  vnl_vector<int> xMin(2, -6);
  spline.SetXMin( xMin );
  if( spline.GetXMin()[0] != -6 )
    {
    std::cout << "xMin should be -6 and not " << spline.GetXMin() << std::endl;
    returnStatus = EXIT_FAILURE;
    }

  vnl_vector<int> xMax(2, 6);
  spline.SetXMax( xMax );
  if( spline.GetXMax()[0] != 6 )
    {
    std::cout << "xMax should be 6 and not " << spline.GetXMax() << std::endl;
    returnStatus = EXIT_FAILURE;
    }

  typedef itk::Image< float, 3 >  ImageType;

  ImageType::Pointer im = ImageType::New();
  ImageType::RegionType imRegion;
  ImageType::SizeType imSize;
  imSize[0] = 60;
  imSize[1] = 60;
  imSize[2] = 20;
  imRegion.SetSize( imSize );
  ImageType::IndexType index0;
  index0[0] = -30;
  index0[1] = -30;
  index0[2] = -10;
  imRegion.SetIndex( index0 );
  im->SetRegions( imRegion );
  ImageType::SpacingType imSpacing;
  imSpacing[0] = 0.2;
  imSpacing[1] = 0.2;
  imSpacing[2] = 0.2;
  im->SetSpacing( imSpacing );
  im->Allocate();

  itk::ImageRegionIteratorWithIndex<ImageType> itIm( im,
    im->GetLargestPossibleRegion() );
  ImageType::PointType pnt;
  itIm.GoToBegin();
  vnl_vector<double> x(2);
  vnl_vector<double> d(2);
  vnl_vector<int> di(2);
  di.fill( 0 );
  di[1] = 1;
  vnl_matrix<double> m(2,2);
  vnl_vector<double> d2(2);
  while( !itIm.IsAtEnd() )
    {
    im->TransformIndexToPhysicalPoint( itIm.GetIndex(), pnt );
    x[0] = pnt[0];
    x[1] = pnt[1];
    switch( (itIm.GetIndex()[2] - index0[2]) % 9 )
      {
      default:
      case 0:
        {
        itIm.Set( spline.Value(x) );
        break;
        }
      case 1:
        {
        itIm.Set( myFuncV->Value(x) );
        break;
        }
      case 2:
        {
        itIm.Set( spline.ValueD(x, di) );
        break;
        }
      case 3:
        {
        itIm.Set( myFuncD->Value(x)[1] );
        break;
        }
      case 4:
        {
        d = spline.ValueD(x);
        itIm.Set( d[0] + d[1] );
        break;
        }
      case 5:
        {
        d = myFuncD->Value(x);
        itIm.Set( d[0] + d[1] );
        break;
        }
      case 6:
        {
        m = spline.Hessian(x);
        itIm.Set( m[0][0]*m[0][0] + m[1][1]*m[1][1] );
        break;
        }
      case 7:
        {
        itIm.Set( spline.ValueJet(x, d, m) );
        break;
        }
      case 8:
        {
        itIm.Set( spline.ValueVDD2(x, d, d2) );
        break;
        }
      }
    ++itIm;
    }

  typedef itk::ImageFileWriter<ImageType> ImageWriterType;
  ImageWriterType::Pointer imWriter = ImageWriterType::New();
  imWriter->SetFileName( argv[1] );
  imWriter->SetInput( im );
  imWriter->SetUseCompression( true );
  imWriter->Update();

  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer rndGen
    = itk::Statistics::MersenneTwisterRandomVariateGenerator::New();
  rndGen->Initialize( 1 );

  spline.GetOptimizerND()->SetSearchForMin( false );
  vnl_vector<double> xStep(2, 0.1);
  spline.GetOptimizerND()->SetXStep( xStep );
  spline.GetOptimizerND()->SetTolerance( 0.0001 );
  spline.GetOptimizerND()->SetMaxIterations( 1000 );
  spline.GetOptimizerND()->SetMaxLineSearches( 40 );
  if( opt->GetSearchForMin() != false )
    {
    std::cout << "SearchForMin not preserved." << std::endl;
    returnStatus = EXIT_FAILURE;
    }
  if( opt->GetTolerance() != 0.0001 )
    {
    std::cout << "tolerance not preserved." << std::endl;
    returnStatus = EXIT_FAILURE;
    }
  opt->SetMaxIterations( 1000 );
  if( opt->GetMaxIterations() != 1000 )
    {
    std::cout << "maxIterations not preserved." << std::endl;
    returnStatus = EXIT_FAILURE;
    }
  int failed = 0;
  for(unsigned int c=0; c<100; c++)
    {
    x[0] = rndGen->GetNormalVariate( 3.14, 0.5 );
    x[1] = rndGen->GetNormalVariate( 0.0, 0.5 );
    std::cout << "Optimizing from " << x[0] << ", " << x[1] << std::endl;
    double xVal = 0;
    if( !spline.Extreme( x, &xVal ) )
      {
      std::cout << "Spline.extreme failed." << std::endl;
      std::cout << "  x = " << x[0] << ", " << x[1] << std::endl;
      std::cout << "  xVal = " << xVal << std::endl;
      returnStatus = EXIT_FAILURE;
      ++failed;
      }
    else
      {
      bool err = false;
      if( vnl_math_abs(x[0] - 3.139) > 0.001
        || vnl_math_abs(x[1] - 0.0) > 0.001 )
        {
        std::cout << "Spline.extreme failed." << std::endl;
        std::cout << "  x = (" << x[0] << ", " << x[1]
          << ") != ideal = (3.1139, 0.0)" << std::endl;
        returnStatus = EXIT_FAILURE;
        err = true;
        }
      if( vnl_math_abs( xVal - 1.918) > 0.001 )
        {
        std::cout << "Spline.extreme failed." << std::endl;
        std::cout << "  xVal=" << xVal << " != 1.918" << std::endl;
        returnStatus = EXIT_FAILURE;
        err = true;
        }
      if( err )
        {
        ++failed;
        }
      else
        {
        std::cout << "Success: x = " << x << " : xVal = "
          << xVal << std::endl;
        }
      }
    }

  delete myFunc;
  delete myFuncV;
  delete myFuncD;
  delete spline1D;
  delete opt;

  std::cout << failed << " out of 100 optimizations failed." << std::endl;

  return returnStatus;
}
