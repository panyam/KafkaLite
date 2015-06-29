

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
	Int64 offset;
	UInt64 size;
};

/**
 * Stores the message size and data.
 */
struct KLMessage
{
	UInt64 size;
	char data[];
};

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

