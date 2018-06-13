
# Server
add_executable(server server.cc)
target_link_libraries(server LINK_PUBLIC utils camera_server)

# Client
add_executable(client client.cc)
target_link_libraries(client LINK_PUBLIC utils camera_client)
