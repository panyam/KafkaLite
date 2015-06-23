
#ifndef __KL_CONTEXT_TESTS_H__
#define __KL_CONTEXT_TESTS_H__

#include "KLTests.h"

class KLContextTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(KLContextTests);
	CPPUNIT_TEST(test_kl_context_new);
	CPPUNIT_TEST(test_kl_topic_open);
	CPPUNIT_TEST(test_kl_topic_close);
	CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp(void) { }
	virtual void tearDown(void) { }

	void test_kl_context_new();
	void test_kl_topic_open();
	void test_kl_topic_close();
};

#endif

