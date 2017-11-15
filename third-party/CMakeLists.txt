add_subdirectory(libwally-core)

add_library(keccak-tiny STATIC
    ../third-party/keccak-tiny/keccak-tiny.c
)

set_target_properties(keccak-tiny PROPERTIES
    C_STANDARD 11
    POSITION_INDEPENDENT_CODE ON
)

if(DEFINED WITH_TESTS)
    set(BUILD_SHARED_LIBS YES)
    add_subdirectory(googletest)
    set_target_properties(gtest PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
    set_target_properties(gmock PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endif(DEFINED WITH_TESTS)
