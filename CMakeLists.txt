cmake_minimum_required(VERSION 3.24)
project(SymbolicCalculator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_GUI "Build Qt GUI application" ON)

add_compile_options(-Wall -Wextra -Wpedantic)

option(SANITIZE "Enable AddressSanitizer and UndefinedBehaviorSanitizer (Debug)" OFF)
if(SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()


add_subdirectory(src/alg)

if(BUILD_GUI)
    # add_subdirectory(apps/gui)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()