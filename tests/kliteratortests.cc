

#include "kliteratortests.h"

const long NUM_MESSAGES = 1000;
const long NUM_RAND_MAX = 100;

void KLIteratorTests::setUp()
{
	// remove the test dir
	kl_rmdirs(TEST_DIR);
	context = kl_context_open(TEST_DIR, NULL);
	topic = kl_topic_open(context, "topic");
	publishMessages(topic, NUM_MESSAGES, NUM_RAND_MAX, false);
}

void KLIteratorTests::tearDown(void)
{
	kl_topic_close(topic); topic = NULL;
	kl_context_close(context); context = NULL;
}

/**
 * Create an iterator.
 */
void KLIteratorTests::test_kl_iterator_new()
{
	KLIterator *iterator = kl_iterator_new(context, "topic", 0);
	CPPUNIT_ASSERT(iterator != NULL);
	kl_iterator_destroy(iterator);
}

/**
 * Create an iterator and consume messages.
 */
void KLIteratorTests::test_kl_iterator_consume()
{
	char buffer[4096];
	KLIterator *iterator = kl_iterator_new(context, "topic", 0);
	CPPUNIT_ASSERT(iterator != NULL);

	for (int i = 0;i < NUM_MESSAGES;i++)
	{
		bool result = kl_iterator_forward(iterator);
		CPPUNIT_ASSERT(result);

		// get the message size
		uint64_t msgsize = kl_iterator_msgsize(iterator);
		KLMessage *message = (KLMessage *)malloc(sizeof(KLMessage) + msgsize + 1);
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
		CPPUNIT_ASSERT(strlen(buffer) == msgsize);
		CPPUNIT_ASSERT(message->header.size == msgsize);
		CPPUNIT_ASSERT(strncmp(buffer, message->data, msgsize) == 0);
		free(message);
	}
	kl_iterator_destroy(iterator);
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLIteratorTests );

