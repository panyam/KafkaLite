
#ifndef __KL_TOPIC_TESTS_H__
#define __KL_TOPIC_TESTS_H__

#include "kltests.h"

extern int run_topic_tests();
extern void test_kl_topic_publish(void **state);
extern void test_kl_topic_publish_load(void **state);
extern void test_kl_topic_publish_load_with_locking(void **state);
extern void test_kl_topic_get_message_metadata(void **state);
extern void test_kl_topic_restart(void **state);

#endif

