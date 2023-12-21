#!/bin/bash

# Parse args.
if [[ "$1" == "-h" ]]; then
    echo "Usage: $(basename "$0") [-jTHREADS] BUILD_PLATFORM BUILD_ARCH REPO_DIR"
    exit 1
fi

THREADS_ARG=
if [[ "$1" == -j* ]]; then
    THREADS_ARG="$1"
    shift 1
fi

if [[ "$#" != 3 ]]; then
    echo "Two arguments required. Use -h for help."
    exit 1
fi

BUILD_PLATFORM="$1"
BUILD_ARCH="$2"
REPO_DIR="$3"

# Check ANDROID_NDK.
if [[ "$BUILD_PLATFORM" == "android" && "$ANDROID_NDK" == "" ]]; then
    echo "Please provide path to your NDK via ANDROID_NDK environment variable!"
    exit 1
fi

# Echo on, fail on errors, fail on undefined var usage, fail on pipeline failure.
set -euxo pipefail

# Prepare cmake flags.
ADDITIONAL_CMAKE_ARGS=()
RUN_TESTS=true
if [[ "$BUILD_PLATFORM" == "darwin" ]]; then
    ADDITIONAL_CMAKE_ARGS+=(
        "-DCMAKE_OSX_ARCHITECTURES=$BUILD_ARCH"
    )
    if [[ "$BUILD_ARCH" == "arm64" ]]; then
        # This is a cross-compile, can't run tests.
        RUN_TESTS=false

        # Don't try running the tests for regex impl, we're cross-compiling and they won't run.
        ADDITIONAL_CMAKE_ARGS+=(
            "-DHAVE_STD_REGEX=ON"
        )
    fi
elif [[ "$BUILD_PLATFORM" == "windows" ]]; then
    ADDITIONAL_CMAKE_ARGS+=(
        "-GNinja" # Use Ninja on windows. Default is MSVC project files, which are multi-config, and it's a mess.
    )
elif [[ "$BUILD_PLATFORM" == "linux" ]]; then
    if [[ "$BUILD_ARCH" == "x86" ]]; then
        ADDITIONAL_CMAKE_ARGS+=(
            "-DCMAKE_CXX_FLAGS=-m32"
            "-DCMAKE_C_FLAGS=-m32"
        )
    fi
elif [[ "$BUILD_PLATFORM" == "android" ]]; then
    if [[ "$BUILD_ARCH" == "arm32" ]]; then
        ANDROID_ARCH_ABI=armeabi-v7a
    elif [[ "$BUILD_ARCH" == "arm64" ]]; then
        ANDROID_ARCH_ABI=arm64-v8a
    elif [[ "$BUILD_ARCH" == "x86" ]]; then
        ANDROID_ARCH_ABI=x86
    elif [[ "$BUILD_ARCH" == "x86_64" ]]; then
        ANDROID_ARCH_ABI=x86_64
    fi

    ANDROID_PLATFORM_VERSION=21
    ANDROID_TOOLCHAIN=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64

    # These are derived from what gradle passes to cmake.
    # Args with ANDROID_ prefix are processed by the toolchain file.
    ADDITIONAL_CMAKE_ARGS+=(
        "-DCMAKE_SYSTEM_NAME=Android"
        "-DCMAKE_SYSTEM_VERSION=$ANDROID_PLATFORM_VERSION"
        "-DANDROID_PLATFORM=android-$ANDROID_PLATFORM_VERSION"
        "-DANDROID_ABI=$ANDROID_ARCH_ABI"
        "-DCMAKE_ANDROID_ARCH_ABI=$ANDROID_ARCH_ABI"
        "-DANDROID_NDK=$ANDROID_NDK"
        "-DCMAKE_ANDROID_NDK=$ANDROID_NDK"
        "-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake"
        "-DANDROID_STL=c++_static"
    )

    # This is a cross-compile, can't run tests.
    RUN_TESTS=false
fi

function build_test_one() {
    local BUILD_TYPE="$1"
    local BUILD_DIR="$2"
    local FORMAT_LIB="$3"
    local FLOAT_LIB="$4"
    local TYPE_NAME_IMPL="$5"
    local ENUM_HASH_LIB="$6"

    echo "================================================================================================"

    cmake \
        -B "$BUILD_DIR" \
        -S "$REPO_DIR" \
        "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" \
        "-DSN_FORMAT_LIB=$FORMAT_LIB" \
        "-DSN_FLOAT_LIB=$FLOAT_LIB" \
        "-DSN_TYPE_NAME_IMPL=$TYPE_NAME_IMPL" \
        "-DSN_ENUM_HASH_LIB=$ENUM_HASH_LIB" \
        "-DSN_CHECK_STYLE=OFF" \
        "${ADDITIONAL_CMAKE_ARGS[@]}"
    cmake --build "$BUILD_DIR" $THREADS_ARG

    if [[ "$RUN_TESTS" == "true" ]]; then
        cmake --build "$BUILD_DIR" --target run_all_tests
        cmake --build "$BUILD_DIR" --target run_all_benchmarks
    fi
}

for BUILD_TYPE in "Debug" "Release"
do
    build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-1" "fmt_bundled" "fast_float_bundled" "funcsig" "std"

    # Only MSVC has <format>, unfortunately.
    if [[ "$BUILD_PLATFORM" == "windows" ]]; then
        build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-2" "std" "fast_float_bundled" "funcsig" "std"
    fi

    build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-3" "fmt_bundled" "strtof" "funcsig" "std"

    # AppleClang and Android clang don't have floating-point std::from_chars
    if [[ "$BUILD_PLATFORM" != "darwin" && "$BUILD_PLATFORM" != "android" ]]; then
        build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-4" "fmt_bundled" "from_chars" "funcsig" "std"
    fi

    build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-5" "fmt_bundled" "fast_float_bundled" "typeid" "std"

    build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-6" "fmt_bundled" "fast_float_bundled" "funcsig" "frozen_bundled"
done
