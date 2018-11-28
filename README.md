[![Build Status](https://travis-ci.com/Multy-io/Multy-Core.svg?branch=master)](https://travis-ci.com/Multy-io/Multy-Core)

# Multy-Core
Cross-platform mobile-first library for HD wallets and creating raw transactions of Bitcoin and Ethereum (and many more to come).

# How to build for iOS:
```
$ cmake ../Multy-Core  -G'Unix Makefiles' -DCMAKE_TOOLCHAIN_FILE=../Multy-Core/tools/ios-cmake/ios.toolchain.cmake -DIOS_PLATFORM=OS -DIOS_DEPLOYMENT_TARGET=9.0 -DCMAKE_OSX_ARCHITECTURES=arm64 -DMULTY_WITH_TESTS=1
$ cmake --build . # or `open ./multy.xcodeproj` and do a build with Xcode
```

# How to build for Android:
Link the root CMakeLists.txt to your gradle file and set arguments to:
```
"-DMULTY_ANDROID_PATH_TO_JNI_WRAPPER=/Users/pavel/AndroidStudioProjects/Multi/app/src/main/cpp/scratch.cpp"
```
