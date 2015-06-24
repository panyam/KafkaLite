

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
	size_t size;
	unsigned long long timestamp;
};


/**
 * Opens a new topic into which messages can be published.  If a topic is opened 
 * multiple times then the same topic object is returned.  A topic can be assigned 
 * to a particular group via the group parameter.  If this is NULL then it is 
 * assigned to the default group.
 *
 * Topic names must be globally unique and the group only serves as a way to
 * limit and control the level of concurrent writes to a particular group.
 */
extern KLTopic *kl_topic_open(KLContextRef context, const char *name);

/**
 * Closes a topic if it is opened.
 */
extern bool kl_topic_close(KLTopic *topic);

/**
 * Finds a topic by name and returns its index.
 */
extern int kl_topic_find(KLContextRef context, const char *name);

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

extern void kl_topic_initialize(KLContext *context, KLTopic *topic, const char *name);
extern void kl_topic_finalize(KLTopic *topic);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

