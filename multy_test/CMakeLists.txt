
if (MULTY_MORE_WARNINGS)
    # Set of warnings is smaller than for multy_core to make life easier.
    add_definitions(
        -Wall -Wunused -Wfloat-equal -Wwrite-strings

        # Disabled warnings:
        -Wno-missing-braces # false-positive on std::array initialization.
    )
endif()

# Explicitly building multy_test as shared library, otherwise test cases would be thrown away by
# linker as symbols with internal linkage with is never referenced.
# That means that test cases can't be discovered by gtest runtime and hence not run.
add_library(multy_test SHARED
    run_tests.cpp

    mocks.cpp
    utility.cpp
    value_printers.cpp

    serialized_keys_test_base.cpp
    smoke_test.cpp
    test_account.cpp
    test_big_int.cpp
    test_bitcoin_account.cpp
    test_bitcoin_transaction.cpp
    test_common.cpp
    test_deletion.cpp
    test_ethereum_account.cpp
    test_ethereum_transaction.cpp
    test_keys.cpp
    test_mnemonic.cpp
    test_object.cpp
    test_properties.cpp
    test_sha3.cpp
    test_transaction.cpp
    test_utility.cpp
)

target_include_directories(
    multy_test
    PRIVATE
    ..
    ../third-party/googletest/googletest/include
    ../third-party/googletest/googlemock/include
    ../third-party/libwally-core/include
)

set_target_properties(
    multy_test
    PROPERTIES
    CXX_STANDARD 11
    LANGUAGE CXX
    LINKER_LANGUAGE CXX
)

target_link_libraries(
    multy_test
    PRIVATE
    gtest
    multy_core
    libwally-core
)
