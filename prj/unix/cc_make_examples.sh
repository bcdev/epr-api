#!/bin/bash

if [ ! -f "../../bin" ]
then
    mkdir ../../bin
fi

if [ -f "../../bin/write_bands" ]
then
    rm ../../bin/write_bands
fi

if [ -f "../../bin/write_bitmask" ]
then
    rm ../../bin/write_bitmask
fi

if [ -f "../../bin/write_ndvi" ]
then
    rm ../../bin/write_ndvi
fi

cc -lm ../../src/*.c ../../src/examples/write_bands.c -o ../../bin/write_bands
cc -lm ../../src/*.c ../../src/examples/write_bitmask.c -o ../../bin/write_bitmask
cc -lm ../../src/*.c ../../src/examples/write_ndvi.c -o ../../bin/write_ndvi

if [ ! -f "../../bin/write_bands" ]
then
    echo ""
    echo "Error:"
    echo "--> write_bands could not be compiled"
else
    echo "--> write_bands compiled successful"
fi

if [ ! -f "../../bin/write_bitmask" ]
then
    echo ""
    echo "Error:"
    echo "--> write_bitmask could not be compiled"
else
    echo "--> write_bitmask compiled successful"
fi

if [ ! -f "../../bin/write_ndvi" ]
then
    echo ""
    echo "Error:"
    echo "--> write_ndvi could not be compiled"
else
    echo "--> write_ndvi compiled successful"
fi


