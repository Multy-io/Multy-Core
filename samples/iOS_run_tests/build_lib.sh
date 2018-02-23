#!/bin/sh
mkdir iOS
cd iOS
cmake ../../../  -G'Unix Makefiles' -DCMAKE_TOOLCHAIN_FILE=../../../tools/ios-cmake/ios.toolchain.cmake -DIOS_PLATFORM=OS -DIOS_DEPLOYMENT_TARGET=9.0 -DCMAKE_OSX_ARCHITECTURES=arm64 -DMULTY_WITH_TESTS=1 -DCMAKE_CONFIGURATION_TYPES=Release
cmake --build .

cp -rf multy_core/libmulty_core.a ../iOS_run_tests/Libraries/libmulty_core.a
cp -rf multy_test/libmulty_test.dylib ../iOS_run_tests/Libraries/libmulty_test.dylib
cp -rf third-party/googletest/googlemock/gtest/libgtest.a ../iOS_run_tests/Libraries/libgtest.a
cp -rf third-party/libkeccak-tiny.a ../iOS_run_tests/Libraries/libkeccak-tiny.a
cp -rf third-party/libmini-gmp.a ../iOS_run_tests/Libraries/libmini-gmp.a
cp -rf third-party/libwally-core/liblibwally-core.a ../iOS_run_tests/Libraries/liblibwally-core.a
cp -rf third-party/libwally-core/libsecp256k1.a ../iOS_run_tests/Libraries/libsecp256k1.a

cd ..
rm -rf iOS


