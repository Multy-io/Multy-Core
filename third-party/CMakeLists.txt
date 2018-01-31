add_subdirectory(libwally-core)

add_library(keccak-tiny STATIC
    ../third-party/keccak-tiny/keccak-tiny.c
)
set_target_properties(keccak-tiny PROPERTIES
    C_STANDARD 11
    POSITION_INDEPENDENT_CODE ON
)

add_library(mini-gmp STATIC
    ./mini-gmp/mini-gmp.c
)

if(DEFINED MULTY_WITH_TESTS)
    add_subdirectory(googletest)
    set_target_properties(gtest PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
    set_target_properties(gmock PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endif(DEFINED MULTY_WITH_TESTS)
