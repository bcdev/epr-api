# Copyright (c) 2013 Antonio Valentino <antonio.valentino@tiscali.it>

add_executable(epr_subset_test epr_subset_test.c)
target_link_libraries(epr_subset_test epr_api)

add_executable(epr_performance_test epr_performance_test.c)
target_link_libraries(epr_performance_test epr_api)

add_executable(epr_test_endian epr_test_endian.c)
target_link_libraries(epr_test_endian epr_api)

# the following test programs use some internal function that is not exported
# by the shared library
if(BUILD_STATIC_LIB)
    add_executable(epr_main_test epr_main_test.c)
    target_link_libraries(epr_main_test epr_api_static bccunit m)

    add_executable(api_unit_tests api_unit_tests.c)
    target_link_libraries(api_unit_tests epr_api_static m)
elseif(BUILD_STATIC_LIB)
    add_executable(epr_main_test epr_main_test.c ${SOURCES})
    target_link_libraries(epr_main_test bccunit m)

    add_executable(api_unit_tests api_unit_tests.c ${SOURCES})
    target_link_libraries(api_unit_tests m)
endif(BUILD_STATIC_LIB)


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
