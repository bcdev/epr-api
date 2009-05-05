#!/bin/bash

if [ ! -f "../../../bin" ]
then
    mkdir ../../../bin
fi

if [ -f "../../../bin/epr_main_test" ]
then
    rm ../../../bin/epr_main_test
fi

cc -lm ../../../bccunit/src/bccunit.c ../../../src/*.c ../../../src/test/epr_main_test.c -o ../../../bin/epr_main_test

if [ ! -f "../../../bin/epr_main_test" ]
then
    echo ""
    echo "Error:"
    echo "--> epr_main_test could not be compiled"
else
    echo "--> epr_main_test compiled successful"
fi
