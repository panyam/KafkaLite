

#ifndef __KL_TOPIC_H__
#define __KL_TOPIC_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

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
extern size_t kl_topic_publish(KLTopic *topic, const char *message, size_t msgsize);

/**
 * Publish a batch of messages to the topic.
 */
extern size_t kl_topic_publish_multi(KLTopic *topic, int numMessages, const char *msgs[], size_t *msgsizes);

/**
 * Get the info about count number of messages starting from a particular message index.
 * The output buffer "out" must point to a buffer that has enough space for
 * outCount KLMessageHeader objects.
 */
extern int kl_topic_get_message_info(KLTopic *topic, int index, KLMessageHeader *out, int outCount);

/**
 * Get a list of messages beginning at a particular index.
 * Issues to consider:
 * If an index is specified then get_message_info nees to be called again to 
 * get the offset and size. However, this is a duplicate call as most likely 
 * the offset and size were fetched by a previous call to get_message_info.  
 * To prevent this an offset could be pass the offset but there is no 
 * way to validate this.  
 * Instead the message info (which is an opaque pointer) is passed which will
 * indicate where to read from.
 *
 * May be make this a private API - so only consumers have access to this.
 */
extern int kl_topic_get_messages(KLTopic *topic, KLMessageHeader *const headers, int numMessages, KLMessage *outMessages);

extern void kl_topic_initialize(KLContext *context, KLTopic *topic, const char *name);
extern void kl_topic_finalize(KLTopic *topic);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

