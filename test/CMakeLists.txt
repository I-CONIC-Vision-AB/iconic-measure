enable_testing()

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

set(SOURCES
    "${PROJECT_SOURCE_DIR}/test/iconic-measure-test.cpp"
)

add_executable(IconicMeasureTest ${SOURCES})

target_include_directories(IconicMeasureTest PUBLIC "${PROJECT_SOURCE_DIR}/test")
target_compile_definitions(IconicMeasureTest PUBLIC BOOST_TEST_DYN_LINK)

target_include_directories(IconicMeasureTest PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(IconicMeasureTest PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_include_directories(IconicMeasureTest PUBLIC "${PROJECT_SOURCE_DIR}/include"
${iconic_INCLUDE_DIRS})

target_link_libraries(IconicMeasureTest 
    ${OpenCL_LIBRARIES}
    ${wxWidgets_LIBRARIES}
    GLEW::GLEW
    ${IconicGpu}
    IconicMeasureCommon
    Boost::thread
    Boost::unit_test_framework
)

# Name : TestName, Name that will be displayed during test; Internal identifier (not a filename) 
# Command : The executable to run
add_test(NAME IconicMeasureTest COMMAND IconicMeasureTest)