REM Create build directory
mkdir build
cd build

xcopy ../res/*.ttf . /Y
xcopy ../res/*.otf . /Y

REM Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

REM Build
cmake --build . -j 8