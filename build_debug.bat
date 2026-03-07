REM Create build directory
mkdir build
cd build

xcopy ../res/*.ttf . /Y

REM Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

REM Build
cmake --build . -j 8