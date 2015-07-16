
#include "klcontexttests.h"

void test_kl_context_open(void **state)
{
	KLContext *out = kl_context_open(TEST_DIR, NULL);
	assert_true(out != NULL);
	assert_int_equal(strcmp(kl_context_basedir(out), TEST_DIR), 0);
	kl_context_close(out);
}

void test_kl_topic_open(void **state)
{
	KLContext *ctx= kl_context_open(TEST_DIR, NULL);
	KLTopic *t1 = kl_topic_open(ctx, "topic");
	KLTopic *t2 = kl_topic_open(ctx, "topic");
	assert_true(t1 == t2);
	kl_context_close(ctx);
}

void test_kl_topic_close(void **state)
{
	KLContext *ctx= kl_context_open(TEST_DIR, NULL);
	KLTopic *t1 = kl_topic_open(ctx, "topic");
	KLTopic *t2 = kl_topic_open(ctx, "topic");
	kl_topic_close(t1);
	assert_true(kl_topic_find(ctx, "topic") == 0);
	kl_topic_close(t1);
	assert_true(kl_topic_find(ctx, "topic") == -1);
	kl_context_close(ctx);
}

int run_context_tests()
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_kl_context_open),
		cmocka_unit_test(test_kl_topic_open),
		cmocka_unit_test(test_kl_topic_close),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
