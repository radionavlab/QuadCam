cmake_minimum_required(VERSION 2.8.3)
project(quad_cam)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wno-format-security -Wno-literal-suffix")

include_directories(
    include/utils
    include/client
    include/server
    include/camera
)

find_library(CAMERA_LIBS NAMES camera PATHS /usr/lib)
if (NOT CAMERA_LIBS)
    message(WARNING "Could not find camera libraries, not building targets that depend on it")
endif()



#Libraries
add_library(quad_cam_lib
    include/camera/snapcam.h
    src/camera/snapcam.cc
)
target_link_libraries(quad_cam_lib ${CAMERA_LIBS})

add_library(quad_cam_server_lib
    include/server/camera_server.h
    src/server/camera_server.cc
)
target_link_libraries(quad_cam_server_lib quad_cam_lib)

add_library(quad_cam_client_lib
    include/client/camera_client.h
    src/client/camera_client.cc
)



# Executables
add_executable(quad_cam_server_exec src/exe/server.cc)
target_link_libraries(quad_cam_server_exec quad_cam_lib quad_cam_server_lib)

add_executable(quad_cam_client_exec src/exe/client.cc)
target_link_libraries(quad_cam_client_exec quad_cam_client_lib)

