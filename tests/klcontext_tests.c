
#include <check.h>
#include <kafkalite.h>

START_TEST(test_kl_context_new)
{
	KLContext *out = kl_context_new("/tmp");
	ck_assert(out != NULL);
	ck_assert(strcmp(kl_context_basedir(out), "/tmp") == 0);
	kl_context_destroy(out);
}
END_TEST

Suite *klcontext_testsuite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("KLContext");
	tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_kl_context_new);
	suite_add_tcase(s, tc_core);
	return s;
}
