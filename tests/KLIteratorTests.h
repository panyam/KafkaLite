
#ifndef __KL_TOPIC_TESTS_H__
#define __KL_TOPIC_TESTS_H__

#include "KLTests.h"

class KLIteratorTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(KLIteratorTests);
	CPPUNIT_TEST(test_kl_iterator_new);
	CPPUNIT_TEST_SUITE_END();
public:
	KLContext *context;
	KLTopic *topic;

public:
	virtual void setUp(void);
	virtual void tearDown(void) { kl_context_close(context); }

	void test_kl_iterator_new();
};

#endif

