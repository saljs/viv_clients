#!/bin/bash

BIN="$1"
FQBN=$(grep "FQBN=" Makefile | sed 's/FQBN=//')
PORT=${2:-"/dev/ttyUSB0"}
OPTIONS=${3:-"eesz=4M1M"}

if [ -z $BIN ]; then
    echo "Error: firmware file required."
    exit 1
fi

echo "Flashing $BIN to $PORT"
arduino-cli upload -b $FQBN --board-options $OPTIONS -p $PORT -i $BIN
