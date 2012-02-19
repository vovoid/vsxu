Vovoid VSXu Ultra
=================
©2003-2012 Vovoid Media Technologies


VSXu (VSX Ultra) is an OpenGL-based (hardware-accelerated), 
modular programming environment with its main purpose to 
visualize music and create real time graphic effects.

The aim is to bridge the gap between programmer 
and artist and enabling acreative and inspiring 
environment to work in for all parties involved.

VSXu is built on a modular plug-in-based architecture 
so anyone can extend it and or make visualization 
presets ("visuals" or "states").


How do i get it?
-----------------
Instructions for compiling a basic version of VSXu Ubuntu/Debian:

o) Make sure you have met the build dependencies:

sudo apt-get install libglew1.6-dev
sudo apt-get install libglfw-dev
sudo apt-get install libpng12-dev
sudo apt-get install libftgl-dev
sudo apt-get install libjpeg8-dev
sudo apt-get install libpulse-dev
sudo apt-get install libxrandr-dev
sudo apt-get install make
sudo apt-get install cmake
sudo apt-get install g++
sudo apt-get install gcc
sudo apt-get install git-core

Optional:
---------
sudo apt-get install libcv-dev libcvaux-dev

o) Get the VSXu Source from github

git clone git://github.com/vovoid/vsxu.git

o) Build it

cd vsxu
mkdir build
cd build
cmake ..
make
make install

For more advanced build instructions (and for the instructions for Windows users)
visit http://www.vsxu.com/development/compiling-from-source
