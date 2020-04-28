#---------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0012 NEW)
if(POLICY CMP0033)
  # This policy was introduced in cmake 3, so we need to check for its
  # existence. When the minimum cmake version above is upgraded to 3
  # this if(POLICY) check can be removed.
  cmake_policy(SET CMP0033 OLD)
endif()

#---------------------------------------------------------------------
project(elastix)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
#---------------------------------------------------------------------
# Generate executable, static library or dynamic library.
mark_as_advanced(ELASTIX_BUILD_EXECUTABLE)
option(ELASTIX_BUILD_EXECUTABLE "Generate executable or library?" OFF)

if(NOT ELASTIX_BUILD_EXECUTABLE)
  add_definitions(-D_ELASTIX_BUILD_LIBRARY)
  mark_as_advanced(ELASTIX_BUILD_SHARED_LIBS)
  option(ELASTIX_BUILD_SHARED_LIBS "Build shared libraries?" ON)
  if(ELASTIX_BUILD_SHARED_LIBS)
    add_definitions(-D_ELASTIX_BUILD_SHARED_LIBRARY)
  endif()
endif()

#---------------------------------------------------------------------
# Find ITK.
find_package(ITK REQUIRED)
if (EXISTS ${ITK_USE_FILE})
  include(${ITK_USE_FILE})
else()
  MESSAGE(FATAL_ERROR "ITK Use file not found")
endif()

#---------------------------------------------------------------------
# Add the CMake dir as an additional module path,
# so that CMake is able to find the FindPackage modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

#---------------------------------------------------------------------
# Find CUDA.
mark_as_advanced(ELASTIX_USE_CUDA)
option(ELASTIX_USE_CUDA "Use CUDA enabled GPU" OFF)

if(ELASTIX_USE_CUDA)
  find_package(CUDA REQUIRED)

  mark_as_advanced(CUDA_BUILD_CUBIN)
  mark_as_advanced(CUDA_BUILD_EMULATION)
  mark_as_advanced(CUDA_VERBOSE_BUILD)
  mark_as_advanced(CUDA_SDK_ROOT_DIR)
endif()

#---------------------------------------------------------------------
# Find OpenCL.
option(ELASTIX_USE_OPENCL "Use OpenCL enabled GPU" OFF)

if(ELASTIX_USE_OPENCL)
  find_package(OpenCL REQUIRED)
  # Force OPENCL_OPTIMIZATION_MAD_ENABLE to on
  if(DEFINED OPENCL_OPTIMIZATION_MAD_ENABLE)
    set(OPENCL_OPTIMIZATION_MAD_ENABLE ON CACHE BOOL
      "Allow a * b + c to be replaced by a mad. The mad computes a * b + c with reduced accuracy." FORCE)
  endif()

  # Include the OpenCL include directories to elastix
  include_directories(${OPENCL_INCLUDE_DIRS})

  # Add some useful macro's
  include(elastixOpenCL)

  # Add definition for the OpenCL
  add_definitions(-DELASTIX_USE_OPENCL)
endif()

#---------------------------------------------------------------------
# Find Eigen
mark_as_advanced(ELASTIX_USE_EIGEN)
option(ELASTIX_USE_EIGEN "Use Eigen library" OFF)

if(ELASTIX_USE_EIGEN)
  find_package(Eigen3 REQUIRED)
  include_directories(${EIGEN3_INCLUDE_DIR})
  add_definitions(-DELASTIX_USE_EIGEN)
endif()

#---------------------------------------------------------------------
# Find OpenMP
find_package(OpenMP)

if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  add_definitions(-DELASTIX_USE_OPENMP)
endif()

#----------------------------------------------------------------------
# Check for the SuiteSparse package
# We need to do that here, because the link_directories should be set
# before declaring any targets.

if(USE_PreconditionedGradientDescent)
 set(SUITESPARSE_BINARY_DIR "" CACHE PATH "Path to SuiteSparse")
 set(SUITESPARSE_USE_FILE ${SUITESPARSE_BINARY_DIR}/UseSuiteSparse.cmake)
 if(EXISTS ${SUITESPARSE_USE_FILE})
  include(${SUITESPARSE_USE_FILE})
 endif()
endif()

#---------------------------------------------------------------------
# Set single output directories for all executables and libraries.
# This makes it easier to create an elxUseFile.cmake, that users can
# include in their programs to borrow elastix functionality.

if(NOT LIBRARY_OUTPUT_PATH)
  set(LIBRARY_OUTPUT_PATH ${elastix_BINARY_DIR}/bin CACHE INTERNAL
    "Single output directory for building all libraries.")
endif()

if(NOT EXECUTABLE_OUTPUT_PATH)
  set(EXECUTABLE_OUTPUT_PATH ${elastix_BINARY_DIR}/bin CACHE INTERNAL
    "Single output directory for building all executables.")
endif()

# Mark these variables as advanced; their default value is usually fine
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#---------------------------------------------------------------------
# Check if Mevis DicomTiff support is desired
mark_as_advanced(ELASTIX_USE_MEVISDICOMTIFF)
option(ELASTIX_USE_MEVISDICOMTIFF
  "Support MevisLab DicomTiff image format" OFF)

#---------------------------------------------------------------------
# Define cmake variable to define extra user component directories
# These directories will be added to the list of include directories
# and they will be searched for CMakeLists.txt files for further
# processing. In these directories, users may put code for their own
# components.

mark_as_advanced(ELASTIX_USER_COMPONENT_DIRS)
set(ELASTIX_USER_COMPONENT_DIRS "" CACHE PATH
  "directories with user defined elastix components")

#---------------------------------------------------------------------
# If IDE supports it, do use folder view.
# gcc automatically ignores it. VC Express does not support and gives
# annoying warnings when this option is turned on.
# VC pro does support it.
set(CMAKE_USE_FOLDERS ON CACHE INTERNAL "Use folder view in IDE")
if(CMAKE_MAKE_PROGRAM MATCHES ".?VCExpress.?")
  set(CMAKE_USE_FOLDERS OFF CACHE INTERNAL "Use folder view in IDE")
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ${CMAKE_USE_FOLDERS})

#---------------------------------------------------------------------
# Set default build type to Release, if none was specified
# Taken from ITK CMake list
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#---------------------------------------------------------------------
# Include directories

set(elxCommon_INCLUDE_DIRECTORIES
  ${elastix_SOURCE_DIR}/Common
  ${elastix_SOURCE_DIR}/Common/CostFunctions
  ${elastix_SOURCE_DIR}/Common/CUDA
  ${elastix_SOURCE_DIR}/Common/ImageSamplers
  ${elastix_SOURCE_DIR}/Common/KNN
  ${elastix_SOURCE_DIR}/Common/KNN/ann_1.1/include
  ${elastix_SOURCE_DIR}/Common/LineSearchOptimizers
  ${elastix_SOURCE_DIR}/Common/ParameterFileParser
  ${elastix_SOURCE_DIR}/Common/Transforms
  ${elastix_SOURCE_DIR}/Common/xout
  ${elastix_SOURCE_DIR}/Common/MevisDicomTiff
)

set(elxCommon_OpenCL_INCLUDE_DIRECTORIES
  ${elastix_SOURCE_DIR}/Common/OpenCL
  ${elastix_SOURCE_DIR}/Common/OpenCL/Factories
  ${elastix_SOURCE_DIR}/Common/OpenCL/ITKimprovements
  ${elastix_SOURCE_DIR}/Common/OpenCL/Filters
  ${elastix_SOURCE_DIR}/Common/OpenCL/Kernels
)

set(elxCore_INCLUDE_DIRECTORIES
  ${elastix_SOURCE_DIR}/Core
  ${elastix_SOURCE_DIR}/Core/Install
  ${elastix_SOURCE_DIR}/Core/Kernel
  ${elastix_SOURCE_DIR}/Core/ComponentBaseClasses
  ${elastix_SOURCE_DIR}/Core/Configuration
  ${elastix_SOURCE_DIR}/Core/Main
)

set(elxComponents_INCLUDE_DIRECTORIES
  ${elastix_SOURCE_DIR}/Components/FixedImagePyramids
  ${elastix_SOURCE_DIR}/Components/ImageSamplers
  ${elastix_SOURCE_DIR}/Components/Interpolators
  ${elastix_SOURCE_DIR}/Components/Metrics
  ${elastix_SOURCE_DIR}/Components/MovingImagePyramids
  ${elastix_SOURCE_DIR}/Components/Optimizers
  ${elastix_SOURCE_DIR}/Components/Registrations
  ${elastix_SOURCE_DIR}/Components/ResampleInterpolators
  ${elastix_SOURCE_DIR}/Components/Resamplers
  ${elastix_SOURCE_DIR}/Components/Transforms
)

set(elxINCLUDE_DIRECTORIES
  ${elxCommon_INCLUDE_DIRECTORIES}
  ${elxCore_INCLUDE_DIRECTORIES}
  ${elxComponents_INCLUDE_DIRECTORIES}
  ${elastix_BINARY_DIR}
  ${ELASTIX_USER_COMPONENT_DIRS}
)

include_directories(${elxINCLUDE_DIRECTORIES})

install(DIRECTORY "${CMAKE_BINARY_DIR}/bin/" DESTINATION "/usr/lib")

# install header files and set variable for UseElastix.cmake
foreach (dir ${elxCommon_INCLUDE_DIRECTORIES} ${elxCommon_OpenCL_INCLUDE_DIRECTORIES}
             ${elxCore_INCLUDE_DIRECTORIES} ${elxComponents_INCLUDE_DIRECTORIES})
  # Replace previously set directories with the install directory counterpart
  string(REPLACE "${elastix_SOURCE_DIR}/" "/usr/include/" install_dir ${dir})
  # Install the header files
  install(DIRECTORY ${dir} DESTINATION "/usr/include")
  # And append the install directories to elxINSTALL_DIRECTORIES for configuring UseElastix
  list(APPEND elxINSTALL_DIRECTORIES ${install_dir})
endforeach(dir)

# include the OpenCL directories
# The key-word BEFORE is important here, because the elastix distribution
# contains some files with the same name as in the ITK distribution.
# Some bugs were fixed and features added. When these are contributed back
# to the ITK the BEFORE keyword is not needed anymore.
if(ELASTIX_USE_OPENCL)
  include_directories( BEFORE ${elxCommon_OpenCL_INCLUDE_DIRECTORIES} )
endif()

#---------------------------------------------------------------------
# Microsoft specific items

if(MSVC)
  # Kill the anoying MS VS warning about non-safe functions.
  # They hide real warnings.
  add_definitions(
    /D_SCL_SECURE_NO_DEPRECATE
    /D_CRT_SECURE_NO_DEPRECATE
    /D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
  )

  # Increases address capacity
  if(WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
  endif()
endif()

#---------------------------------------------------------------------
# Process the sub-directories

# Common dir: code that is neither related to the core of elastix or
# to specific components.
add_subdirectory(Common)

# Components: the registration components such as metrics, transforms,
# optimizers, etc.
add_subdirectory(Components)

# Core dir: code that takes care of starting elastix, loading
# components, definitions of macros etc.
add_subdirectory(Core)

#---------------------------------------------------------------------
# Configure the examples

set(ELASTIX_DOX_DIR  ${elastix_SOURCE_DIR}/../dox)
set(ELASTIX_TOOLS_DIR  ${elastix_SOURCE_DIR}/../tools)
set(ELASTIX_HELP_DIR ${elastix_SOURCE_DIR}/../help CACHE PATH
  "path to the doxygen generated help files and the examples")

# Copy the examples to the help directory

if(WIN32)
  configure_file(
    ${ELASTIX_DOX_DIR}/example.bat
    ${ELASTIX_HELP_DIR}/example.bat
    COPYONLY)
else()
  configure_file(
    ${ELASTIX_DOX_DIR}/example
    ${ELASTIX_HELP_DIR}/example
    COPYONLY)
endif()

make_directory(${ELASTIX_HELP_DIR}/exampleinput)
make_directory(${ELASTIX_HELP_DIR}/exampleoutput)

set(ELX_EXAMPLEINPUTFILES
  fixed.mhd
  fixed.raw
  mask_fixed.mhd
  mask_fixed.raw
  mask_moving.mhd
  mask_moving.raw
  moving.mhd
  moving.raw
  parameters_Affine.txt
  parameters_BSpline.txt
  parameters_Rigid.txt
  parameters_Translation.txt
  solution_deformedmovingimage.mhd
  solution_deformedmovingimage.raw
)

foreach(ELX_EXAMPLEINPUTFILE ${ELX_EXAMPLEINPUTFILES})
  configure_file(
    ${ELASTIX_DOX_DIR}/exampleinput/${ELX_EXAMPLEINPUTFILE}
    ${ELASTIX_HELP_DIR}/exampleinput/${ELX_EXAMPLEINPUTFILE}
    COPYONLY)
endforeach()

#---------------------------------------------------------------------
# Get version information.

# Get the version number of elastix
file(STRINGS ${elastix_SOURCE_DIR}/Core/Install/elxBaseComponent.h
  ELASTIX_VERSION REGEX "\(#define\ __ELASTIX_VERSION\)")
string(SUBSTRING ${ELASTIX_VERSION} 26 3 ELASTIX_VERSION)

# Split version in major minor. Assuming no patch number, just x.y
string(REGEX MATCH "[0-9]+" ELASTIX_VERSION_MAJOR "${ELASTIX_VERSION}")
string(REGEX REPLACE "([0-9]+)\\." "" ELASTIX_VERSION_MINOR "${ELASTIX_VERSION}")
set(ELASTIX_VERSION_PATCH "12")
set(ELASTIX_FULL_VERSION
       "${ELASTIX_VERSION_MAJOR}.${ELASTIX_VERSION_MINOR}.${ELASTIX_VERSION_PATCH}")

#---------------------------------------------------------------------
# Configure the doxygen-configuration

find_package(Doxygen QUIET)
string(COMPARE NOTEQUAL ${DOXYGEN} "DOXYGEN-NOTFOUND" Doxygen_FOUND)

if(Doxygen_FOUND)

  # Set the path to the doxygen configuration source
  set(ELASTIX_DOXYGEN_DIR ${ELASTIX_DOX_DIR}/doxygen)

  # Get the version number of doxygen
  exec_program(${DOXYGEN} ARGS "--version" OUTPUT_VARIABLE ELASTIX_DOXYGEN_VERSION)

  # Get date
  if(UNIX OR CYGWIN)
    exec_program("date '+%d-%m-%Y'" OUTPUT_VARIABLE ELASTIX_DOXYGEN_DATE)
  endif()
  if(WIN32)
    if(NOT CYGWIN)
      execute_process(COMMAND "${ELASTIX_DOXYGEN_DIR}/doxdate.bat"
        OUTPUT_VARIABLE ELASTIX_DOXYGEN_DATETEMP)
      string(REPLACE "/" "-" ELASTIX_DOXYGEN_DATE ${ELASTIX_DOXYGEN_DATETEMP})
    endif()
  endif()

  # Configure the doxygen configuration
  configure_file(
    ${ELASTIX_DOXYGEN_DIR}/doxyfile.in
    ${ELASTIX_HELP_DIR}/doxyfile.out @ONLY)

  # Configure the footer of the help website.
  configure_file(
    ${ELASTIX_DOXYGEN_DIR}/DoxygenFooter.html.in
    ${ELASTIX_HELP_DIR}/DoxygenFooter.html @ONLY)

  # Configure the MainPage.dox
  configure_file(
    ${ELASTIX_DOXYGEN_DIR}/MainPage.dox.in
    ${ELASTIX_HELP_DIR}/MainPage.dox @ONLY)
endif()

#---------------------------------------------------------------------
# Testing

set(ELASTIX_BUILD_TESTING OFF CACHE BOOL
  "Perform some tests on basic functionality of elastix.")
if(ELASTIX_BUILD_TESTING)
  enable_testing()
  add_subdirectory(Testing)
  include(CTest)
endif()

#---------------------------------------------------------------------
# Packaging

# Version information
# If the next line is uncommented the package name will be like
# elastix-4.3-win64 instead of elastix-4.3.0-win64
set(CPACK_PACKAGE_VERSION_MAJOR ${ELASTIX_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${ELASTIX_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${ELASTIX_VERSION_PATCH})
# Set install directory.
set(CPACK_INSTALL_PREFIX "/usr")
# Debian package
# Also install the copyright file, since when the user enables packaging
# we assume that the package is meant to distribute.
install(FILES "${elastix_SOURCE_DIR}/LICENSE" DESTINATION .)
install(FILES "${elastix_SOURCE_DIR}/NOTICE" DESTINATION .)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_DESCRIPTION "Elastix: a toolbox for rigid and nonrigid registration of images")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A library based on ITK (Insight Segmentation and Registration Toolkit)")
set(CPACK_PACKAGE_VENDOR "Bossa Nova Robotics")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Akshay Prasad")
set(CPACK_PACKAGE_CONTACT "akshay.prasad@bossanova.com")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/NOTICE")

set(CPACK_SET_DESTDIR "OFF")

# set the generators
# Setting the install directory. By default it is installed in usr/local/
set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

#---------------------------------------------------------------------
# Make it easier to include elastix functionality in other programs.
set(INSTALL_INCLUDE_DIR "/usr/include")
set(LIB_CMAKE_PATH "/usr/lib/cmake/elastix")
set(INSTALL_CMAKE_DIR "/usr/share/cmake/elastix")

# Save library dependencies.
# The library dependencies file.
export_library_dependencies(${elastix_BINARY_DIR}/elxLibraryDepends.cmake)
install(FILES ${elastix_BINARY_DIR}/elxLibraryDepends.cmake
        DESTINATION ${LIB_CMAKE_PATH})
set(elxLIBRARY_DEPENDS_FILE "${LIB_CMAKE_PATH}/elxLibraryDepends.cmake")
set(LIBRARY_INSTALL_PATH "/usr/lib")
# The "Use" file.
set(elxUSE_FILE ${elastix_BINARY_DIR}/UseElastix.cmake)

# Create the use file.
configure_file(${elastix_SOURCE_DIR}/CMake/UseElastix.cmake.in
 ${elxUSE_FILE} @ONLY)

file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
# ... for the build tree
configure_file(ElastixConfig.cmake.in
  "${PROJECT_BINARY_DIR}/ElastixConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${Elastix_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(ElastixConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ElastixConfig.cmake" @ONLY)
# ... for both
configure_file(ElastixConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/ElastixConfigVersion.cmake" @ONLY)
# Install UseElastix.cmake, ElastixConfig.cmake and
#                           ElastixConfigVersion.cmake
install(FILES
  "${elxUSE_FILE}"
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ElastixConfig.cmake"
  "${PROJECT_BINARY_DIR}/ElastixConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

include(CPack)
