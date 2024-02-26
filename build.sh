#!/bin/bash
rm -rf build/
rm -rf prismatic-game.pdx/ prismatic-game.pdx.zip
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=$PLAYDATE_SDK_PATH/C_API/buildsupport/arm.cmake ..
make
mv ../prismatic-game.pdx ./
zip -r prismatic-game.pdx.zip prismatic-game.pdx/
