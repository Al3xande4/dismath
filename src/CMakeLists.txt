add_subdirectory(core)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    if(ipo_supported)
        set_property(TARGET core PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()
