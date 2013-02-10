# Copyright (c) 2013 Antonio Valentino <antonio.valentino@tiscali.it>

add_library(bccunit STATIC bccunit.c)

add_executable(bccunit_test bccunit_test.c)
target_link_libraries(bccunit_test bccunit)

add_test(BCCUNIT_TEST_SUITE_01 bccunit_test)
set_tests_properties(BCCUNIT_TEST_SUITE_01 PROPERTIES PASS_REGULAR_EXPRESSION
    "bccunit: test summary: 4 test case\\(s\\) total, 2 failure\\(s\\), 0 error\\(s\\)")
