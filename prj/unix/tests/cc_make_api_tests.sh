#!/bin/bash

if [ ! -f "../../../bin" ]
then
    mkdir ../../../bin
fi

if [ -f "../../../bin/api_tests" ]
then
    rm ../../../bin/api_tests
fi


cc -lm ../../../src/*.c ../../../src/api-test/api_tests.c -o ../../../bin/api_tests

if [ ! -f "../../../bin/api_tests" ]
then
    echo ""
    echo "Error:"
    echo "--> api_tests could not be compiled"
else
    echo "--> api_tests compiled successful"
fi
