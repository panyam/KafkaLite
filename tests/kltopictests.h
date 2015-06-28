
#ifndef __KL_TOPIC_TESTS_H__
#define __KL_TOPIC_TESTS_H__

#include "kltests.h"

class KLTopicTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(KLTopicTests);
	CPPUNIT_TEST(test_kl_topic_publish);
	CPPUNIT_TEST(test_kl_topic_publish_load);
	CPPUNIT_TEST(test_kl_topic_publish_load_with_locking);
	CPPUNIT_TEST(test_kl_topic_restart);
	CPPUNIT_TEST_SUITE_END();
public:
	KLContext *context;

public:
	virtual void setUp(void) {
		// remove the test dir
		kl_rmdirs(TEST_DIR);
		context = kl_context_open(TEST_DIR, NULL);
	}
	virtual void tearDown(void) { 
		kl_context_close(context);
	}

	void test_kl_topic_publish();
	void test_kl_topic_publish_load();
	void test_kl_topic_publish_load_with_locking();
	void test_kl_topic_restart();
};

#endif

