#!/bin/bash

SCRIPT_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
SOURSE_DIR=$SCRIPT_DIR/build/CMake
BUILD_DIR=$SOURSE_DIR/gnumake

mkdir -p $BUILD_DIR
cmake -S $SOURSE_DIR -B $BUILD_DIR

cmake --build $BUILD_DIR --config Debug

cd $BUILD_DIR && \
ctest -C Debug --output-on-failure