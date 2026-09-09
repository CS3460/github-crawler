#!/usr/bin/env bash
set -e

BUILD_DIR="build"
BUILD_TYPE="Release"

mkdir -p "$BUILD_DIR"

cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_CXX_COMPILER=clang++

# Build using all available CPU cores
cmake --build "$BUILD_DIR" --parallel

echo ""
echo "=== Build Complete ==="
echo "Run target with: ./${BUILD_DIR}/github-crawler"
