cmake_minimum_required(VERSION 2.8.3)
project(snap_cam)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wno-format-security -Wno-literal-suffix")

include_directories(
    include/utils
)

find_library(CAMERA_LIBS NAMES camera PATHS /usr/lib)
if (NOT CAMERA_LIBS)
    message(WARNING "Could not find camera libraries, not building targets that depend on it")
endif()

## Camera library
add_library(snap_cam_lib
    include/camera/snapcam.h
    src/camera/snapcam.cc
)
target_link_libraries(snap_cam_lib ${CAMERA_LIBS})

# Server Library
add_library(snap_cam_server_lib
    include/server/camera_server.h
    src/server/camera_server.cc)

# Client Library
add_library(snap_cam_client_lib
    include/client/camera_client.h
    src/client/camera_client.cc)

# Executables
add_executable(snap_cam_server_exec src/server/server.cc)
target_link_libraries(snap_cam_server_exec snap_cam_lib snap_cam_server_lib)

add_executable(snap_cam_client_exec src/client/client.cc)
target_link_libraries(snap_cam_client_exec snap_cam_client_lib)

