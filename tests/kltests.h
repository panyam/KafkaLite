
#ifndef __KL_TESTS_BASE_H__
#define __KL_TESTS_BASE_H__

#include <kafkalite.h>
#include <setjmp.h>
#include <cmocka.h>

#define TEST_DIR	"/tmp/kafka"

// Generate random messages

extern void makeRandomMessage(char *buffer, int msgId, int nRand);
extern long long publishMessages(KLTopic *topic, int numMessages, int nRandMax, bool assertOffsets);

#endif
