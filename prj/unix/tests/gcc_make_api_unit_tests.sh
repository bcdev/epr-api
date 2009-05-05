#!/bin/bash

if [ ! -f "../../../bin" ]
then
    mkdir ../../../bin
fi

if [ -f "../../../bin/api_unit_tests" ]
then
    rm ../../../bin/api_unit_tests
fi


gcc -lm ../../../bccunit/src/bccunit.c ../../../src/*.c ../../../src/test/api_unit_tests.c -o ../../../bin/api_unit_tests

if [ ! -f "../../../bin/api_unit_tests" ]
then
    echo ""
    echo "Error:"
    echo "--> api_unit_tests could not be compiled"
else
    echo "--> api_unit_tests compiled successful"
fi
