
#ifndef __KL_PRIVATE_H__
#define __KL_PRIVATE_H__

#include "kafkalite.h"

struct KLContext
{
	/**
	 * Base dir where all messages are written to.
	 */
	char basedir[PATH_MAX];

	/**
	 * List of topic IDs to maintain as topics are created.
	 */
	unsigned long long topicIdCounter;

	/**
	 * List of all open topics.
	 */
	KLArray *topics;
};

struct KLTopic
{
	/**
	 * Context to which this topic belongs.
	 */
	KLContext *context;

	/**
	 * Name of the topic.
	 */
	const char *name;

	/**
	 * Number of messages in the topic.
	 */
	size_t numMessages;

	/**
	 * Reference count of this topic.
	 */
	int refCount;
};

struct KLMessageInfo
{
};

#endif

