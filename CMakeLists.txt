############################################################################
#                                                                          #
#  file: CMakeLists.txt                                                    #
#  Copyright (C) 2022                                                      #
#      Mattia Piazza                                                       #
#      Dipartimento di Ingegneria Industriale                              #
#      Universita` degli Studi di Trento                                   #
#      email: mattia.piazza@unitn.it                                       #
#                                                                          #
############################################################################

cmake_minimum_required(VERSION 3.14)

project( BasicMPLcppProject )
include(./cmake_utils/CMakeLists-common.txt)

project(
  ${PROJECT_NAME}
  VERSION ${UTILS_PROJECT_VERSION}
  HOMEPAGE_URL ""
)

include(./cmake_utils/CMakeLists-cflags.txt)
include(./cmake_utils/CMakeLists-utilities.txt)

# Evaluating the suffix to append to target built by this CMake file
utils_artifacts_suffix(ARTIFACTS_STATIC_SUFFIX TRUE)
if( UTILS_BUILD_SHARED )
  utils_artifacts_suffix(ARTIFACTS_DYNAMIC_SUFFIX FALSE)
endif()

message( STATUS "Compiler used: ${CMAKE_CXX_COMPILER_ID}" )
message( STATUS "BASE SUFFIXES = ${ARTIFACTS_STATIC_SUFFIX} ${ARTIFACTS_DYNAMIC_SUFFIX}" )

#   ___
#  / __| ___ _  _ _ _ __ ___ ___
#  \__ \/ _ \ || | '_/ _/ -_|_-<
#  |___/\___/\_,_|_| \__\___/__/
#
set( SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" )

set( SOURCES )
file( GLOB S ${SOURCE_DIR}/*.cc )
foreach(F ${S})
  # build relative path respect to source directory
  file( RELATIVE_PATH RF ${SOURCE_DIR} "${F}" )
  list( APPEND SOURCES ${RF} )
endforeach(F ${S})

#   _  _             _
#  | || |___ __ _ __| |___ _ _ ___
#  | __ / -_) _` / _` / -_) '_(_-<
#  |_||_\___\__,_\__,_\___|_| /__/
#
set( PUBLIC_HEADERS )
file(
  GLOB S
  ${SOURCE_DIR}/*.h*
  ${SOURCE_DIR}/*/*.h*
)
foreach (F ${S})
  # build relative path respect to source directory
  file( RELATIVE_PATH RF ${SOURCE_DIR} "${F}" )
  list( APPEND PUBLIC_HEADERS ${RF} )
endforeach(F ${S})

# extra include directory for compilation
include_directories( ./lib3rd/include )

#       _                           _                 _
#    __| | ___ _ __   ___ _ __   __| | ___ _ __   ___(_) ___  ___
#   / _` |/ _ \ '_ \ / _ \ '_ \ / _` |/ _ \ '_ \ / __| |/ _ \/ __|
#  | (_| |  __/ |_) |  __/ | | | (_| |  __/ | | | (__| |  __/\__ \
#   \__,_|\___| .__/ \___|_| |_|\__,_|\___|_| |_|\___|_|\___||___/
#             |_|
#
set(UTILS_DEPEND_TARGETS)

# DEPENDENCIES (TRY TO USE ALREADY COMPILED SUBMODULE IN LARGE PROJECT)
utils_add_library_or_submodule( "Utils" UTILS )


message( STATUS "----------------------------------------------------------------" )
message( STATUS "UTILS            = ${UTILS}" )
message( STATUS "----------------------------------------------------------------" )

##################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# find_package(Eigen3)

# if (${EIGEN3_FOUND})
#   include_directories(${EIGEN3_INCLUDE_DIR})
# else()
#   message(STATUS "Eigen3 not found")
# endif()

find_package(Python3 COMPONENTS Interpreter Development)
if (${Python3_FOUND})
  include_directories(${Python3_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Python3 not found, please install it.")
endif()

find_package(NumPy)
if (${PYTHON_NUMPY_FOUND})
  include_directories(${PYTHON_NUMPY_INCLUDE_DIR})
else()
  message(WARNING "Python3 NumPy not found, proceeding with -DWITHOUT_NUMPY."
  " Some functions might not work.")
  add_definitions(-DWITHOUT_NUMPY)
endif()

##################

#   _____                  _
#  |_   _|_ _ _ _ __ _ ___| |_ ___
#    | |/ _` | '_/ _` / -_)  _(_-<
#    |_|\__,_|_| \__, \___|\__/__/
#                |___/
#
# define target
utils_setup_target(
  ${PROJECT_NAME}
  TRUE # build static
  "${SOURCE_DIR}"
  "${SOURCES}"
  "${SOURCE_DIR}"
  "${PUBLIC_HEADERS}"
)
if ( UTILS_BUILD_SHARED )
  utils_setup_target(
    ${PROJECT_NAME}
    FALSE # build dynamic
    "${SOURCE_DIR}"
    "${SOURCES}"
    "${SOURCE_DIR}"
    "${PUBLIC_HEADERS}"
  )
  target_link_libraries( ${UTILS_NAMESPACE}_${PROJECT_NAME} ${UTILS_MODULE_LIBS} ${Python3_LIBRARIES} )
endif()

if ( UTILS_DEPEND_TARGETS )
  add_dependencies( ${UTILS_NAMESPACE}_${PROJECT_NAME}_Static ${UTILS_DEPEND_TARGETS} )
endif()

#   _____       _
#  |_   _|__ __| |_ ___
#    | |/ -_|_-<  _(_-<
#    |_|\___/__/\__/__/
#
if ( UTILS_ENABLE_TESTS )

  enable_testing()

  if(APPLE OR UNIX OR MINGW )
    find_package( ZLIB REQUIRED )
  else()
    if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
      find_library( ZLIB_LIBRARIES zlibstaticd HINTS ./lib3rd/lib )
    else()
      find_library( ZLIB_LIBRARIES zlibstatic HINTS ./lib3rd/lib )
    endif()
    set( ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/lib3rd/zlib/include ")
  endif()

  set( LIBS ${UTILS_NAMESPACE}_${PROJECT_NAME}_Static ${UTILS} ${ZLIB_LIBRARIES} ${CMAKE_DL_LIBS} ${Python3_LIBRARIES} )

  if( UNIX )
    if ( NOT APPLE )
      set( THREADS_PREFER_PTHREAD_FLAG ON )
      find_package( Threads REQUIRED )
      set( LIBS ${LIBS} Threads::Threads )
    endif()
  else()
    #libcmt.lib
    #find_library( WS2 ws2_32 REQUIRED )
    #find_library( IPH iphlpapi REQUIRED )
    #find_library( KER kernel32 REQUIRED )
    #find_library( IPP ippsmt REQUIRED )
    #find_library( WPTH pthread REQUIRED )
    #set( LIBS ${LIBS} ${WS2} ${IPH} ${KER} )
    set( LIBS ${LIBS} ws2_32 iphlpapi kernel32 )
    if (UTILS_BUILD_SHARED)
      target_link_libraries( ${UTILS_NAMESPACE}_${PROJECT_NAME} ${LIBS} ${ZLIB_LIBRARIES} ${CMAKE_DL_LIBS} ${Python3_LIBRARIES} )
    endif()
  endif()

  set(
    EXELISTCPP
    test0
    test1
    basic
  )

  add_custom_target( "${PROJECT_NAME}_all_tests" ALL )

  set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin )
  foreach( S ${EXELISTCPP} )
    add_executable( ${S} ${CMAKE_CURRENT_SOURCE_DIR}/tests/${S}.cc )
    target_link_libraries( ${S} ${LIBS} )
    set_target_properties( ${S} PROPERTIES SUFFIX ".exe" )
    add_test( NAME "${S}" COMMAND ./bin/${S}.exe WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    add_dependencies( "${PROJECT_NAME}_all_tests" ${S} )
  endforeach()

endif()

#   ___         _        _ _
#  |_ _|_ _  __| |_ __ _| | |
#   | || ' \(_-<  _/ _` | | |
#  |___|_||_/__/\__\__,_|_|_|
#
install(
  TARGETS ${TARGETS_TO_BE_INSTALLED}
  # EXPORT (da definire)
  LIBRARY DESTINATION ${INSTALL_PREFIX}/dll
  ARCHIVE DESTINATION ${INSTALL_PREFIX}/lib
  RUNTIME DESTINATION ${INSTALL_PREFIX}/bin
  PUBLIC_HEADER DESTINATION ${PROJECT_PUBLIC_HEADER_DESTINATION}
)

#   ___         _
#  | _ \__ _ __| |____ _ __ _ ___
#  |  _/ _` / _| / / _` / _` / -_)
#  |_| \__,_\__|_\_\__,_\__, \___|
#                       |___/
#

include( ./cmake_utils/CMakeLists-cpack.txt )

utils_final_messages()
