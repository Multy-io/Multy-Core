cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# General options
option(MULTY_WITH_TESTS "Build multy_test library" NO)
option(MULTY_MORE_WARNINGS "More warnings" OFF)
option(MULTY_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(MULTY_ENABLE_SIMULATE_ERROR "Enable simulating errors in multy_core, see THROW_IF_WALLY_ERROR for details." OFF)
# This is a standard CMake option, but we are putting it here to emphasise.
option(BUILD_SHARED_LIBS "Build as shared libraries." OFF)
option(MULTY_WITH_TEST_APP "Build sample app that runs the tests (consider MULTY_WITH_TESTS)." OFF)

# Platform-specific options
if (ANDROID)
    # Android specific options, must be prefixed with MULTY_ANDROID_
    set(MULTY_ANDROID_PATH_TO_JNI_WRAPPER "" CACHE PATH "Path to jni wrapper source file.")
elseif(DEFINED IOS_PLATFORM)
    # iOS specific options, must be prefixed with MULTY_IOS_
else()
    # Desktop specific options, must be prefixed with MULTY_DESKTOP_
endif()

project(multy C CXX)

if(MULTY_WITH_TEST_APP)
    set(MULTY_WITH_TESTS ON)
    add_executable(
        multy
        main.cpp
    )
    target_link_libraries(multy PRIVATE multy_core)
    target_include_directories(multy PRIVATE .)
endif()

add_subdirectory(multy_core)
add_subdirectory(third-party)

if (ANDROID)
    add_library(multy_core_jni
       SHARED
       "${MULTY_ANDROID_PATH_TO_JNI_WRAPPER}"
    )

    find_library(android_log_lib log )

    target_link_libraries(multy_core_jni
        PRIVATE
        multy_core
        multy_test
        android
        ${android_log_lib}
    )

    target_include_directories(multy_core_jni PRIVATE .)

    set_target_properties(multy_core_jni
        PROPERTIES
        CXX_STANDARD 11
        LANGUAGE CXX
        LINKER_LANGUAGE CXX
    )
endif()

if(MULTY_WITH_TESTS)
    add_subdirectory(multy_test)

    if(TARGET multy)
        target_compile_definitions(multy PRIVATE MULTY_WITH_TESTS)
        target_link_libraries(multy PRIVATE multy_test)
    endif()
endif()
