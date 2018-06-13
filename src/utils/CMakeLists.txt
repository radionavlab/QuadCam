add_library(utils STATIC
    utils.cc
)

target_include_directories(
    utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

