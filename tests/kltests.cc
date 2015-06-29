
#include "kltests.h"

const char *SAMPLE_MESSAGE = "Hello World 1234567890";
const UInt64 SAMPLE_MESSAGE_LEN = strlen(SAMPLE_MESSAGE);

void makeRandomMessage(char *buffer, int msgId, int nRand)
{
	char buffer2[32];
	sprintf(buffer, "[%06d-%03d]: Hello World.  Random numbers: ", msgId, nRand);
	for (int j = 0;j < nRand;j++)
	{
		sprintf(buffer2, "%02d, ", j);
		strcat(buffer, buffer2);
	}
}

long long publishMessages(KLTopic *topic, int numMessages, int nRandMax, bool assertOffsets)
{
	// long long beforeTime = kl_current_timestamp();
	UInt64 offset = 0;
	UInt64 totalSize = 0;
	UInt64 msgsize = SAMPLE_MESSAGE_LEN;
	const char *message = SAMPLE_MESSAGE;
	char buffer[4096];
	long long elapsedTime = 0;
	for (int i = 0;i < numMessages;i++)
	{
		if (nRandMax > 0)
		{
			int nRand = i % nRandMax;
			makeRandomMessage(buffer, i, nRand);
			msgsize = strlen(buffer);
			message = buffer;
		}
		long long beforeTime = kl_current_timestamp();
		offset = kl_topic_publish(topic, message, msgsize);
		long long afterTime = kl_current_timestamp();
		elapsedTime += (afterTime - beforeTime);
		totalSize += msgsize;
		if (assertOffsets)
		{
			CPPUNIT_ASSERT(offset == (totalSize + (sizeof(UInt64) * (i + 1))));
		}
	}
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == numMessages);
	CPPUNIT_ASSERT(totalSize + (sizeof(UInt64) * numMessages) == offset);
	return elapsedTime;
}
