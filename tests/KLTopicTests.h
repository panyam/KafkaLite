
#ifndef __KL_TOPIC_TESTS_H__
#define __KL_TOPIC_TESTS_H__

#include "KLTests.h"

class KLTopicTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(KLTopicTests);
	CPPUNIT_TEST(test_kl_topic_publish);
	CPPUNIT_TEST(test_kl_topic_consume);
	CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp(void) { }
	virtual void tearDown(void) { }

	void test_kl_topic_publish();
	void test_kl_topic_consume();
};

#endif

