

#include <kafkalite.h>
#include "KLTopicTests.h"

/**
 * Publish a message to a topic and see its count go up.
 */
void KLTopicTests::test_kl_topic_publish()
{
	KLTopic *topic = kl_topic_open(context, "topic");

	kl_topic_publish(topic, "hello world", strlen("hello world"));
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == 1);
	kl_topic_publish(topic, "hello world2", strlen("hello world2"));
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == 2);
}

/**
 * Test that a publish persists data on a restart.
 */
void KLTopicTests::test_kl_topic_restart()
{
	KLContext *context = kl_context_open(TEST_DIR, NULL);
	KLTopic *topic = kl_topic_open(context, "topic");
	kl_topic_publish(topic, "hello world", strlen("hello world"));
	kl_topic_publish(topic, "hello world2", strlen("hello world2"));
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == 2);

	// now close the context and open it again
	kl_context_close(context);

	context = kl_context_open(TEST_DIR, NULL);
	topic = kl_topic_open(context, "topic");
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == 2);
}

/**
 * Test publishing large number of messages and time it 
 * with and without mutex manager
 */
void KLTopicTests::test_kl_topic_publish_load()
{
	KLContext *context = kl_context_open(TEST_DIR, NULL);
	KLTopic *topic = kl_topic_open(context, "topic");
	const char *buffer = "Hello World 1234567890";
	size_t buffsize = strlen(buffer);
	long long beforeTime = current_timestamp();
	long NUM_MESSAGES = 1000000;
	for (int i = 0;i < NUM_MESSAGES;i++)
	{
		if ((i % 100000) == 0)
		{
			kl_log("Done: %d\n", i);
		}
		kl_topic_publish(topic, buffer, buffsize);
	}
	long long afterTime = current_timestamp();
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == NUM_MESSAGES);
	kl_log("Elapsed Time for %ld messages: %lld", NUM_MESSAGES, afterTime - beforeTime);

	// now close the context and open it again
	kl_context_close(context);

	context = kl_context_open(TEST_DIR, NULL);
	topic = kl_topic_open(context, "topic");
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == 2);
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLTopicTests );

