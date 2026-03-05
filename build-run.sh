#!/bin/bash
rm -rf build/
rm -rf prismatic-game.pdx/ prismatic-game.pdx.zip
mkdir build
cd build
cmake ..
make
mv ../prismatic-game.pdx ./
zip -r prismatic-game.pdx.zip prismatic-game.pdx/
pdsim ./prismatic-game.pdx
