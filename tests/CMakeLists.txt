include(FetchContent)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(unit_tests
    alg/entities/test_entities.cpp
    alg/tables/test_generator.cpp
    alg/transition/test_transition.cpp
    alg/algebra/test_word_adder.cpp
    alg/algebra/test_substract.cpp
    alg/algebra/test_multiply.cpp
    alg/algebra/test_divide.cpp
    alg/algebra/test_divmod.cpp
    alg/test_algebra_api.cpp
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${PROJECT_SOURCE_DIR}/src/alg"
)

target_link_libraries(unit_tests PRIVATE
    alg::alg
    GTest::gtest_main
)

gtest_discover_tests(unit_tests)
