##############################################################################
#
# Library:   TubeTK
#
# Copyright 2010 Kitware Inc. 28 Corporate Drive,
# Clifton Park, NY, 12065, USA.
#
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################

include_regular_expression( "^.*$" )

include( Midas3FunctionAddTest )
set( MIDAS_REST_URL http://midas3.kitware.com/midas/api/rest )
set( MIDAS_KEY_DIR ${TubeTK_SOURCE_DIR}/MIDAS_Keys )

set( TEMP ${TubeTK_BINARY_DIR}/Temporary )

set( PROJ_EXE
 ${TubeTK_BINARY_DIR}/bin/${PROJECT_NAME} )

set( IMAGECOMPARE_EXE
 ${TubeTK_BINARY_DIR}/bin/ImageCompareCommand )

# Test1
Midas3FunctionAddTest( ${PROJECT_NAME}-Test1
             ${PROJ_EXE}
               -i
               -r
               -a Test1 001
               -o ${TEMP}
               -b DSC_
               MIDAS_DIRECTORY{DICOMSeries} )

Midas3FunctionAddTest( ${PROJECT_NAME}-Test1-Compare
             ${IMAGECOMPARE_EXE}
               -t ${TEMP}/DSC_Test1_20030625_361_3__0.86_1.60_3.mha
               -b MIDAS{DSC_Test1_20030625_361_3__0.86_1.60_3.mha.md5} )

set_property( TEST ${PROJECT_NAME}-Test1-Compare
                      APPEND PROPERTY DEPENDS ${PROJECT_NAME}-Test1 )
