# cmake/CompilerWarnings.cmake
# Applies a strong set of compiler warnings to a target.
# Usage: target_enable_warnings(<target>)

function(target_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /w14242 /w14254 /w14263 /w14265 /w14287
            /w18019 /w14296 /w14311 /w14545 /w14546
            /w14547 /w14549 /w14555 /w14619 /w14640
            /w14826 /w14905 /w14906 /w14928
            /permissive-
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wconversion
            -Wsign-conversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
        )
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            target_compile_options(${target} PRIVATE
                -Wmisleading-indentation
                -Wduplicated-cond
                -Wduplicated-branches
                -Wlogical-op
                -Wuseless-cast
            )
        endif()
    endif()
endfunction()