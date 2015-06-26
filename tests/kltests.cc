
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

long long publishMessages(KLTopic *topic, int numMessages, int nRandMax)
{
	long long beforeTime = current_timestamp();
	// long long beforeTime = current_timestamp();
	if (nRandMax > 0)
	{
		char buffer[1024];
		for (int i = 0;i < numMessages;i++)
		{
			int nRand = i % nRandMax;
			makeRandomMessage(buffer, i, nRand);
			kl_topic_publish(topic, buffer, strlen(buffer));
		}
	} else {
		for (int i = 0;i < numMessages;i++)
		{
			kl_topic_publish(topic, SAMPLE_MESSAGE, SAMPLE_MESSAGE_LEN);
		}
	}
	long long afterTime = current_timestamp();
	CPPUNIT_ASSERT(kl_topic_message_count(topic) == numMessages);
	return afterTime - beforeTime;
}
