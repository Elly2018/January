#!/bin/bash

# Create build directory
mkdir -p build
cd build

cp ../Roboto-Medium.ttf .

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . -j 8