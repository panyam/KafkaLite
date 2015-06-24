
#ifndef __KL_MUTEX_H__
#define __KL_MUTEX_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

#define KL_MUTEX_LOCK(factory, mutex)		do { if (factory) factory->MutexLock(mutex, false); } while(0)
#define KL_MUTEX_TRYLOCK(factory, mutex)	do { if (factory) factory->MutexLock(mutex, true); } while(0)
#define KL_MUTEX_UNLOCK(factory, mutex)		do { if (factory) factory->MutexUnlock(mutex); } while(0)

#define KL_RWLOCK_RDLOCK(factory, mutex)	do { if (factory) factory->RWLockReadLock(mutex, false); } while(0)
#define KL_RWLOCK_TRYRDLOCK(factory, mutex)	do { if (factory) factory->RWLockReadLock(mutex, true); } while(0)
#define KL_RWLOCK_WRLOCK(factory, mutex)	do { if (factory) factory->RWLockWriteLock(mutex, false); } while(0)
#define KL_RWLOCK_TRYWRLOCK(factory, mutex)	do { if (factory) factory->RWLockWriteLock(mutex, true); } while(0)
#define KL_RWLOCK_UNLOCK(factory, mutex)	do { if (factory) factory->RWLockUnlock(mutex); } while(0)

/** 
 * A factory for managing platform specific concurrency primitives.
 */
struct KLMutexFactory
{
	void *(*MutexNew)(void *data);
	void (*MutexDestroy)(void *data);
	void (*MutexLock)(void *data, bool tryOnly);
	void (*MutexUnlock)(void *data);

	void *(*RWLockNew)(void *data);
	void (*RWLockDestroy)(void *data);
	void (*RWLockReadLock)(void *data, bool tryOnly);
	void (*RWLockWriteLock)(void *data, bool tryOnly);
	void (*RWLockUnlock)(void *data);
};

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

