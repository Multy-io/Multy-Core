set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MULTY_MORE_WARNINGS)
    add_definitions(
        -Wall -Weffc++ -Wpedantic -Wextra -Wunused -Wfloat-equal -Wpointer-arith -Wwrite-strings

        # Disabled warnings:
         -Wno-missing-braces # false-positive on std::array initialization.
    )
endif()

if (MULTY_WARNINGS_AS_ERRORS)
    add_definitions(-Werror)
endif()

add_library(multy_core
    # API headers
    api.h
    account.h
    big_int.h
    common.h
    error.h
    key.h
    mnemonic.h
    sha3.h
    properties.h
    transaction.h

    # implementation of API functions
    src/api/account.cpp
    src/api/account_impl.cpp
    src/api/big_int.cpp
    src/api/big_int_impl.cpp
    src/api/common.cpp
    src/api/error.cpp
    src/api/key.cpp
    src/api/key_impl.cpp
    src/api/mnemonic.cpp
    src/api/properties.cpp
    src/api/properties_impl.cpp
    src/api/sha3.cpp
    src/api/sha3_impl.cpp
    src/api/transaction.cpp
    src/api/transaction_impl.cpp

    # Common stuff
    src/account_base.cpp

    src/exception.cpp
    src/hd_path.cpp
    src/object.cpp
    src/transaction_base.cpp
    src/u_ptr.cpp
    src/utility.cpp

    # Bitcoin
    src/bitcoin/bitcoin_account.cpp
    src/bitcoin/bitcoin_transaction.cpp

    # Ethereum
    src/ethereum/ethereum_account.cpp
    src/ethereum/ethereum_transaction.cpp
)

target_compile_definitions(multy_core PRIVATE BUILDING_MULTY_CORE=1)
target_compile_definitions(multy_core PRIVATE
    MULTY_CORE_VERSION_MAJOR=0
    MULTY_CORE_VERSION_MINOR=1
    MULTY_CORE_VERSION_BUILD=0
    MULTY_CORE_VERSION_NOTE="PRE-ALPHA-TESTNET"
    MULTY_CORE_VERSION_COMMIT=""
)

target_include_directories(multy_core PRIVATE
    ..
    ../third-party/libwally-core/include/
    ../third-party/libwally-core/src/ccan
    ../third-party
)

target_link_libraries(multy_core
    PUBLIC
    libwally-core
    keccak-tiny
    mini-gmp
)

set_target_properties(multy_core PROPERTIES CXX_VISIBILITY_PRESET hidden)

