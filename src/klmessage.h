

#ifndef __KL_MESSAGE_H__
#define __KL_MESSAGE_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Info/Metadata about a message.
 */
struct KLMessageMetadata
{
	int64_t offset;
	uint64_t size;
};

/**
 * Stores the message size and data.
 */
struct KLMessage
{
	uint64_t size;
	char data[];
};

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

