add_library(alg STATIC
    entities/word.cpp
    transition/graph_validator.cpp
    transition/transition_manager.cpp
    tables/table_generator.cpp
    algebra/word_adder.cpp
    algebra/algebra.cpp
)
add_library(alg::alg ALIAS alg)

target_include_directories(alg
    PUBLIC  "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    if(ipo_supported)
        set_property(TARGET alg PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()