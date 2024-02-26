#!/bin/bash
rm -rf build/
rm -rf prismatic-game.pdx/ prismatic-game.pdx.zip
mkdir build/
cd build
cmake ..
make
cd ..
pdsim ./prismatic-game.pdx
