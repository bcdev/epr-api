# Copyright (c) 2013 Antonio Valentino <antonio.valentino@tiscali.it>

set(TARGETS api_unit_tests
            epr_main_test
            epr_subset_test
            epr_test_endian)

foreach(TEST ${TARGETS})
    add_executable(${TEST} ${TEST}.c)
    target_link_libraries(${TEST} epr_api)
endforeach(TEST)

target_link_libraries(epr_main_test bccunit)


# endianess
include(TestBigEndian)

TEST_BIG_ENDIAN(BIGENDIAN)
if(${BIGENDIAN})
    set(ENDIANESS BE)
else(${BIGENDIAN})
    set(ENDIANESS LE)
endif(${BIGENDIAN})


#add_test(TEST_API_01 api_unit_tests)
#add_test(TEST_EPR_01 epr_main_test)
#add_test(TEST_EPR_02 epr_subset_test)
add_test(TEST_EPR_03 epr_test_endian)
set_tests_properties(TEST_EPR_03 PROPERTIES PASS_REGULAR_EXPRESSION
    ${ENDIANESS})
