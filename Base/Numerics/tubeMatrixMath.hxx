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

#ifndef __tubeMatrixMath_hxx
#define __tubeMatrixMath_hxx

#include "tubeMatrixMath.h"

#include <vnl/algo/vnl_symmetric_eigensystem.h>

namespace tube
{

/**
 * Compute the orthogonal vector of one vector
 * Do not check for wrong dimension             */
template< class T >
vnl_vector<T>
ComputeOrthogonalVector(vnl_vector<T>  v)
{
  // create the orthogonal vector
  vnl_vector<T> n(v.size());

  /** if the dimension is 2
   *  purely arbitrary sign */
  if(v.size() == 2)
    {
    n(0)=v(1);
    n(1)=-v(0);
    }
  /** if the dimension is 3 */
  if(v.size() == 3)
    {
    vnl_vector< T > tt(3);

    tt[0] = -v[1];
    tt[1] = v[0];
    tt[2] = 0.5;
    tt.normalize();

    n = ComputeCrossVector( v, tt );
    }

  return n;
}


/**
 * Compute the cross vector in 3D resulting from two vectors
 * Do not check for wrong dimension
 * could use vnl_cross3D instead        */
template< class T >
vnl_vector<T>
ComputeCrossVector(vnl_vector<T> v1, vnl_vector<T> v2)
{
  vnl_vector<T> dest(v1.size());
  dest(0) = ((v1)(1) * (v2)(2)) - ((v1)(2) * (v2)(1));
  dest(1) = ((v1)(2) * (v2)(0)) - ((v1)(0) * (v2)(2));
  dest(2) = ((v1)(0) * (v2)(1)) - ((v1)(1) * (v2)(0));

  return dest;
}


/**
 *return the new position folowing the vector dir */
template< class T >
vnl_vector<T>
ComputeLineStep(vnl_vector<T> x, double a, vnl_vector<T> dir)
{
  int n = x.size();
  vnl_vector<T> destX(n);
  for(int i=0; i<n; i++)
    {
    destX(i) = (T)(x(i) + a * dir(i));
    }

  return destX;
}


/**
 * Compute the Euclidean distance  */
template< class T >
double
ComputeEuclideanDistanceVector(vnl_vector<T> x, const vnl_vector<T> y)
{
  double s = 0;
  for(unsigned int i=0; i<x.size(); i++)
    {
    s += (x(i)-y(i))*(x(i)-y(i));
    }
  return vcl_sqrt(s);
}

/**
 * Compute the Euclidean distance  */
template< class TPoint >
double
ComputeEuclideanDistance( TPoint x, TPoint y )
{
  double s = 0;
  for( unsigned int i = 0; i < TPoint::PointDimension; i++)
    {
    s += (x[i]-y[i])*(x[i]-y[i]);
    }
  return vcl_sqrt(s);
}

/**
 * Compute eigen values and vectors  */
template< class T >
void
ComputeEigen( vnl_matrix<T> const & mat,
  vnl_matrix<T> &eVects, vnl_vector<T> &eVals,
  bool orderByAbs, bool minToMax )
{

  int n = mat.rows();

  vnl_vector<T> subD(n);

  eVects = mat;
  eVals.set_size( n );
  switch(n)
    {
    case 1:
      eVects.set_size(1,1);
      eVects.fill( 1 );
      eVals.set_size(1);
      eVals.fill( mat[0][0] );
      break;
    case 2:
      ComputeTriDiag2D(eVects, eVals, subD);
      ComputeTqli(eVals, subD, eVects);
      break;
    case 3:
      ComputeTriDiag3D(eVects, eVals, subD);
      ComputeTqli(eVals, subD, eVects);
      break;
    default:
      vnl_symmetric_eigensystem< T > eigen( mat );
      eVects = eigen.V;
      eVals.set_size( eVects.columns() );
      for( unsigned int d=0; d<eVects.columns(); d++ )
        {
        eVals[d] = eigen.get_eigenvalue( d );
        }
      break;
    }

  if(orderByAbs)
    {
    for(int i=0; i<n-1; i++)
      {
      for(int j=i+1; j<n; j++)
        {
        if( ( vnl_math_abs(eVals(j))>vnl_math_abs(eVals(i)) && !minToMax )
          || ( vnl_math_abs(eVals(j))<vnl_math_abs(eVals(i)) && minToMax ) )
          {
          T tf = eVals(j);
          eVals(j) = eVals(i);
          eVals(i) = tf;
          for(int k=0; k<n; k++)
            {
            tf = eVects(k,j);
            eVects(k,j) = eVects(k,i);
            eVects(k,i) = tf;
            }
          }
        }
      }
    }
  else
    {
    for(int i=0; i<n-1; i++)
      {
      for(int j=i+1; j<n; j++)
        {
        if( ( eVals(j)>eVals(i) && !minToMax )
          || ( eVals(j)<eVals(i) && minToMax ) )
          {
          T tf = eVals(j);
          eVals(j) = eVals(i);
          eVals(i) = tf;
          for(int k=0; k<n; k++)
            {
            tf = eVects(k,j);
            eVects(k,j) = eVects(k,i);
            eVects(k,i) = tf;
            }
          }
        }
      }
    }
}


/**
 * Preform trilinear diagonalisation in 2D */
template< class T >
void
ComputeTriDiag2D(vnl_matrix<T> &mat,
  vnl_vector<T> &diag, vnl_vector<T> &subD)
{
  diag(0) = mat(0,0);
  diag(1) = mat(1,1);
  subD(0) = mat(0,1);
  subD(1) = 0;

  mat(0,0) = 1;
  mat(0,1) = 0;

  mat(1,0) = 0;
  mat(1,1) = 1;
}

/**
 * Preform trilinear diagonalisation in 3D */
template< class T >
void
ComputeTriDiag3D(vnl_matrix<T> &mat,
  vnl_vector<T> &diag, vnl_vector<T> &subD)
{
  double  a = mat(0,0), b = mat(0,1), c = mat(0,2),
          d = mat(1,1), e = mat(1,2), f = mat(2,2);

  diag(0) = static_cast< T >( a );
  subD(2) = 0;
  if(c != 0)
    {
    const double s = vcl_sqrt(b*b+c*c);
    b /= s;
    c /= s;
    const double q = 2*b*e+c*(f-d);
    diag(1) = static_cast< T >( d+c*q );
    diag(2) = static_cast< T >( f-c*q );
    subD(0) = static_cast< T >( s );
    subD(1) = static_cast< T >( e-b*q );

    mat(0,0) = 1;
    mat(0,1) = 0;
    mat(0,2) = 0;

    mat(1,0) = 0;
    mat(1,1) = static_cast< T >( b );
    mat(1,2) = static_cast< T >( c );

    mat(2,0) = 0;
    mat(2,1) = static_cast< T >( c );
    mat(2,2) = static_cast< T >( -b );
    }
  else
    {
    diag(1) = static_cast< T >( d );
    diag(2) = static_cast< T >( f );
    subD(0) = static_cast< T >( b );
    subD(1) = static_cast< T >( e );

    mat(0,0) = 1;
    mat(0,1) = 0;
    mat(0,2) = 0;

    mat(1,0) = 0;
    mat(1,1) = 1;
    mat(1,2) = 0;

    mat(2,0) = 0;
    mat(2,1) = 0;
    mat(2,2) = 1;
    }
}

template< class T >
void
ComputeTqli (vnl_vector<T> &diag, vnl_vector<T> &subD, vnl_matrix<T> &mat)
{
  int iter;
  int i;
  int k;
  int l;
  int m;

  double dd;
  double g;
  double r;
  double f;
  double s;
  double c;
  double p;
  double b;

  int n = mat.rows();

  for(l=0; l<n; l++)
    {
    for(iter = 0; iter < EIGEN_MAX_ITERATIONS; iter++)
      {
      for(m=l; m<n; m++)
        {
        if(m!=(n-1))
          {
          dd = vnl_math_abs(diag(m))+vnl_math_abs(diag(m+1));
          }
        else
          {
          dd = vnl_math_abs(diag(m));
          }

        if(vnl_math_abs(subD(m))+dd == dd)
          {
          break;
          }
        }
      if(m == l)
        {
        break;
        }
      g = (diag(l+1)-diag(l))/(2*subD(l));
      r = vcl_sqrt(g*g+1);
      if(g<0)
        {
        g = diag(m)-diag(l)+subD(l)/(g-r);
        }
      else
        {
        g = diag(m)-diag(l)+subD(l)/(g+r);
        }
      s = 1;
      c = 1;
      p = 0;
      for(i=m-1; i>=l; i--)
        {
        f = s*subD(i);
        b = c*subD(i);
        if(vnl_math_abs(f)>=vnl_math_abs(g))
          {
          c = g/f;
          r = vcl_sqrt(c*c+1);
          subD(i+1) = static_cast< T >( f*r );
          c *= (s = 1/r);
          }
        else
          {
          s = f/g;
          r = vcl_sqrt(s*s+1);
          subD(i+1) = static_cast< T >( g*r );
          s *= (c = 1/r);
          }
        g = diag(i+1)-p;
        r = (diag(i)-g)*s+2*b*c;
        p = s*r;
        diag(i+1) = static_cast< T >( g+p );
        g = c*r-b;

        for(k=0; k<n; k++)
          {
          f = mat(k,i+1);
          mat(k,i+1) = static_cast< T >( s*mat(k,i)+c*f );
          mat(k,i) = static_cast< T >( c*mat(k,i)-s*f );
          }
        }
      diag(l) -= static_cast< T >( p );
      subD(l) = static_cast< T >( g );
      subD(m) = 0;
      }
    if(iter == EIGEN_MAX_ITERATIONS)
      {
      throw("NR_tqli - exceeded maximum iterations\n");
      }
    }
}

} // End namespace tube

#endif // End !defined(__tubeMatrixMath_hxx)
