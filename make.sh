#!/bin/bash

SCRIPT_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

# Build directory contains the Makefile script for GNU Make build utility
BUILD_DIR="${SCRIPT_DIR}/build/GNUMake"

# Build the targets defined in the Makefile
# passing any command line arguments to GNU Make via $@
echo "make $@"
make -C "$BUILD_DIR" $@
