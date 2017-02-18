#!/bin/sh
BASE_PATH=`dirname $0`
cd $BASE_PATH
export LD_LIBRARY_PATH=./
$BASE_PATH/vsxu_player.bin
