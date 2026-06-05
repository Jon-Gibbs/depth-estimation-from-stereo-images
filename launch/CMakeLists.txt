cmake_minimum_required(VERSION 3.5)
project(StereoDepthPipeline)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ../image_rectification
    ../disparity_mapping
)

set(SOURCES
    main.cpp
    ../image_rectification/rectifier.cpp
    ../disparity_mapping/disparity_mapper.cpp
    ../disparity_mapping/template_matcher.cpp
)

add_executable(stereo_pipeline ${SOURCES})

target_link_libraries(stereo_pipeline
    ${OpenCV_LIBS}
    yaml-cpp
)
