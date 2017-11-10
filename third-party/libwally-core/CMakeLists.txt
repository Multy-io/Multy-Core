cmake_minimum_required (VERSION 2.8)

set(SOURCES
    src/aes.c
    src/base58.c
    src/bip32.c
    src/bip38.c
    src/bip39.c
    src/elements.c
    src/hex.c
    src/hmac.c
    src/internal.c
    src/mnemonic.c
    src/pbkdf2.c
    src/scrypt.c
    src/sign.c
    src/wordlist.c
    src/ccan/ccan/crypto/ripemd160/ripemd160.c
    src/ccan/ccan/crypto/sha256/sha256.c
    src/ccan/ccan/crypto/sha512/sha512.c
    src/ccan/ccan/str/hex/hex.c
 )
add_library(libwally-core SHARED ${SOURCES})
target_include_directories(libwally-core
    PUBLIC
        .
        src
        src/ccan
        src/secp256k1
        src/secp256k1/include
)
target_link_libraries (libwally-core secp256k1)


if (DEFINED IOS_PLATFORM)

    add_library(secp256k1 STATIC
        src/secp256k1/src/secp256k1.c
    )
    target_compile_options(libwally-core
        PRIVATE
            -W  -Wcast-align -Wnested-externs -Wshadow -Wstrict-prototypes
            -Wformat -Wformat-securit  -Wformat-nonliteral  -O3
            -D_FORTIFY_SOURCE=1 -fstack-protector-strong -DNDEBUG=1 -flto
            -flax-vector-conversions -Wno-unused-function -Wno-long-long
            -Wno-overlength-string -Wno-variadic-macros  -MP
    )
else()
    add_library(secp256k1 SHARED
         src/secp256k1/contrib/lax_der_parsing.c
         src/secp256k1/contrib/lax_der_privatekey_parsing.c
         src/secp256k1/src/secp256k1.c
    )
endif()

target_include_directories(secp256k1
    PUBLIC
        src/secp256k1
        src/secp256k1/src
        src/secp256k1/include
)
target_compile_options(secp256k1
    PRIVATE
        -std=c89
)
if(LINUX_DESKTOP)
    target_compile_definitions(secp256k1
        PUBLIC
            -DUSE_NUM_NONE=1 -DUSE_FIELD_5X52=1 -DUSE_FIELD_INV_BUILTIN=1
            -DUSE_SCALAR_4X64=1 -DUSE_SCALAR_INV_BUILTIN=1
            -DHAVE___INT128=1 -DVERIFY=1
        PRIVATE
            -DENABLE_MODULE_ECDH -DENABLE_MODULE_RECOVERY
            -DENABLE_MODULE_GENERATOR -DENABLE_MODULE_RANGEPROOF
            -DENABLE_MODULE_SURJECTIONPROOF
    )
else()
    target_compile_definitions(secp256k1
            PUBLIC
              -DHAVE_CONFIG_H
            PRIVATE
              -DENABLE_MODULE_RANGEPROOF -DENABLE_MODULE_SURJECTIONPROOF
              -DENABLE_MODULE_ECDH -DENABLE_MODULE_GENERATOR
    )
endif()
