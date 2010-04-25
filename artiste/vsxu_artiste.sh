#!/bin/sh
#cp "vsxu_artiste/Release - Linux32/vsxu_artiste.bin" .
export LD_LIBRARY_PATH=./
#export LD_DEBUG=bindings
#nice -n -10 ./vsxu_artiste.bin -f -s 1920,1200 -p 620,200
./vsxu_artiste.bin -s 900,600
