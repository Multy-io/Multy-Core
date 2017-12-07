cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(WITH_TESTS "Build tests shared library (DLL)" NO)
set(PATH_TO_JNI_WRAPPER "" CACHE PATH "Path to jni wrapper source file.")

project(multy)

add_executable(
    multy
    main.cpp
)
target_link_libraries(multy PRIVATE multy_core multy_transaction)
target_include_directories(multy PRIVATE .)

add_subdirectory(multy_core)
add_subdirectory(multy_transaction)
add_subdirectory(third-party)

if (ANDROID)
    add_library(core_jni
       SHARED
       "${PATH_TO_JNI_WRAPPER}"
    )

    find_library(log-lib
                  log )

    target_link_libraries(core_jni PRIVATE multy_core multy_test multy_transaction android ${log-lib})
    target_include_directories(core_jni PRIVATE .)
    set_target_properties(core_jni PROPERTIES
        CXX_STANDARD 11
        LANGUAGE CXX
    )
endif()

if(WITH_TESTS)
    target_compile_definitions(multy PRIVATE WITH_TESTS)
    add_subdirectory(multy_test)
    target_link_libraries(multy PRIVATE multy_test)
endif()
