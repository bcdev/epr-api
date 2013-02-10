# Copyright (c) 2013 Antonio Valentino <antonio.valentino@tiscali.it>

add_executable(api_tests api_tests.c)
target_link_libraries(api_tests epr_api)

#add_test(TEST_API_02 api_tests)
