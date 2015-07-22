

#include "kliteratortests.h"

static const long NUM_MESSAGES = 1000;
static const long NUM_RAND_MAX = 100;

typedef struct KLIteratorTests
{
	KLContext *context;
	KLTopic *topic;
} KLIteratorTests;

int kl_iterator_tests_setup(void **state)
{
	// remove the test dir
	kl_rmdirs(TEST_DIR);
	KLIteratorTests *tc = calloc(1, sizeof(KLIteratorTests));
	tc->context = kl_context_open(TEST_DIR, NULL);
	tc->topic = kl_topic_open(tc->context, "topic");
	*state = tc;
	publishMessages(tc->topic, NUM_MESSAGES, NUM_RAND_MAX, false);
	return 0;
}

int kl_iterator_tests_teardown(void **state)
{
	KLIteratorTests *tc = *state;
	kl_topic_close(tc->topic);
	kl_context_close(tc->context);
	free(tc);
	*state = NULL;
	return 0;
}

/**
 * Create an iterator.
 */
void test_kl_iterator_new(void **state)
{
	KLIteratorTests *tc = *state;
	KLIterator *iterator = kl_iterator_new(tc->context, "topic", 0);
	assert_true(iterator != NULL);
	kl_iterator_destroy(iterator);
}

/**
 * Create an iterator and consume messages.
 */
void test_kl_iterator_consume(void **state)
{
	char buffer[4096];
	KLIteratorTests *tc = *state;
	KLIterator *iterator = kl_iterator_new(tc->context, "topic", 0);
	assert_true(iterator != NULL);

	for (int i = 0;i < NUM_MESSAGES;i++)
	{
		bool result = kl_iterator_forward(iterator);
		assert_true(result);

		// get the message size
		uint64_t msgsize = kl_iterator_msgsize(iterator);
		KLMessage *message = (KLMessage *)calloc(1, sizeof(KLMessage) + msgsize + 1);
		kl_iterator_message(iterator, message);
		message->data[msgsize] = 0;
		makeRandomMessage(buffer, i, i % NUM_RAND_MAX);
		KLMessageMetadata *const messageInfo = kl_iterator_metadata(iterator);
		if (strlen(buffer) != msgsize || message->header.size != msgsize || strncmp(buffer, message->data, msgsize) != 0)
		{
			kl_log("\nFailed at I: %d, Offset: %ld, Size: %lu", i, messageInfo->offset, messageInfo->size);
			kl_log("\nBuffer:  |%s|", buffer);
			kl_log("\nMessage: |%s|", message->data);
		}
		assert_true(strlen(buffer) == msgsize);
		assert_true(message->header.size == msgsize);
		assert_true(strncmp(buffer, message->data, msgsize) == 0);
		free(message);
	}
	kl_iterator_destroy(iterator);
}

int run_iterator_tests()
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_kl_iterator_new, kl_iterator_tests_setup, kl_iterator_tests_teardown),
		cmocka_unit_test_setup_teardown(test_kl_iterator_consume, kl_iterator_tests_setup, kl_iterator_tests_teardown),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
