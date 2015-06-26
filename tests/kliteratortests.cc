

#include "KLIteratorTests.h"

const long NUM_MESSAGES = 1000;
const long NUM_RAND_MAX = 100;

void KLIteratorTests::setUp()
{
	// remove the test dir
	rmdirs(TEST_DIR);
	context = kl_context_open(TEST_DIR, NULL);
	topic = kl_topic_open(context, "topic");
	publishMessages(topic, NUM_MESSAGES, NUM_RAND_MAX, false);
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
	kl_context_close(context);
	rmdirs("/tmp/kafka2"); context = kl_context_open("/tmp/kafka2", NULL);
	topic = kl_topic_open(context, "topic");
	publishMessages(topic, NUM_MESSAGES, NUM_RAND_MAX, true);

	// buffer to store the message
	KLBuffer *msgbuffer = kl_buffer_new(512 * 512);

	char buffer[4096];
	KLIterator *iterator = kl_iterator_new(context, "topic", 0);
	CPPUNIT_ASSERT(iterator != NULL);

	for (int i = 0;i < NUM_MESSAGES;i++)
	{
		bool result = kl_iterator_forward(iterator);
		CPPUNIT_ASSERT(result);

		// get the message size
		size_t msgsize = kl_iterator_msgsize(iterator);
		kl_buffer_reset(msgbuffer);
		kl_buffer_ensure_capacity(msgbuffer, msgsize);
		KLMessage *message = (KLMessage *)kl_buffer_bytes(msgbuffer);
		kl_iterator_message(iterator, message);
		message->data[msgsize] = 0;

		makeRandomMessage(buffer, i, i % NUM_RAND_MAX);
		KLMessageHeader messageInfo = kl_iterator_metadata(iterator);
		if (strlen(buffer) != msgsize || message->size != msgsize || strncmp(buffer, message->data, msgsize) != 0)
		{
			kl_log("\nFailed at I: %d, Offset: %lld, Size: %lu", i, messageInfo.offset, messageInfo.size);
			kl_log("\nBuffer:  |%s|", buffer);
			kl_log("\nMessage: |%s|", message->data);
		}
		CPPUNIT_ASSERT(strlen(buffer) == msgsize);
		CPPUNIT_ASSERT(message->size == msgsize);
		CPPUNIT_ASSERT(strncmp(buffer, message->data, msgsize) == 0);
	}
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLIteratorTests );

