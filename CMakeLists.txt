cmake_minimum_required(VERSION 3.24)
project(symbol-calculator LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)

option(SANITIZE "Enable AddressSanitizer and UndefinedBehaviorSanitizer (Debug)" OFF)
option(SANITIZE_THREAD "Enable ThreadSanitizer (Debug, mutually exclusive with SANITIZE)" OFF)
if(SANITIZE_THREAD)
    add_compile_options(-fsanitize=thread -fno-omit-frame-pointer -no-pie)
    add_link_options(-fsanitize=thread -no-pie)
elseif(SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
