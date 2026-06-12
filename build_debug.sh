#!/bin/bash

# Create build directory
mkdir -p bin
mkdir -p build
cd bin

cp ../res/*.ttf .
cp ../res/*.otf .

cd ../build
# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

echo "Compile core: $(($(nproc) - 1))"

sleep 3

# Build
cmake --build . -j $(($(nproc) - 1))

