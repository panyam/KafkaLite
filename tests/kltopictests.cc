

#include <kafkalite.h>
#include "kltopictests.h"
// DO NOT INCLUDE THIS OUTSIDE TESTS
#include "../src/klprivate.h"

const long NUM_MESSAGES = 1000000;

/**
 * Publish a message to a topic and see its count go up.
 */
void KLTopicTests::test_kl_topic_publish()
{
    KLTopic *topic = kl_topic_open(context, "topic2");

    kl_topic_publish(topic, "hello world", strlen("hello world"));
    CPPUNIT_ASSERT(kl_topic_message_count(topic) == 1);
    kl_topic_publish(topic, "hello world2", strlen("hello world2"));
    CPPUNIT_ASSERT(kl_topic_message_count(topic) == 2);
}

/**
 * Test publishing large number of messages with no locking.
 */
void KLTopicTests::test_kl_topic_publish_load()
{
    /*
    KLTopic *topic = kl_topic_open(context, "topic");
    long long timeTaken = publishMessages(topic, NUM_MESSAGES, 0, false);
    kl_log("\nElapsed Time for %ld messages: %lld", NUM_MESSAGES, timeTaken);
    */
}

/**
 * Test publishing large number of messages with pthread locking.
 */
void KLTopicTests::test_kl_topic_publish_load_with_locking()
{
    /*
    context = kl_context_open(TEST_DIR, kl_pthread_lock_manager());
    KLTopic *topic = kl_topic_open(context, "topic");
    long long timeTaken = publishMessages(topic, NUM_MESSAGES, 0, false);
    kl_log("\nElapsed Time for %ld messages: %lld", NUM_MESSAGES, timeTaken);
    */
}

/**
 * Test that a publish persists data on a restart.
 */
void KLTopicTests::test_kl_topic_restart()
{
    KLTopic *topic = kl_topic_open(context, "topic2");
    kl_topic_publish(topic, "hello world", strlen("hello world"));
    kl_topic_publish(topic, "hello world2", strlen("hello world2"));
    CPPUNIT_ASSERT(kl_topic_message_count(topic) == 2);

    // now close the context and open it again
    kl_context_close(context);

    context = kl_context_open(TEST_DIR, NULL);
    topic = kl_topic_open(context, "topic2");
    CPPUNIT_ASSERT(kl_topic_message_count(topic) == 2);
}

/**
 * Test getting of message metadata.
 *
 * Test getting message metadata when they are split between disk and in memory
 * buffer.
 */
void KLTopicTests::test_kl_topic_get_message_metadata()
{
    // write enough messages that they are 
    const int msgsize = 40;
    const char *message = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456 - %03d";
    char msgbuff[50];

    context = kl_context_open(TEST_DIR, NULL);
    KLTopic *topic = kl_topic_open(context, "topic");
    topic->flushThreshold = 4 * (sizeof(KLMessageHeader) + msgsize);
    kl_buffer_destroy(topic->dataBuffer);
    kl_buffer_destroy(topic->indexBuffer);
    topic->dataBuffer = kl_buffer_new(4 * (sizeof(KLMessageHeader) + msgsize));
    topic->indexBuffer = kl_buffer_new(4 * (sizeof(KLMessageMetadata)));

    for (int i = 0;i < 7;i++) 
    {
        sprintf(msgbuff, "%s%d", message, i);
        int len = strlen(msgbuff);
        CPPUNIT_ASSERT(len == msgsize);
        kl_topic_publish(topic, msgbuff, len);
    }

    // now 4 messages should be on disk and 3 on the buffer
    CPPUNIT_ASSERT(topic->flushedAtIndex == 4);
    CPPUNIT_ASSERT(topic->flushedAtOffset == 4 * (sizeof(KLMessageHeader) + msgsize));
    CPPUNIT_ASSERT(topic->currOffset == 7 * (sizeof(KLMessageHeader) + msgsize));
    CPPUNIT_ASSERT(topic->currIndex == 7);
    CPPUNIT_ASSERT(kl_buffer_size(topic->dataBuffer) == 3 * (sizeof(KLMessageHeader) + msgsize));

    // Now start reading
    // read 1 - everything is on disk
	KLMessageMetadata metadatas[16];
	size_t nRead = kl_topic_get_message_metadata(topic, 1, metadatas, 3);
	CPPUNIT_ASSERT(nRead == 3);

	nRead = kl_topic_get_message_metadata(topic, 5, metadatas, 2);
	CPPUNIT_ASSERT(nRead == 2);

	nRead = kl_topic_get_message_metadata(topic, 0, metadatas, 10);
	CPPUNIT_ASSERT(nRead == 7);

    kl_topic_close(topic);
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLTopicTests );

