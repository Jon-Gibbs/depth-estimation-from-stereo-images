cmake_minimum_required(VERSION 3.50)
project(DisparityMapper)

set(CMAKE_CXX_STANDARD 17)

# find dependencies
find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

# include headers
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${YAML_CPP_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}   # so main.cpp can find rectifier.h
)

# define the executable and its source files
add_executable(disparity_mapper
    main.cpp
    disparity_mapper.cpp
)

# link libraries
target_link_libraries(disparity_mapper
    ${OpenCV_LIBS}
    yaml-cpp
)