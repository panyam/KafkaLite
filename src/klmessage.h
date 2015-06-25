

#ifndef __KL_MESSAGE_H__
#define __KL_MESSAGE_H__

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
};

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

