set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MULTY_MORE_WARNINGS)
    add_definitions(
        -Wall -Weffc++ -Wpedantic -Wextra -Wunused -Wfloat-equal -Wpointer-arith -Wwrite-strings

        # Disabled warnings:
         -Wno-missing-braces # false-positive on std::array initialization.
         -Wno-tautological-constant-out-of-range-compare # false-positive on caution checks
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
    src/hash.cpp

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
    ../third-party/libwally-core/src/ccan
    ../third-party
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_libraries(multy_core
    PUBLIC
    libwally-core
    keccak-tiny
    mini-gmp
)
add_dependencies(multy_core multy_core_generate_version)

set_target_properties(multy_core PROPERTIES CXX_VISIBILITY_PRESET hidden)
