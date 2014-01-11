#!/bin/sh
# Configure Linux Release version into build/linux
mkdir -p linux
cd linux
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" ../..
cd ..
