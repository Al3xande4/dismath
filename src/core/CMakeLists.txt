add_library(core_api INTERFACE)
target_include_directories(core_api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(core STATIC 
    engine/algebra/word_adder.cpp
    engine/rules/tables/table_generator.cpp
    engine/rules/transition/transition_manager.cpp
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(core PUBLIC core_api)
