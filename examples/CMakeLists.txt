cmake_minimum_required(VERSION 2.8.3)
project(quad_cam_client)

# Build type and flags
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

# Library and headers
set(CAMERA_CLIENT_INCLUDE_DIR "/usr/local/include/quadcam")
set(CAMERA_CLIENT_LIBS "/usr/local/lib/quadcam/libcamera_client.a")

# Include headers
include_directories(${CAMERA_CLIENT_INCLUDE_DIR})

# Executable
add_executable(${PROJECT_NAME} client.cc)

# Link library
target_link_libraries(${PROJECT_NAME} ${CAMERA_CLIENT_LIBS})
