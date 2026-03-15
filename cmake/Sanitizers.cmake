# cmake/Sanitizers.cmake
# Enables ASan + UBSan when ENABLE_SANITIZERS=ON and build type is Debug.

if(ENABLE_SANITIZERS)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(MSVC)
            message(WARNING "Sanitizers not supported on MSVC — skipping.")
        else()
            message(STATUS "Sanitizers: ASan + UBSan enabled")
            add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
            add_link_options   (-fsanitize=address,undefined)
        endif()
    else()
        message(STATUS "Sanitizers requested but build type is not Debug — skipping.")
    endif()
endif()
