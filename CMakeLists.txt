cmake_minimum_required(VERSION 3.20)
project(
    BROKERINO
    VERSION 0.1.0
    DESCRIPTION "Lightweight health-oriented MQTT broker using Boost.Asio"
    LANGUAGES CXX
)

#  Guard: no in-source builds 
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed.\n"
        "Run:  cmake -S . -B build"
    )
endif()

# Global C++ settings 
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS     ON)

# Default build type 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type set -- defaulting to RelWithDebInfo")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose: Debug | Release | RelWithDebInfo | MinSizeRel" FORCE)
endif()

#  User-facing options 
option(BUILD_TESTS         "Build unit & integration tests"        ON)
option(USE_STANDALONE_ASIO "Use standalone Asio instead of Boost" ON)
option(ENABLE_WEBHOOKS     "HTTP webhook support via cpr"          ON)
option(ENABLE_SANITIZERS   "Enable ASan + UBSan in Debug builds"  OFF)

# Optional CMake modules 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)

# LTO (Release only, guarded) 
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _ipo_ok OUTPUT _ipo_why)
    if(_ipo_ok)
        message(STATUS "LTO enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    else()
        message(STATUS "LTO not supported: ${_ipo_why}")
    endif()
endif()

# System dependencies 
find_package(Threads REQUIRED)

#  FetchContent 
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET               OFF)

# nlohmann/json 
FetchContent_Declare(json
    URL      https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
)

# spdlog 
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.3
    GIT_SHALLOW    ON
)

# SQLite amalgamation
FetchContent_Declare(sqlite_src
    URL      https://www.sqlite.org/2026/sqlite-amalgamation-3510200.zip
    URL_HASH SHA3_256=9a9dd4eef7a97809bfacd84a7db5080a5c0eff7aaf1fc1aca20a6dc9a0c26f96
)

# cpr 
FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.11.2
    GIT_SHALLOW    ON
)

if(USE_STANDALONE_ASIO)
    FetchContent_Declare(asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG        asio-1-36-0
        GIT_SHALLOW    ON
    )
endif()

if(BUILD_TESTS)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
        GIT_SHALLOW    ON
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

# Suppress subproject build noise 
set(SPDLOG_BUILD_EXAMPLE  OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH    OFF CACHE BOOL "" FORCE)
set(JSON_BuildTests       OFF CACHE BOOL "" FORCE)
set(JSON_Install          OFF CACHE BOOL "" FORCE)
set(CPR_BUILD_TESTS       OFF CACHE BOOL "" FORCE)

# Populate 
set(_fc_deps json spdlog sqlite_src)
if(ENABLE_WEBHOOKS)
    list(APPEND _fc_deps cpr)
endif()
if(USE_STANDALONE_ASIO)
    list(APPEND _fc_deps asio)
endif()
if(BUILD_TESTS)
    list(APPEND _fc_deps googletest)
endif()

FetchContent_MakeAvailable(${_fc_deps})

#  SQLite3 canonical target 
# Prefer system library (e.g. libsqlite3-dev on Pi).
# Both paths expose the same target name: SQLite::SQLite3
find_package(SQLite3 QUIET)
if(SQLite3_FOUND)
    message(STATUS "Using system SQLite3 ${SQLite3_VERSION}")
else()
    message(STATUS "System SQLite3 not found -- compiling amalgamation")
    add_library(sqlite3_amalgamation STATIC
        ${sqlite_src_SOURCE_DIR}/sqlite3.c
    )
    target_include_directories(sqlite3_amalgamation PUBLIC
        ${sqlite_src_SOURCE_DIR}
    )
    target_compile_definitions(sqlite3_amalgamation PUBLIC
        SQLITE_THREADSAFE=1
        SQLITE_ENABLE_FTS5
        SQLITE_ENABLE_JSON1
        SQLITE_DQS=0
        SQLITE_DEFAULT_MEMSTATUS=0
    )
    add_library(SQLite::SQLite3 ALIAS sqlite3_amalgamation)
endif()

# Standalone Asio interface target 
if(USE_STANDALONE_ASIO)
    add_library(asio_iface INTERFACE)
    target_include_directories(asio_iface INTERFACE
        "${asio_SOURCE_DIR}/asio/include"
    )
    target_link_libraries(asio_iface INTERFACE Threads::Threads)
    target_compile_definitions(asio_iface INTERFACE
        ASIO_STANDALONE
        ASIO_NO_DEPRECATED
        ASIO_HAS_MOVE
    )
    add_library(asio::asio ALIAS asio_iface)
endif()

#  Subdirectories 
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()