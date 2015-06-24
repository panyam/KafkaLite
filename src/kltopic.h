

#ifndef __KL_TOPIC_H__
#define __KL_TOPIC_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

struct KLMessageInfo
{
	unsigned index;
	unsigned offset;
	size_t msgsize;
	unsigned timestamp;
};


/**
 * Finds a topic by name and returns its index.
 */
extern int kl_topic_find(KLContextRef context, const char *name);

/**
 * Opens a new topic into which messages can be published.
 * If a topic is opened multiple times then the same topic
 * object is returned.
 */
extern KLTopic *kl_topic_open(KLContextRef context, const char *name);

/**
 * Closes a topic if it is opened.
 */
extern void kl_topic_close(KLTopic *topic);

/**
 * Publish a new message to a topic.
 */
extern void kl_topic_publish(KLTopic *topic, const char *message, size_t msgsize);

/**
 * Returns the information about a particular message at given offset for a
 * particular topic.
 */
extern void kl_topic_message_info(KLTopic *topic, size_t offset, KLMessageInfo *msginfo);

/**
 * Return the number of messages in the topic.
 */
extern int kl_topic_message_count(KLTopic *topic);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

