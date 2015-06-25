
#include <kafkalite.h>
#include "KLContextTests.h"

void KLContextTests::test_kl_context_open()
{
	KLContext *out = kl_context_open(TEST_DIR, NULL);
	CPPUNIT_ASSERT(out != NULL);
	CPPUNIT_ASSERT(strcmp(kl_context_basedir(out), TEST_DIR) == 0);
	kl_context_close(out);
}

void KLContextTests::test_kl_topic_open()
{
	KLContext *ctx= kl_context_open(TEST_DIR, NULL);
	KLTopic *t1 = kl_topic_open(ctx, "topic");
	KLTopic *t2 = kl_topic_open(ctx, "topic");
	CPPUNIT_ASSERT(t1 == t2);
	kl_context_close(ctx);
}

void KLContextTests::test_kl_topic_close()
{
	KLContext *ctx= kl_context_open(TEST_DIR, NULL);
	KLTopic *t1 = kl_topic_open(ctx, "topic");
	KLTopic *t2 = kl_topic_open(ctx, "topic");
	kl_topic_close(t1);
	CPPUNIT_ASSERT(kl_topic_find(ctx, "topic") == 0);
	kl_topic_close(t1);
	CPPUNIT_ASSERT(kl_topic_find(ctx, "topic") == -1);
	kl_context_close(ctx);
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLContextTests );

