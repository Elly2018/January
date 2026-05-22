#!/bin/bash

# Create build directory
mkdir -p build
cd build

cp ../res/*.ttf .
cp ../res/*.otf .

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build . -j 8