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

Compilation Instructions for a basic version of VSXu Ubuntu/Debian:

Make sure you have met the build dependencies:

      sudo apt-get install libglew1.6-dev libglfw-dev libpng12-dev libftgl-dev libjpeg8-dev libpulse-dev libxrandr-dev make cmake g++ gcc git-core

Optional dependencies:

      sudo apt-get install libcv-dev libcvaux-dev libhighgui-dev

Get the VSXu Source from github:

      git clone git://github.com/vovoid/vsxu.git

Build it:

      cd vsxu
      mkdir build
      cd build
      cmake ..
      make
      make install

For more advanced build instructions (and for the instructions for Windows users)
visit http://www.vsxu.com/development/compiling-from-source
