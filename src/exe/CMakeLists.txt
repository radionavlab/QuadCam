# Server
add_executable(server server.cc)
target_link_libraries(server LINK_PUBLIC utils camera_server)
