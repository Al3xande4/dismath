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
    core/entities/test_entities.cpp
    core/rules/test_generator.cpp
    core/rules/test_transition.cpp
    core/algebra/test_word_adder.cpp
    core/algebra/test_substract.cpp
    core/algebra/test_multiply.cpp
    core/algebra/test_divide.cpp
    core/algebra/test_divmod.cpp
)

target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(unit_tests PRIVATE
    core
    GTest::gtest_main
)

gtest_discover_tests(unit_tests)
