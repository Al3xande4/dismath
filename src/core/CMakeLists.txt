add_library(core_api INTERFACE)
target_include_directories(core_api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(core INTERFACE)
target_link_libraries(core INTERFACE core_api)
