add_subdirectory(libwally-core)

set(JSONCPP_WITH_TESTS OFF CACHE INTERNAL "Do not build JsonCpp tests.")
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE INTERNAL "Do not run JsonCpp tests after build.")
add_subdirectory(jsoncpp)

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

add_library(ccan
    ./ccan/str/base32/base32.c
)
target_include_directories(ccan
    PUBLIC
    .
)
