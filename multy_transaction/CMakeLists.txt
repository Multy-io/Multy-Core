set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(multy_transaction
    SHARED
    amount.h
    properties.h
    transaction.h

    src/amount.cpp
    src/properties.cpp
    src/transaction.cpp

    internal/amount.cpp
    internal/bitcoin_transaction.cpp
    internal/properties.cpp
    internal/transaction_base.cpp
    internal/transaction.cpp
)

target_compile_definitions(multy_transaction PRIVATE BUILDING_MULTY_TRANSACTION=1)

target_include_directories(multy_transaction PRIVATE
    .
    ..
    ../third-party/libwally-core/include/
    ../third-party/libwally-core/src/ccan
    ../third-party
)

target_link_libraries(
    multy_transaction
    PRIVATE
    multy_core
    mini-gmp
)

set_target_properties(multy_transaction PROPERTIES CXX_VISIBILITY_PRESET hidden)
