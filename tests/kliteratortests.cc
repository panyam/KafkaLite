

#include "KLIteratorTests.h"

const long NUM_MESSAGES = 1000;

void KLIteratorTests::setUp()
{
	// remove the test dir
	rmdirs(TEST_DIR);
	context = kl_context_open(TEST_DIR, NULL);
	topic = kl_topic_open(context, "topic");
	publishMessages(topic, NUM_MESSAGES, true);
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
	// buffer to store the message
	KLBuffer *msgbuffer = kl_buffer_new(512 * 512);

	char buffer[1024];
	KLIterator *iterator = kl_iterator_new(context, "topic", 0);
	CPPUNIT_ASSERT(iterator != NULL);

	for (int i = 0;i < 5;i++)
	{
		bool result = kl_iterator_forward(iterator, false);
		CPPUNIT_ASSERT(result);

		// get the message size
		size_t msgsize = kl_iterator_msgsize(iterator);
		kl_buffer_reset(msgbuffer);
		kl_buffer_ensure_capacity(msgbuffer, msgsize);
		kl_iterator_message(iterator, kl_buffer_bytes(msgbuffer));
	}
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLIteratorTests );

