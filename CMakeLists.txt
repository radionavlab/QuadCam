cmake_minimum_required(VERSION 2.8.3)
project(quad_cam)

# Build type and flags
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wno-format-security -Wno-literal-suffix")

add_subdirectory(src)
