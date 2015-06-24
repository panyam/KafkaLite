

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

/**
 * Test that a publish persists data on a restart.
 */
void KLTopicTests::test_kl_topic_restart()
{
	KLContext *ctx = kl_context_open(TEST_DIR, NULL);
	KLTopic *topic = kl_topic_open(ctx, "topic");
	kl_topic_publish(topic, "hello world", strlen("hello world"));
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == 1);

	// now close the context and open it again
	kl_context_close(ctx);

	ctx = kl_context_open(TEST_DIR, NULL);
	topic = kl_topic_open(ctx, "topic");
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == 1);
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLTopicTests );

