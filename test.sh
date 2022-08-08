#!/bin/sh
set -e
set -x

DATAURL=http://www.brockmann-consult.de/beam/data/products/MERIS/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.zip

if [ ! -d testdata ]
then
    mkdir testdata
    cd testdata
    wget ${DATAURL}
    # cp $(xdg-user-dir DOWNLOAD)/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.zip .
    unzip MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.zip
    cd -
fi

mkdir -p build/release
make all_rel

export LD_LIBRARY_PATH=${PWD}/build/release:${LD_LIBRARY_PATH}

cd bccunit/src/
cc bccunit_test.c bccunit.c -L../../build/release/ -lepr_api -o bccunit_test
./bccunit_test
cd -

cd src/api-test
mkdir out
gcc api_tests.c -L../../build/release/ -lepr_api -o api_tests
./api_tests ../../testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1 out
cd -

cd src/test

cc api_unit_tests.c ../../bccunit/src/bccunit.c -L../../build/release/ -lepr_api -o api_unit_tests
cc epr_main_test.c ../../bccunit/src/bccunit.c -L../../build/release/ -lepr_api -o epr_main_test
cc epr_performance_test.c ../../bccunit/src/bccunit.c -L../../build/release/ -lepr_api -o epr_performance_test
cc epr_subset_test.c ../../bccunit/src/bccunit.c -L../../build/release/ -lepr_api -o epr_subset_test
cc epr_test_endian.c ../../bccunit/src/bccunit.c -L../../build/release/ -lepr_api -o epr_test_endian

./api_unit_tests
./epr_performance_test ../../testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1
./epr_subset_test ../../testdata/MER_RR__2PNRAL20100429_160201_000003102089_00040_42679_0001.N1 reflec_5
./epr_test_endian

cd -

./src/test/epr_main_test
