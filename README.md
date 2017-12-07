# Multy-Core
Coming soon..


# For run on iOS:
"""
$ cmake ../Multy-Core  -GXcode -DCMAKE_TOOLCHAIN_FILE=../Multy-Core/tools/ios-cmake/ios.toolchain.cmake -DIOS_PLATFORM=OS -DIOS_DEPLOYMENT_TARGET=10.0 -DCMAKE_OSX_ARCHITECTURES=arm64 -DWITH_TESTS=1
"""
# For make on Android:
add to gradle file:
1) arguments "-DPATH_TO_JNI_WRAPPER=/Users/pavel/AndroidStudioProjects/Multi/app/src/main/cpp/scratch.cpp"
2) path "/Users/pavel/Documents/project/multy/MUL_fix_android/Multy-Core/CMakeLists.txt"
