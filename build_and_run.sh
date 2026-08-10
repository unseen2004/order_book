#!/bin/bash

set -e

echo "Building HFT with -O3 optimization..."
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O3" ..
make -j$(nproc)

echo ""
echo "Running HFT..."
./hft
