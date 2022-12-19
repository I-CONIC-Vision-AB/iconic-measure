# Which files to compile
set (SRC_DIR "${PROJECT_SOURCE_DIR}/src/IconicMeasureCommon")
set(SOURCES
    "${SRC_DIR}/OpenCLGrid.cpp"
    "${SRC_DIR}/VideoPlayerFrame.cpp"
    "${SRC_DIR}/IconicMeasureHandler.cpp"
    "ImageCanvas.cpp"
)

# The dynamic library
add_library(IconicMeasureCommon SHARED ${SOURCES}  "MeasureEvent.cpp")

target_compile_definitions(IconicMeasureCommon PUBLIC CL_TARGET_OPENCL_VERSION=220)

include(GenerateExportHeader)
set(ICONIC_EXPORTS_DIR "${CMAKE_BINARY_DIR}/exports")
generate_export_header(IconicMeasureCommon
    BASE_NAME iconic_measure_common
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/include/IconicMeasureCommon/exports.h"
)

target_include_directories(IconicMeasureCommon
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
    PRIVATE 
        ${iconic_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS}
)

target_link_libraries(IconicMeasureCommon 
    PRIVATE
        ${OpenCL_LIBRARIES}
        ${wxWidgets_LIBRARIES}
        Boost::thread
        Boost::timer
        GLEW::GLEW
        ${IconicGpu}        
        ${IconicVideo}
#        ${IconicGui}
)

