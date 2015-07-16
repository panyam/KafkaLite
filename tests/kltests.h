
#ifndef __KL_TESTS_BASE_H__
#define __KL_TESTS_BASE_H__

#include <kafkalite.h>
#include <setjmp.h>
#include <cmocka.h>

#define TEST_DIR	"/tmp/kafka"

// Generate random messages
extern const char *SAMPLE_MESSAGE;
extern const size_t SAMPLE_MESSAGE_LEN;

extern void makeRandomMessage(char *buffer, int msgId, int nRand);
extern long long publishMessages(KLTopic *topic, int numMessages, int nRandMax, bool assertOffsets);

#endif
