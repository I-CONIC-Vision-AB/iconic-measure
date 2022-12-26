cmake_minimum_required(VERSION 3.24)
project(iconic-measure LANGUAGES CXX)

# Edit ICONIC_PATH to where I-CONIC API is installed.
# Option 1. Add -DICONIC_PATH:STRING="C:/Dev/iconic-api-binary/x64-Release" to CMake command arguments, or
# Option 2. Uncomment and edit this string (not recommended since this file is version controlled with GIT)
# set(ICONIC_PATH "C:/Dev/iconic-api-binary/x64-Release") 
if (NOT ICONIC_PATH)
   message(FATAL_ERROR " Please set ICONIC_PATH to path with I-CONIC API (e.g. C:/Dev/iconic-api-binary/x64-Release)!")
endif()

find_package(OpenCL REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread timer)
find_path(BOOST_GEOMETRY_INCLUDE_DIRS "boost/geometry.hpp") # Make sure we have boost::geometry for geometry processing
find_package(GLEW REQUIRED)
find_package(wxWidgets 3.1 REQUIRED COMPONENTS core base gl)
find_package(Eigen3 REQUIRED)
add_compile_definitions(EIGEN_DEFAULT_TO_ROW_MAJOR) # Make Eigen use row major matrices

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set a few I-CONIC paths
set(iconic_INCLUDE_DIRS "${ICONIC_PATH}/include")
set(IconicGpu "${ICONIC_PATH}/lib/IconicGpu.lib")
set(IconicVideo "${ICONIC_PATH}/lib/IconicVideo.lib")
set(IconicGui "${ICONIC_PATH}/lib/IconicGui.lib")
set(IconicSensor "${ICONIC_PATH}/lib/IconicSensor.lib")

add_subdirectory("src/IconicMeasureCommon") # Library
add_subdirectory("src/VideoDisplay")        # Sample application

