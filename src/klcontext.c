
#include "klprivate.h"

/**
 * Creates a new KL context starting at a particular folder where all data will
 * be stored.   This is what is shared by all consumers and producers in a
 * system.
 */
KLContext *kl_context_new(const char *basedir)
{
	KLContext *out = calloc(1, sizeof(KLContext));
	strcpy(out->basedir, basedir);
	out->topicIdCounter = 0;
	out->topics = kl_array_new(sizeof(KLTopic), 64);
	return out;
}

/**
 * Destroys a KL context and is no longer usable by the consumers and the
 * producers.
 */
void kl_context_destroy(KLContext *context)
{
	if (context)
		free(context);
}

/**
 * Gets the base folder for the KL context.
 */
const char *kl_context_basedir(KLContext *context)
{
	return context ? context->basedir : NULL;
}

/**
 * Finds a topic by name and returns its index.
 */
int kl_context_topic_find(KLContextRef context, const char *name)
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
 * Opens a new topic into which messages can be published.
 * If a topic is opened multiple times then the same topic
 * object is returned.
 */
KLTopic *kl_context_topic_open(KLContextRef context, const char *name)
{
	// see if this topic is already opened
	// if so then just increment its count
	KLTopic *topic = NULL;
	int index = kl_context_topic_find(context, name);
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
void kl_context_topic_close(KLContextRef context, KLTopic *topic)
{
	// see if this topic is already opened
	// if so then just increment its count
	int index = kl_context_topic_find(context, topic->name);
	if (index >= 0)
	{
		topic = kl_array_element_at(context->topics, index);
		topic->refCount--;
		if (topic->refCount == 0)
		{
			// TODO: reclaim if hits 0
			kl_array_remove_at(context->topics, index);
		}
	}
}
