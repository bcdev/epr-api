#include "bccunit.h"


static struct BcUnitTest g_test_buf[MAX_NUM_TEST_CASES];
static long g_test_buf_index = 0;


BcTest bc_create_test_impl(const char* name, BcTestFunc func, void* user_data);
void bc_run_test_impl(BcTest test, BcTestResult* result, int level);


BcTestCase bc_create_test_case(const char* name, BcTestFunc func, void* user_data) {
    assert(func != NULL);
    return bc_create_test_impl(name, func, user_data);
}


BcTestSuite bc_create_test_suite(const char* name) {
    return bc_create_test_impl(name, NULL, NULL);
}


BcTest bc_create_test_impl(const char* name, BcTestFunc func, void* user_data) {
    BcTest test_new = NULL;

    assert(name != NULL);

    if (g_test_buf_index >= MAX_NUM_TEST_CASES) {
        fprintf(stderr, "bccunit error: no more test cases available\n");
        return NULL;
    }

    if (strlen(name) > MAX_LEN_TEST_NAME) {
        fprintf(stderr, "bccunit error: test case name too long (max. length = %d): \"%s\"\n", MAX_LEN_TEST_NAME, name);
        return NULL;
    }

    test_new = &g_test_buf[g_test_buf_index++];

    strcpy(test_new->name, name);
    test_new->func  = func;
    test_new->user_data  = user_data;
    test_new->test_first = NULL;
    test_new->test_next  = NULL;
    test_new->test_last  = NULL;
    test_new->num_tests  = (func != NULL) ? 1 : 0;

    return test_new;
}


void bc_add_test_case(BcTestSuite test_suite, const char* name, BcTestFunc func) {
    BcTestCase test_case = bc_create_test_case(name, func, NULL);
    bc_add_test(test_suite, test_case);
}


void bc_add_test(BcTestSuite test_suite, BcTest test) {
    assert(test_suite != NULL);
    assert(test_suite->func == NULL);
    assert(test != NULL);
    assert(test->test_next == NULL);

    if (test_suite->test_last != NULL) {
        test_suite->test_last->test_next = test;
        test_suite->test_last = test;
    } else {
        test_suite->test_first = test;
        test_suite->test_last  = test;
    }
    test_suite->num_tests++;
}


void bc_run_test(BcTest test, BcTestResult* result) {
    assert(result != NULL);

    result->num_test_cases = 0;
    result->num_failures = 0;
    result->num_errors = 0;

    bc_run_test_impl(test, result, 0);

    printf("bccunit: test summary: %d test case(s) total, %d failure(s), %d error(s)\n",
           result->num_test_cases,
           result->num_failures,
           result->num_errors);
}


void bc_print_message_prefix(int level) {
    int indent;
    printf("bccunit: ");
    for (indent = 0; indent < level; indent++) {
        printf("  ");
    }
}


void bc_run_test_impl(BcTest test, BcTestResult* result, int level) {
    assert(test != NULL);
    assert(result != NULL);

    if (test->func != NULL) { /* it´a BcTest */
        int status;

        bc_print_message_prefix(level);
        printf("running test case \"%s\"\n", test->name);

        status = test->func(test->user_data);

        result->num_test_cases += 1;
        result->num_failures += (status >= BC_TEST_FAILURE) ? status : 0;
        result->num_errors += (status == BC_TEST_ERROR)   ? 1 : 0;
    } else { /* If test or func not set --> it's a suite */
        BcTest sub_test;

        bc_print_message_prefix(level);
        printf("running test suite \"%s\" containing %d test(s)\n", test->name, test->num_tests);

        for (sub_test = test->test_first; sub_test != NULL; sub_test = sub_test->test_next) {
            bc_run_test_impl(sub_test, result, level + 1);
        }
    }
}


int bc_assert_cond(const char* msg, int cond, const char* file_path, long line_no) {
    return bc_assert_status(msg,
                            cond != 0 ? BC_TEST_SUCCESS : BC_TEST_FAILURE,
                            file_path,
                            line_no);
}


int bc_assert_status(const char* msg, int status, const char* file_path, long line_no) {
    const char* reason = NULL;
    if (status == BC_TEST_FAILURE) {
        reason = "failure";
    } else if (status == BC_TEST_ERROR) {
        reason = "error";
    }
    if (reason != NULL) {
        printf("bccunit: %s: file %s: line %ld: %s\n", reason, file_path, line_no, msg);
    }
    return status;
}
