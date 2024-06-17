#!/bin/bash

# cmake command to build. 

if [ ! -d "build" ]; then
    echo "creating 'build' directory."
    mkdir build
else
    echo "existing 'build' directory found."
fi

cd build
cmake -DCMAKE_PREFIX_PATH=$GRPC_CPP_INSTALL ..
cmake --build .
cd ..
