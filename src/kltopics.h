

#ifndef __KL_TOPIC_H__
#define __KL_TOPIC_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Info about a message.
 */
struct KLMessageInfo
{
	unsigned offset;
	unsigned size;
	unsigned timestamp;
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
 * Return the number of messages in the topic.
 */
extern int kl_topic_message_count(KLTopic *topic);

/**
 * Publish a single message to the topic.
 */
extern void kl_topic_publish(KLTopic *topic, const char *message, size_t msgsize);

/**
 * Publish a batch of messages to the topic.
 */
extern void kl_topic_publish_multi(KLTopic *topic, int numMessages, const char *msgs[], size_t *msgsizes);

/**
 * Get the info about count number of messages starting from a particular message index.
 * The output buffer "out" must point to a buffer that has enough space for
 * outCount KLMessageInfo objects.
 */
extern int kl_topic_get_message_info(KLTopic *topic, int index, KLMessageInfo *out, int outCount);

/**
 * Get a bunch of messages beginning at a particular index.
 */
extern int kl_topic_get_messages(KLTopic *topic, int index, char *output, int outCount);

extern void kl_topic_initialize(KLContext *context, KLTopic *topic, const char *name);
extern void kl_topic_finalize(KLTopic *topic);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

