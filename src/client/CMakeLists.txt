add_library(camera_client STATIC
    camera_client.cc
)

target_include_directories(
    camera_client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(camera_client LINK_PUBLIC utils)

install(TARGETS camera_client DESTINATION /usr/local/lib/quadcam)
install(FILES camera_client.h frame_data.h DESTINATION /usr/local/include/quadcam)
