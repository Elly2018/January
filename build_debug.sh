#!/bin/bash

# Create build directory
mkdir -p build
cd build

cp ../Roboto-Medium.ttf .

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build . -j 8