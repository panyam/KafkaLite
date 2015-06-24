
#ifndef __KL_MUTEX_H__
#define __KL_MUTEX_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

/** 
 * A factory for managing platform specific concurrency primitives.
 */
struct KLMutexFactory
{
	void *(*MutexNew)(void *data);
	void (*MutexDestroy)(void *data);
	void (*MutexLock)(void *data);
	void (*MutexUnlock)(void *data);
};

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

