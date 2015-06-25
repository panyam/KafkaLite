

#include "KLIteratorTests.h"

// Generate random messages
const long NUM_MESSAGES = 1000;
const char *SAMPLE_MESSAGE = "Hello World 1234567890";
size_t SAMPLE_MESSAGE_LEN = strlen(SAMPLE_MESSAGE);

void makeRandomMessage(char *buffer, int msgId, int nRand)
{
	char buffer2[32];
	sprintf(buffer, "[%06d-%03d]: Hello World.  Random numbers: ", msgId, nRand);
	for (int j = 0;j < nRand;j++)
	{
		sprintf(buffer2, "%02d, ", j);
		strcat(buffer, buffer2);
	}
}

void KLIteratorTests::setUp()
{
	// remove the test dir
	rmdirs(TEST_DIR);
	context = kl_context_open(TEST_DIR, NULL);
	topic = kl_topic_open(context, "topic");

	// long long beforeTime = current_timestamp();
	char buffer[1024];
	for (int i = 0;i < NUM_MESSAGES;i++)
	{
		int nRand = rand() % 100;
		makeRandomMessage(buffer, i, nRand);
		kl_topic_publish(topic, buffer, strlen(buffer));
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

/**
 * Create an iterator and consume messages.
 */
void KLIteratorTests::test_kl_iterator_consume()
{
	KLIterator *iterator = kl_iterator_new(context, "topic", 0);
	for (int i = 0;i < 5;i++)
	{
	}
	CPPUNIT_ASSERT(iterator != NULL);
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLIteratorTests );

