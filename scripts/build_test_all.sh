#!/bin/bash

# Parse args.
if [[ "$1" == "-h" ]]; then
    echo "Usage: $(basename "$0") [-jTHREADS] BUILD_PLATFORM REPO_DIR"
    exit 1
fi

THREADS_ARG=
if [[ "$1" == -j* ]]; then
    THREADS_ARG="$1"
    shift 1
fi

if [[ "$#" != 2 ]]; then
    echo "Two arguments required. Use -h for help."
    exit 1
fi

BUILD_PLATFORM="$1"
REPO_DIR="$2"

# Echo on, fail on errors, fail on undefined var usage, fail on pipeline failure.
set -euxo pipefail

function build_test_one() {
    local BUILD_TYPE="$1"
    local BUILD_DIR="$2"
    local FORMAT_LIB="$3"
    local FLOAT_LIB="$4"
    local TYPE_NAME_IMPL="$5"

    echo "================================================================================================"

    cmake \
        -B "$BUILD_DIR" \
        -S "$REPO_DIR" \
        "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" \
        "-DSN_FORMAT_LIB=$FORMAT_LIB" \
        "-DSN_FLOAT_LIB=$FLOAT_LIB" \
        "-DSN_TYPE_NAME_IMPL=$TYPE_NAME_IMPL" \
        "-DSN_CHECK_STYLE=OFF"
    cmake --build "$BUILD_DIR" $THREADS_ARG
    cmake --build "$BUILD_DIR" --target run_all_tests
}

for BUILD_TYPE in "Debug" "Release"
do
    build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-1" "fmt_bundled" "fast_float_bundled" "funcsig"

    # Only MSVC has <format>, unfortunately.
    if [[ "$BUILD_PLATFORM" == "windows" ]]; then
        build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-2" "std" "fast_float_bundled" "funcsig"
    fi

    build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-3" "fmt_bundled" "strtof" "funcsig"

    # AppleClang doesn't have floating-point std::from_chars
    if [[ "$BUILD_PLATFORM" != "darwin" ]]; then
        build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-3" "fmt_bundled" "from_chars" "funcsig"
    fi

    build_test_one "$BUILD_TYPE" "build-$BUILD_TYPE-4" "fmt_bundled" "fast_float_bundled" "typeid"
done
