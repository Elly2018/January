@echo off

REM Create build directory
mkdir bin
mkdir build
cd bin

xcopy ../res/*.ttf . /Y
xcopy ../res/*.otf . /Y

cd ../build
REM Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

REM Build
cmake --build .