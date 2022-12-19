

set(SOURCES
    "${PROJECT_SOURCE_DIR}/src/VideoDisplay/VideoPlayerApp.cpp"
)

if (WIN32)
		add_executable(IconicMeasure WIN32 ${SOURCES})
else ()
		add_executable(IconicMeasure ${SOURCES})
endif ()

target_compile_definitions(IconicMeasure PUBLIC CL_TARGET_OPENCL_VERSION=220)

target_include_directories(IconicMeasure
    PUBLIC
        ${PROJECT_SOURCE_DIR} 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
    PRIVATE 
        ${iconic_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS}
)

target_link_libraries(IconicMeasure 
    PRIVATE
        GLEW::GLEW
        OpenCL::OpenCL
        ${wxWidgets_LIBRARIES}
        ${IconicVideo}
        ${IconicSampleCommon}
        IconicMeasureCommon
)

# Copy the I-CONIC binaries to the output binary folder
add_custom_command(TARGET IconicMeasure POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ICONIC_PATH}/bin"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)