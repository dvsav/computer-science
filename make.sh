#!/bin/bash

SCRIPT_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
BLD_DIR="${SCRIPT_DIR}/build/GNUMake"

echo "make $@"
make -C "$BLD_DIR" $@
