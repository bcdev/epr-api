#ifndef BCCUNIT_H_INCL
#define BCCUNIT_H_INCL

#ifdef __cplusplus
extern "C" {
    #endif /* __cplusplus */


    #include <setjmp.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <assert.h>



    /**
     * Maximum number of test cases.
     * Increase this number when you get the error message
     * "no more test cases available"
     */
    #define MAX_NUM_TEST_CASES 1024

    /**
     * Maximum string length of a test case name.
     * Increase this number when you get the error message
     * "test case name too long"
     */
    #define MAX_LEN_TEST_NAME 64


    #define BC_TEST_SUCCESS  0
    #define BC_TEST_FAILURE  1
    #define BC_TEST_ERROR    -1

    typedef struct {
        int num_test_cases;
        int num_failures;
        int num_errors;
    }
    BcTestResult;

    typedef int (*BcTestFunc)(void* user_data);
    typedef void (*BcTestResultFunc)(BcTestResult* result, void* user_data);

    struct BcUnitTest {
        char               name[MAX_LEN_TEST_NAME + 1];
        BcTestFunc         func;       /* always NULL for test suites */
        void*              user_data;  /* always NULL for test suites, optional user data, passed as arg to func call */
        struct BcUnitTest* test_first; /* valid for test suites only */
        struct BcUnitTest* test_last;  /* valid for test suites only */
        struct BcUnitTest* test_next;  /* valid for test cases only */
        int                num_tests;  /* valid for test suites only, for cases aways 1 */
    };


    typedef struct BcUnitTest* BcTest;
    typedef struct BcUnitTest* BcTestCase;
    typedef struct BcUnitTest* BcTestSuite;


    BcTestSuite bc_create_test_suite(const char* name);
    BcTestCase bc_create_test_case(const char* name, BcTestFunc func, void* user_data);
    void bc_add_test(BcTestSuite test_suite, BcTest test);
    void bc_add_test_case(BcTestSuite test_suite, const char* name, BcTestFunc func);
    void bc_run_test(BcTest test, BcTestResult* result);
    int bc_assert_cond(const char* msg, int cond, const char* file_path, long line_no);
    int bc_assert_status(const char* msg, int status, const char* file_path, long line_no);


    #define BC_BEGIN_TEST(TEST) \
    int TEST(void* user_data) {

        #define BC_END_TEST() \
        return (BC_TEST_SUCCESS); }

    #define BC_ASSERT_TRUE(EXPR) \
    BC_ASSERT_IMPL("(" #EXPR ")", (EXPR))

    #define BC_ASSERT_FALSE(EXPR) \
    BC_ASSERT_IMPL("!(" #EXPR ")", !(EXPR))

    #define BC_ASSERT_NOT_NULL(EXPR) \
    BC_ASSERT_IMPL("(" #EXPR ") != NULL", (EXPR) != NULL)

    #define BC_ASSERT_NULL(EXPR) \
    BC_ASSERT_IMPL("(" #EXPR ") == NULL", (EXPR) == NULL)

    #define BC_ASSERT_SAME(EXPR1, EXPR2) \
    BC_ASSERT_IMPL("(" #EXPR1 ") == (" #EXPR2 ")", (EXPR1) == (EXPR2))

    #define BC_ASSERT_NOT_SAME(EXPR1, EXPR2) \
    BC_ASSERT_IMPL("(" #EXPR1 ") != (" #EXPR2 ")", (EXPR1) != (EXPR2))

    #define BC_FAIL(MSG) \
    BC_ASSERT_IMPL(MSG, 0)

    #define BC_ERROR(MSG) \
    { return bc_assert_status(MSG, BC_TEST_ERROR, __FILE__, __LINE__); }

    #define BC_ASSERT_IMPL(MSG, COND) \
    if (bc_assert_cond(MSG, COND, __FILE__, __LINE__) != BC_TEST_SUCCESS) { return BC_TEST_FAILURE; } else { }

    #ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*BCCUNIT_H_INCL*/
