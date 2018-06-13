add_library(camera_client
    camera_client.cc
)

target_include_directories(
    camera_client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(camera_client LINK_PUBLIC utils)
