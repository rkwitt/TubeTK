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

find_package( PythonInterp )

if( PYTHON_EXECUTABLE )
  add_test( PrintSelf-BaseObjectDocuments ${PYTHON_EXECUTABLE}
    ${TubeTK_SOURCE_DIR}/CMake/PrintSelfCheck.py
    ${TubeTK_SOURCE_DIR}/Base/ObjectDocuments )
endif( PYTHON_EXECUTABLE )

set( BASE_OBJECT_DOCUMENTS_TESTS
  ${TubeTK_BINARY_DIR}/${Slicer4_INSTALL_BIN_DIR}/tubeBaseObjectDocumentsTests )

set( BASE_OBJECT_DOCUMENTS_HEADER_TEST
  ${TubeTK_BINARY_DIR}/${Slicer4_INSTALL_BIN_DIR}/tubeBaseObjectDocumentsHeaderTest )

set( TEMP ${TubeTK_BINARY_DIR}/Temporary )

set( tubeBaseObjectDocumentsTests_SRCS
  tubeBaseObjectDocumentsPrintTest.cxx )

include_directories(
  ${TubeTK_SOURCE_DIR}/Base/Common
  ${TubeTK_SOURCE_DIR}/Base/ObjectDocuments )

add_executable( tubeBaseObjectDocumentsHeaderTest
  tubeBaseObjectDocumentsHeaderTest.cxx )

target_link_libraries( tubeBaseObjectDocumentsHeaderTest
  TubeObjectDocuments
  ${ITK_LIBRARIES} )

add_executable( tubeBaseObjectDocumentsTests
  tubeBaseObjectDocumentsTests.cxx
  ${tubeBaseObjectDocumentsTests_SRCS} )

target_link_libraries( tubeBaseObjectDocumentsTests
  TubeObjectDocuments
  ${ITK_LIBRARIES} )

add_test( tubeBaseObjectDocumentsHeaderTest
  ${BASE_OBJECT_DOCUMENTS_HEADER_TEST} )

add_test( tubeBaseObjectDocumentsPrintTest
  ${BASE_OBJECT_DOCUMENTS_TESTS}
  tubeBaseObjectDocumentsPrintTest )
