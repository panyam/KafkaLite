

#include <kafkalite.h>
#include "KLTopicTests.h"

/**
 * Publish a message to a topic and see its count go up.
 */
void KLTopicTests::test_kl_topic_publish()
{
	KLContext *ctx= kl_context_open(TEST_DIR, NULL);
	KLTopic *topic = kl_topic_open(ctx, "topic");

	kl_topic_publish(topic, "hello world", strlen("hello world"));
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == 1);
	kl_topic_publish(topic, "hello world2", strlen("hello world2"));
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == 2);
	kl_context_close(ctx);
}

void KLTopicTests::test_kl_topic_consume()
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

CPPUNIT_TEST_SUITE_REGISTRATION( KLTopicTests );

