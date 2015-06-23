
#ifndef __KL_CONTEXT_TESTS_H__
#define __KL_CONTEXT_TESTS_H__

#include "kltests_base.h"

class KLContextTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(KLContextTests);
	CPPUNIT_TEST(test_kl_context_new);
	CPPUNIT_TEST(test_kl_context_topic_open);
	CPPUNIT_TEST_SUITE_END();

public:
	KLContextTests();
	virtual void setUp(void) { }
	virtual void tearDown(void) { }

public:
	void test_kl_context_new();
	void test_kl_context_topic_open();
};

#endif

