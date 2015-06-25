

#include "KLIteratorTests.h"

// Generate random messages
const long NUM_MESSAGES = 1000;
const char *SAMPLE_MESSAGE = "Hello World 1234567890";
size_t SAMPLE_MESSAGE_LEN = strlen(SAMPLE_MESSAGE);

void KLIteratorTests::setUp()
{
	// remove the test dir
	rmdirs(TEST_DIR);
	context = kl_context_open(TEST_DIR, NULL);
	topic = kl_topic_open(context, "topic");

	// long long beforeTime = current_timestamp();
	for (int i = 0;i < NUM_MESSAGES;i++)
	{
		kl_topic_publish(topic, SAMPLE_MESSAGE, SAMPLE_MESSAGE_LEN);
	}
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == NUM_MESSAGES);
}

/**
 * Create an iterator.
 */
void KLIteratorTests::test_kl_iterator_new()
{
	KLIterator *iterator = kl_iterator_new(context, "topic", 0);
	CPPUNIT_ASSERT(iterator != NULL);
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLIteratorTests );

