
add_library(multy_test SHARED
    run_tests.cpp
    serialized_keys_test_base.cpp
    smoke_test.cpp
    test_account.cpp
    test_bitcoin_account.cpp
    test_bitcoin_transaction.cpp
    test_ethereum_account.cpp
    test_keys.cpp
    test_mnemonic.cpp
    test_utility.cpp
    utility.cpp
    value_printers.cpp
)

target_include_directories(
    multy_test
    PRIVATE
    ..
    ../third-party/googletest/googletest/include
    ../third-party/googletest/googlemock/include
    ../third-party/libwally-core/include
)

target_link_libraries(
    multy_test
    PRIVATE
    gtest
    multy_core
    multy_transaction
)
