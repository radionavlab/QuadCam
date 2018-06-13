add_library(camera_server
    camera_server.cc
)

target_include_directories(
    camera_server PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(camera_server LINK_PUBLIC utils camera_wrapper)
