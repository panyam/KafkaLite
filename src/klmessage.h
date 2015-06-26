

#ifndef __KL_MESSAGE_H__
#define __KL_MESSAGE_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Info/Metadata about a message.
 */
struct KLMessageHeader
{
	off_t offset;
	size_t size;
};

/**
 * Stores the message size and data.
 */
struct KLMessage
{
	size_t size;
	char data[];
};

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

