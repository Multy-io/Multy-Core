set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(multy_core
    SHARED

    src/account.cpp
    src/common.cpp
    src/error.cpp
    src/keys.cpp
    src/mnemonic.cpp

    internal/account_base.cpp
    internal/account.cpp
    internal/bitcoin_account.cpp
    internal/ethereum_account.cpp
    internal/hd_path.cpp
    internal/key.cpp
    internal/u_ptr.cpp
    internal/utility.cpp
)

target_compile_definitions(multy_core PRIVATE BUILDING_MULTY_CORE=1)
target_include_directories(multy_core PRIVATE
    ..
    ../third-party/libwally-core/include/
    ../third-party/libwally-core/src/ccan
    ../third-party
)

target_link_libraries(multy_core libwally-core keccak-tiny)
set_target_properties(multy_core PROPERTIES CXX_VISIBILITY_PRESET hidden)

