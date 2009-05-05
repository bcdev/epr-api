#include "bccunit.h"


BC_BEGIN_TEST(test_func_1)
	BC_ASSERT_TRUE(1 == 1);
BC_END_TEST()


BC_BEGIN_TEST(test_func_2)
	BC_ASSERT_NOT_NULL((void*)1);
BC_END_TEST()


int get_3()
{
	return 2;
}


BC_BEGIN_TEST(test_func_3)
	int x = 3;
	BC_ASSERT_NULL(NULL);
	//BC_ASSERT_NULL((void*) 4);
	BC_ASSERT_SAME(3, get_3());
BC_END_TEST()


BC_BEGIN_TEST(test_func_4)
	int x = 3;
	BC_ASSERT_NULL(NULL);
	//BC_ASSERT_NULL((void*) 4);
	BC_ASSERT_SAME(3, get_3());
BC_END_TEST()


int main(int argc, char** argv)
{
	BcTestSuite tsuite1;
	BcTestSuite tsuite2;
	BcTestResult result;

	tsuite1 = bc_create_test_suite("test_suite_1");

	bc_add_test_case(tsuite1, "test_case_1", test_func_1);
	bc_add_test_case(tsuite1, "test_case_2", test_func_2);
	bc_add_test_case(tsuite1, "test_case_3", test_func_3);

	tsuite2 = bc_create_test_suite("test_suite_2");
	bc_add_test_case(tsuite2, "test_case_4", test_func_4);
	bc_add_test(tsuite1, tsuite2);

	bc_run_test(tsuite1, &result);

	return 0;
}


