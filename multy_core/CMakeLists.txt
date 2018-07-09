set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MULTY_MORE_WARNINGS)
    add_definitions(
        -Wall -Wpedantic -Wextra -Wunused -Wfloat-equal -Wpointer-arith -Wwrite-strings

        # Disabled warnings:
         -Wno-missing-braces # false-positive on std::array initialization.
    )

    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
        add_definitions(
            -Wno-tautological-constant-out-of-range-compare # false-positive on caution checks
        )
    endif()
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES ".*Debug.*" OR MULTY_FORCE_ENABLE_ERROR_BACKTRACE)
    add_definitions(-DMULTY_ENABLE_ERROR_BACKTRACE=1)
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES ".*Release.*")
    add_definitions(-DMULTY_INVARIANT_THROWS_EXCEPTION=1)
endif()

if (MULTY_WARNINGS_AS_ERRORS)
    add_definitions(-Werror)
endif()

add_library(multy_core
    # API headers
    account.h
    api.h
    big_int.h
    blockchain.h
    common.h
    error.h
    key.h
    mnemonic.h
    properties.h
    sha3.h
    transaction.h
    transaction_builder.h

    # implementation of API functions
    src/api/account.cpp
    src/api/account_impl.cpp
    src/api/big_int.cpp
    src/api/big_int_impl.cpp
    src/api/binary_data.cpp
    src/api/blockchain.cpp
    src/api/common.cpp
    src/api/error.cpp
    src/api/key.cpp
    src/api/key_impl.cpp
    src/api/mnemonic.cpp
    src/api/properties.cpp
    src/api/properties_impl.h
    src/api/properties_impl.cpp
    src/api/sha3.cpp
    src/api/sha3_impl.cpp
    src/api/transaction.cpp
    src/api/transaction_impl.cpp
    src/api/transaction_builder.cpp
    src/api/transaction_builder_impl.cpp

    # Common stuff
    src/codec.cpp
    src/account_base.cpp
    src/ec_key_utils.cpp
    src/hash.cpp
    src/blockchain_facade_base.cpp
    src/backtrace.cpp

    src/binary_data_utility.cpp
    src/enum_name_map.cpp
    src/error_utility.cpp
    src/exception.cpp
    src/hd_path.cpp
    src/object.cpp
    src/transaction_base.cpp
    src/u_ptr.cpp
    src/utility.cpp

    # Bitcoin
    bitcoin.h
    src/bitcoin/bitcoin_facade.cpp
    src/bitcoin/bitcoin_account.cpp
    src/bitcoin/bitcoin_key.cpp
    src/bitcoin/bitcoin_transaction.cpp

    # Ethereum
    ethereum.h
    src/ethereum/ethereum_facade.cpp
    src/ethereum/ethereum_account.cpp
    src/ethereum/ethereum_address.cpp
    src/ethereum/ethereum_transaction.cpp
    src/ethereum/ethereum_token.cpp
    src/ethereum/ethereum_transaction_builder_multisig.cpp

    # Golos
    golos.h
    src/golos/golos.cpp
    src/golos/golos_facade.cpp
    src/golos/golos_account.cpp
    src/golos/golos_transaction.cpp

    # EOS
    EOS.h
    src/EOS/EOS_facade.cpp
    src/EOS/EOS_account.cpp
)

# Run a script that generates version.h in build dir
add_custom_target(
    multy_core_generate_version
    COMMAND ${CMAKE_COMMAND}
            -DIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.template
            -DOUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/include/generated/version.h
            -DGIT_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/version_from_git.cmake
)

target_compile_definitions(multy_core PRIVATE BUILDING_MULTY_CORE=1)

target_include_directories(multy_core PRIVATE
    ..
    ../third-party/libwally-core/include/
    ../third-party
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_libraries(multy_core
    PUBLIC
    libwally-core
    keccak-tiny
    mini-gmp
    ccan
)
add_dependencies(multy_core multy_core_generate_version)

set_target_properties(multy_core PROPERTIES CXX_VISIBILITY_PRESET hidden)
