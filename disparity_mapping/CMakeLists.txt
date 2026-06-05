

cmake_minimum_required(VERSION 3.10)
project(disparity_mapper)

# Tell CMake where to find OpenCV (your MSYS2 UCRT64 install)
set(OpenCV_DIR "C:/msys64/ucrt64/lib/cmake/opencv4")

find_package(OpenCV REQUIRED)

add_executable(disparity_mapper main.cpp disparity_mapper.cpp template_matcher.cpp)

# Include headers + link libraries
target_include_directories(disparity_mapper PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(disparity_mapper PRIVATE ${OpenCV_LIBS})

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()