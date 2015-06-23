
#ifndef __KL_CONTEXT_H__
#define __KL_CONTEXT_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Creates a new KL context starting at a particular folder where all data will
 * be stored.   This is what is shared by all consumers and producers in a
 * system.
 */
extern KLContextRef kl_context_new(const char *basedir);

/**
 * Destroys a KL context and is no longer usable by the consumers and the
 * producers.
 */
extern void kl_context_destroy(KLContextRef context);

/**
 * Gets the base folder for the KL context.
 */
extern const char *kl_context_basedir(KLContextRef context);

/**
 * Finds a topic by name and returns its index.
 */
extern int kl_context_topic_find(KLContextRef context, const char *name);

/**
 * Opens a new topic into which messages can be published.
 * If a topic is opened multiple times then the same topic
 * object is returned.
 */
extern KLTopic *kl_context_topic_open(KLContextRef context, const char *name);

/**
 * Closes a topic if it is opened.
 */
extern void kl_context_topic_close(KLContextRef context, KLTopic *topic);

/**
 * Publish a new message to a topic.
 */
extern void kl_context_publish(KLContextRef context, KLTopic *topic, void *message, size_t msgsize);

/**
 * Returns the information about a particular message at given offset for a
 * particular topic.
 */
extern void kl_context_message_info(KLContextRef context, const char *topic, size_t offset, KLMessageInfo *msginfo);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

