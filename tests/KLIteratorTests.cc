

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
	KLIterator *iterator = kl_iterator_new(context, "topic", 0);
	for (int i = 0;i < 5;i++)
	{
	}
	CPPUNIT_ASSERT(iterator != NULL);
}

CPPUNIT_TEST_SUITE_REGISTRATION( KLIteratorTests );

