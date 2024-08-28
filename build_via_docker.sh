#!/bin/bash -ex

pushd $(dirname "$0")

git submodule update --init

# Make sure we have stack trace available
if ! grep MICROPY_PY_SYS_SETTRACE "$(pwd)/micropython/ports/rp2/mpconfigport.h"; then
    echo "#define MICROPY_PY_SYS_SETTRACE 1" >> "$(pwd)/micropython/ports/rp2/mpconfigport.h"
fi

pushd docker
docker build -t local_build_pico_https .
popd

docker run -v "$(pwd)":/source/ -it local_build_pico_https