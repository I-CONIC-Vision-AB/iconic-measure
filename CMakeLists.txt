cmake_minimum_required(VERSION 3.24)
project(iconic-measure LANGUAGES CXX)

# Edit ICONIC_PATH to where I-CONIC API is installed:
set(ICONIC_PATH "C:/Dev/iconic-api-binary/x64-Release") 
if (NOT ICONIC_PATH)
   message(FATAL_ERROR " Please set ICONIC_PATH to path with I-CONIC API (e.g. C:/Dev/iconic-api-binary/x64-Release)!")
endif()

find_package(OpenCL REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread timer)
find_package(GLEW REQUIRED)
find_package(wxWidgets 3.1 REQUIRED COMPONENTS net core base gl xml)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavdevice
    libavformat
    libavcodec
    libavutil
    libavfilter
    libswresample
    libswscale
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set a few I-CONIC paths
set(iconic_INCLUDE_DIRS "${ICONIC_PATH}/include")
set(IconicGpu "${ICONIC_PATH}/lib/IconicGpu.lib")
set(IconicVideo "${ICONIC_PATH}/lib/IconicVideo.lib")
set(IconicGui "${ICONIC_PATH}/lib/IconicGui.lib")

add_subdirectory("src/IconicMeasureCommon") # Library
add_subdirectory("src/VideoDisplay")        # Sample application

