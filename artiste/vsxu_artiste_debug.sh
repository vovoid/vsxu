#!/bin/sh
#cp "vsxu_artiste/Release - Linux32/vsxu_artiste.bin" .
export LD_LIBRARY_PATH=./
#export LD_DEBUG=bindings
gdb ./vsxu_artiste.bin -p 620,200
