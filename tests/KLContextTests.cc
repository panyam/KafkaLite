
#include <kafkalite.h>
#include "KLContextTests.h"

void KLContextTests::test_kl_context_new()
{
	KLContext *out = kl_context_new("/tmp");
	CPPUNIT_ASSERT(out != NULL);
	CPPUNIT_ASSERT(strcmp(kl_context_basedir(out), "/tmp") == 0);
	kl_context_destroy(out);
}

void KLContextTests::test_kl_context_topic_open()
{
	KLContext *ctx= kl_context_new("/tmp");
	KLTopic *t1 = kl_context_topic_open(ctx, "topic");
	KLTopic *t2 = kl_context_topic_open(ctx, "topic");
	CPPUNIT_ASSERT(t1 == t2);
	kl_context_destroy(ctx);
}

void KLContextTests::test_kl_context_topic_close()
{
	KLContext *ctx= kl_context_new("/tmp");
	KLTopic *t1 = kl_context_topic_open(ctx, "topic");
	KLTopic *t2 = kl_context_topic_open(ctx, "topic");
	kl_context_topic_close(ctx, t1);
	CPPUNIT_ASSERT(kl_context_topic_find(ctx, "topic") == 0);
	kl_context_topic_close(ctx, t1);
	CPPUNIT_ASSERT(kl_context_topic_find(ctx, "topic") == -1);
	kl_context_destroy(ctx);
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLContextTests );

