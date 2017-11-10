cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_TESTS "Build tests shared library (DLL)" NO)
set(LIBWALLY_BIN "" CACHE FILEPATH "Path to prebuild libwally-core")
set(LIBWALLY_BIN_PATH "" CACHE PATH "Path to directory with prebuild libwally-core")
set(LIBSECP256K1_BIN "" CACHE FILEPATH "Path to prebuild libsecp256k1")
set(LIBSECP256K1_BIN_PATH "" CACHE PATH "Path to directory with prebuild libsecp256k1")

project(multy)

add_executable(
    multy
    main.cpp
)
target_link_libraries(multy PRIVATE multy_core)
target_include_directories(multy PRIVATE .)

add_subdirectory(multy_core)
add_subdirectory(third-party)

if(WITH_TESTS)
    target_compile_definitions(multy PRIVATE WITH_TESTS)
    add_subdirectory(multy_test)
    target_link_libraries(multy PRIVATE multy_test)
endif()

