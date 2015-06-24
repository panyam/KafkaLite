
#include "klprivate.h"

int kl_topic_find_unlocked(KLContextRef context, const char *name)
{
	for (int i = 0, count = kl_array_count(context->topics);i < count;i++)
	{
		KLTopic *topic = (KLTopic *)kl_array_element_at(context->topics, i);
		if (strcmp(topic->name, name) == 0)
		{
			return i;
		}
	}
	return -1;
}

/**
 * Finds a topic by name and returns its index.
 */
int kl_topic_find(KLContextRef context, const char *name)
{
	if (context->mutexFactory)
		context->mutexFactory->MutexLock(context->topicListMutex);
	int index = kl_topic_find_unlocked(context, name);
	if (context->mutexFactory)
		context->mutexFactory->MutexUnlock(context->topicListMutex);
	return index;
}

/**
 * Opens a new topic into which messages can be published.
 * If a topic is opened multiple times then the same topic
 * object is returned.
 */
KLTopic *kl_topic_open(KLContextRef context, const char *name)
{
	// see if this topic is already opened
	// if so then just increment its count
	KLTopic *topic = NULL;
	int index = kl_topic_find_unlocked(context, name);
	if (index < 0)
	{
		topic = kl_array_insert_at(context->topics, -1);
		topic->name = strdup(name);
		topic->refCount = 0;
	} else {
		topic = kl_array_element_at(context->topics, index);
	}
	topic->refCount++;
	topic->context = context;
	return topic;
}

/**
 * Closes a topic if it is opened.
 */
void kl_topic_close(KLTopic *topic)
{
	if (!topic)
		return ;

	KLContextRef context = topic->context;
	// see if this topic is already opened
	// if so then just increment its count
	int index = kl_topic_find_unlocked(context, topic->name);
	if (index >= 0)
	{
		topic = kl_array_element_at(context->topics, index);
		topic->refCount--;
		if (topic->refCount == 0)
		{
			kl_array_remove_at(context->topics, index);
		}
	}
}

void kl_topic_publish(KLTopic *topic, const char *msg, size_t msgsize)
{
	if (topic)
	{
	}
}

int kl_topic_message_count(KLTopic *topic)
{
	if (!topic)
		return 0;
	return 0;
}

