#!/bin/bash -ex

cd /source/micropython

make -C mpy-cross

pushd ports/rp2
make clean
make BOARD=RPI_PICO_W submodules
make BOARD=RPI_PICO_W USER_C_MODULES=/source/userlibs/excprinter/micropython.cmake

cp /source/micropython/ports/rp2/build-RPI_PICO_W/firmware.uf2 /source/images/