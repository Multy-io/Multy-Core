if(LIBWALLY_BIN)
    if (IOS_PLATFORM)
        add_library(libwally-core STATIC IMPORTED GLOBAL)
    else(IOS_PLATFORM)
        add_library(libwally-core SHARED IMPORTED GLOBAL)
    endif(IOS_PLATFORM)

    set_target_properties(libwally-core PROPERTIES IMPORTED_LOCATION
        ${LIBWALLY_BIN}
    )
    message("Using prebuild libwally-core : ${LIBWALLY_BIN}")
elseif(LIBWALLY_BIN_PATH)
    add_library(libwally-core SHARED IMPORTED GLOBAL)
    message("ANDROID_ABI : ${ANDROID_ABI}")
    # TODO: use FIND_LIBRARY
    if (ANDROID_ABI)
        set_target_properties(libwally-core PROPERTIES IMPORTED_LOCATION
            ${LIBWALLY_BIN_PATH}/${ANDROID_ABI}/libwallycore.so)
    else()
        set_target_properties(libwally-core PROPERTIES IMPORTED_LOCATION
            ${LIBWALLY_BIN_PATH}/libwallycore.so)
    endif()
    message("Found prebuild libwally-core at : ${LIBWALLY_BIN_PATH}")
else()
    message("Building libwally-core from source")
    set(wally_sources
        libwally-core/src/aes.c
        libwally-core/src/base58.c
        libwally-core/src/bip32.c
        libwally-core/src/bip38.c
        libwally-core/src/bip39.c
    #    libwally-core/src/ccan/ccan/build_assert/test/compile_fail.c
    #    libwally-core/src/ccan/ccan/build_assert/test/compile_fail-expr.c
    #    libwally-core/src/ccan/ccan/build_assert/test/compile_ok.c
    #    libwally-core/src/ccan/ccan/build_assert/test/run-BUILD_ASSERT_OR_ZERO.c
    #    libwally-core/src/ccan/ccan/compiler/test/compile_fail-printf.c
    #    libwally-core/src/ccan/ccan/compiler/test/run-is_compile_constant.c
        libwally-core/src/ccan/ccan/crypto/ripemd160/ripemd160.c
    #    libwally-core/src/ccan/ccan/crypto/ripemd160/test/run-lotsa-data.c
    #    libwally-core/src/ccan/ccan/crypto/ripemd160/test/run-test-vectors.c
    #    libwally-core/src/ccan/ccan/crypto/ripemd160/test/run-types.c
        libwally-core/src/ccan/ccan/crypto/sha256/sha256.c
    #    libwally-core/src/ccan/ccan/crypto/sha256/test/run-33-bit-test.c
    #    libwally-core/src/ccan/ccan/crypto/sha256/test/run-lotsa-data.c
    #    libwally-core/src/ccan/ccan/crypto/sha256/test/run-test-vectors.c
    #    libwally-core/src/ccan/ccan/crypto/sha256/test/run-types.c
        libwally-core/src/ccan/ccan/crypto/sha512/sha512.c
    #    libwally-core/src/ccan/ccan/endian/test/compile_ok-constant.c
    #    libwally-core/src/ccan/ccan/endian/test/run.c
        libwally-core/src/ccan/ccan/str/hex/hex.c
    #    libwally-core/src/ccan/ccan/str/hex/test/run.c
    #    libwally-core/src/ccan/ccan/tap/tap.c
    #    libwally-core/src/ccan/ccan/tap/test/run.c
    #    libwally-core/src/cpufeatures/cpu-features.c # Android ?
    #    libwally-core/src/ctaes/bench.c
    #    libwally-core/src/ctaes/ctaes.c
    #    libwally-core/src/ctaes/test.c
    #    libwally-core/src/ctest/test_clear.c # move to test ?
    #    libwally-core/src/data/wordlists/chinese_simplified.c
    #    libwally-core/src/data/wordlists/chinese_traditional.c
    #    libwally-core/src/data/wordlists/english.c
    #    libwally-core/src/data/wordlists/french.c
    #    libwally-core/src/data/wordlists/italian.c
    #    libwally-core/src/data/wordlists/japanese.c
    #    libwally-core/src/data/wordlists/spanish.c
        libwally-core/src/elements.c
        libwally-core/src/hex.c
        libwally-core/src/hmac.c
        libwally-core/src/internal.c
        libwally-core/src/mnemonic.c
        libwally-core/src/pbkdf2.c
        libwally-core/src/scrypt.c
    #    libwally-core/src/scrypt/crypto_scrypt.c
    #    libwally-core/src/scrypt/crypto_scrypt_smix.c
    #    libwally-core/src/scrypt/crypto_scrypt_smix_neon.c
    #    libwally-core/src/scrypt/crypto_scrypt_smix_sse2.c
        libwally-core/src/sign.c
        libwally-core/src/wordlist.c
    #    libwally-core/src/wrap_js/src/combined.c
    #    libwally-core/src/wrap_js/src/combined_ccan2.c
    #    libwally-core/src/wrap_js/src/combined_ccan.c
    )

    add_library(libwally-core SHARED ${wally_sources})
    target_include_directories(libwally-core PRIVATE
        .
        libwally-core
        libwally-core/src
        libwally-core/src/ccan
        libwally-core/src/secp256k1
        libwally-core/src/secp256k1/include
    )
    target_link_libraries(libwally-core secp256k1)
endif()

if(LIBSECP256K1_BIN)
    add_library(secp256k1 SHARED IMPORTED GLOBAL)
    set_target_properties(secp256k1
        PROPERTIES IMPORTED_LOCATION
        ${LIBSECP256K1_BIN})
    message("Using prebuild secp256k1 ${LIBWALLY_BIN}")

elseif (LIBSECP256K1_BIN_PATH)
    add_library(secp256k1 SHARED IMPORTED GLOBAL)
    if (ANDROID_ABI)
        set_target_properties(secp256k1 PROPERTIES IMPORTED_LOCATION
            ${LIBSECP256K1_BIN_PATH}/${ANDROID_ABI}/secp256k1.so)
    else()
        set_target_properties(secp256k1 PROPERTIES IMPORTED_LOCATION
            ${LIBSECP256K1_BIN_PATH}/secp256k1.so)
    endif()
    message("Using prebuild secp256k1 from ${LIBSECP256K1_BIN_PATH}")

else()

message("Building libsecp256k1 from source")
# secp256k1 cmake-stuff was inspired by
# https://github.com/chfast/secp256k1/blob/cb5dc20ecc116d27452f8ecb1ae50205834da172/CMakeLists.txt
add_library(secp256k1 SHARED
    libwally-core/src/secp256k1/contrib/lax_der_parsing.c
    libwally-core/src/secp256k1/contrib/lax_der_privatekey_parsing.c
#    libwally-core/src/secp256k1/src/asm/field_10x26_arm.s
#    libwally-core/src/secp256k1/src/bench_ecdh.c
#    libwally-core/src/secp256k1/src/bench_generator.c
#    libwally-core/src/secp256k1/src/bench_internal.c
#    libwally-core/src/secp256k1/src/bench_rangeproof.c
#    libwally-core/src/secp256k1/src/bench_recover.c
#    libwally-core/src/secp256k1/src/bench_schnorr_verify.c
#    libwally-core/src/secp256k1/src/bench_sign.c
#    libwally-core/src/secp256k1/src/bench_verify.c
#    libwally-core/src/secp256k1/src/gen_context.c
#    libwally-core/src/secp256k1/src/java/org_bitcoin_NativeSecp256k1.c
#    libwally-core/src/secp256k1/src/java/org_bitcoin_Secp256k1Context.c
    libwally-core/src/secp256k1/src/secp256k1.c
#    libwally-core/src/secp256k1/src/tests.c
#    libwally-core/src/secp256k1/src/tests_exhaustive.c
)
target_include_directories(secp256k1
    PUBLIC
        libwally-core/src/secp256k1
        libwally-core/src/secp256k1/src
        libwally-core/src/secp256k1/include
    PRIVATE
)
target_compile_definitions(secp256k1
    PUBLIC -DUSE_NUM_NONE=1 -DUSE_FIELD_5X52=1 -DUSE_FIELD_INV_BUILTIN=1 -DUSE_SCALAR_4X64=1 -DUSE_SCALAR_INV_BUILTIN=1 -DHAVE___INT128=1 -DVERIFY=1
    PRIVATE -DENABLE_MODULE_ECDH -DENABLE_MODULE_RECOVERY -DENABLE_MODULE_GENERATOR -DENABLE_MODULE_RANGEPROOF -DENABLE_MODULE_SURJECTIONPROOF
)
endif()

add_library(keccak-tiny STATIC
    ../third-party/keccak-tiny/keccak-tiny.c
)

set_target_properties(keccak-tiny PROPERTIES
    C_STANDARD 11
    POSITION_INDEPENDENT_CODE ON
)

if(DEFINED WITH_TESTS)
    set(BUILD_SHARED_LIBS YES)
    add_subdirectory(googletest)
    set_target_properties(gtest PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
    set_target_properties(gmock PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endif(DEFINED WITH_TESTS)
