
#include "KLTests.h"

const char *SAMPLE_MESSAGE = "Hello World 1234567890";
const size_t SAMPLE_MESSAGE_LEN = strlen(SAMPLE_MESSAGE);

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
	// long long beforeTime = current_timestamp();
	size_t offset = 0;
	size_t totalSize = 0;
	size_t msgsize = SAMPLE_MESSAGE_LEN;
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
		long long beforeTime = current_timestamp();
		offset = kl_topic_publish(topic, message, msgsize);
		long long afterTime = current_timestamp();
		elapsedTime += (afterTime - beforeTime);
		totalSize += msgsize;
		if (assertOffsets)
		{
			CPPUNIT_ASSERT(offset == (totalSize + (sizeof(size_t) * (i + 1))));
		}
	}
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == numMessages);
	CPPUNIT_ASSERT(totalSize + (sizeof(size_t) * numMessages) == offset);
	return elapsedTime;
}
