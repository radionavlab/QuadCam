# Server

FIND_PACKAGE( Boost COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(server server.cc)
target_link_libraries(server LINK_PUBLIC utils camera_server ${Boost_LIBRARIES})
