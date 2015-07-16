

#include <kafkalite.h>
#include "kltopictests.h"
// DO NOT INCLUDE THIS OUTSIDE TESTS
#include "../src/klprivate.h"

static const long NUM_MESSAGES = 1000000;

typedef struct KLTopicTests {
	KLContext *context;
} KLTopicTests;

int kl_topic_tests_setup(void **state) {
	// remove the test dir
	kl_rmdirs(TEST_DIR);
	KLTopicTests *tc = calloc(1, sizeof(KLTopicTests));
	tc->context = kl_context_open(TEST_DIR, NULL);
	*state = tc;
	return 0;
}

int kl_topic_tests_teardown(void **state) { 
	KLTopicTests *tc = *state;
	kl_context_close(tc->context);
	free(tc);
	*state = NULL;
	return 0;
}

/**
 * Publish a message to a topic and see its count go up.
 */
void test_kl_topic_publish(void **state)
{
	KLTopicTests *tc = *state;
    KLTopic *topic = kl_topic_open(tc->context, "topic2");

    kl_topic_publish(topic, "hello world", strlen("hello world"));
    assert_true(kl_topic_message_count(topic) == 1);
    kl_topic_publish(topic, "hello world2", strlen("hello world2"));
    assert_true(kl_topic_message_count(topic) == 2);
}

/**
 * Test publishing large number of messages with no locking.
 */
void test_kl_topic_publish_load(void **state)
{
	KLTopicTests *tc = *state;
    KLTopic *topic = kl_topic_open(tc->context, "topic");
    long long timeTaken = publishMessages(topic, NUM_MESSAGES, 0, false);
    kl_log("\nElapsed Time for %ld messages: %lld", NUM_MESSAGES, timeTaken);
}

/**
 * Test publishing large number of messages with pthread locking.
 */
void test_kl_topic_publish_load_with_locking(void **state)
{
	KLTopicTests *tc = *state;
    tc->context = kl_context_open(TEST_DIR, kl_pthread_lock_manager());
    KLTopic *topic = kl_topic_open(tc->context, "topic");
    long long timeTaken = publishMessages(topic, NUM_MESSAGES, 0, false);
    kl_log("\nElapsed Time for %ld messages: %lld", NUM_MESSAGES, timeTaken);
}

/**
 * Test that a publish persists data on a restart.
 */
void test_kl_topic_restart(void **state)
{
	KLTopicTests *tc = *state;
    KLTopic *topic = kl_topic_open(tc->context, "topic2");
    kl_topic_publish(topic, "hello world", strlen("hello world"));
    kl_topic_publish(topic, "hello world2", strlen("hello world2"));
    assert_true(kl_topic_message_count(topic) == 2);

    // now close the context and open it again
    kl_context_close(tc->context);

    tc->context = kl_context_open(TEST_DIR, NULL);
    topic = kl_topic_open(tc->context, "topic2");
    assert_true(kl_topic_message_count(topic) == 2);
}

/**
 * Test getting of message metadata.
 *
 * Test getting message metadata when they are split between disk and in memory
 * buffer.
 */
void test_kl_topic_get_message_metadata(void **state)
{
    // write enough messages that they are 
    const int msgsize = 40;
    const char *message = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456 - %03d";
    char msgbuff[50];

	KLTopicTests *tc = *state;
    tc->context = kl_context_open(TEST_DIR, NULL);
    KLTopic *topic = kl_topic_open(tc->context, "topic");
    topic->flushThreshold = 4 * (sizeof(KLMessageHeader) + msgsize);
    kl_buffer_destroy(topic->dataBuffer);
    kl_buffer_destroy(topic->indexBuffer);
    topic->dataBuffer = kl_buffer_new(4 * (sizeof(KLMessageHeader) + msgsize));
    topic->indexBuffer = kl_buffer_new(4 * (sizeof(KLMessageMetadata)));

    for (int i = 0;i < 7;i++) 
    {
        sprintf(msgbuff, "%s%d", message, i);
        int len = strlen(msgbuff);
        assert_true(len == msgsize);
        kl_topic_publish(topic, msgbuff, len);
    }

    // now 4 messages should be on disk and 3 on the buffer
    assert_true(topic->flushedAtIndex == 4);
    assert_true(topic->flushedAtOffset == 4 * (sizeof(KLMessageHeader) + msgsize));
    assert_true(topic->currOffset == 7 * (sizeof(KLMessageHeader) + msgsize));
    assert_true(topic->currIndex == 7);
    assert_true(kl_buffer_size(topic->dataBuffer) == 3 * (sizeof(KLMessageHeader) + msgsize));

    // Now start reading
    // read 1 - everything is on disk
	KLMessageMetadata metadatas[16];
	size_t nRead = kl_topic_get_message_metadata(topic, 1, metadatas, 3);
	assert_true(nRead == 3);

	nRead = kl_topic_get_message_metadata(topic, 5, metadatas, 2);
	assert_true(nRead == 2);

	nRead = kl_topic_get_message_metadata(topic, 0, metadatas, 10);
	assert_true(nRead == 7);

    kl_topic_close(topic);
}

int run_topic_tests()
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_kl_topic_publish, kl_topic_tests_setup, kl_topic_tests_teardown),
		// cmocka_unit_test_setup_teardown(test_kl_topic_publish_load, kl_topic_tests_setup, kl_topic_tests_teardown),
		// cmocka_unit_test_setup_teardown(test_kl_topic_publish_load_with_locking, kl_topic_tests_setup, kl_topic_tests_teardown),
		cmocka_unit_test_setup_teardown(test_kl_topic_restart, kl_topic_tests_setup, kl_topic_tests_teardown),
		cmocka_unit_test_setup_teardown(test_kl_topic_get_message_metadata, kl_topic_tests_setup, kl_topic_tests_teardown),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
