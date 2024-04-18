#!/bin/bash

BIN="$1"
FQBN=$(grep "FQBN=" Makefile | sed 's/FQBN=//')
PORT=${2:-"/dev/ttyUSB0"}

if [ -z $BIN ]; then
    echo "Error: firmware file required."
    exit 1
fi

echo "Flashing $BIN to $PORT"
arduino-cli upload -b $FQBN -p $PORT -i $BIN
