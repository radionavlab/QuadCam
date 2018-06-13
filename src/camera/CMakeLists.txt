add_library(camera_wrapper
    quadcam.cc
)

target_include_directories(
    camera_wrapper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Camera Library
find_library(CAMERA_LIBS NAMES camera PATHS /usr/lib)
if(NOT CAMERA_LIBS)
    message(WARNING "Can't find camera libraries!")
endif()

target_link_libraries(camera_wrapper LINK_PUBLIC utils ${CAMERA_LIBS})
