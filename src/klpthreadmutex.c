
#include "klprivate.h"
#include <pthread.h>

void *kl_pthread_mutex_new(void *data)
{
	pthread_mutex_t *Mutex = calloc(1, sizeof(pthread_mutex_t));;
	pthread_mutexattr_t Attr;

	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(Mutex, &Attr);
	return Mutex;
}

void kl_pthread_mutex_destroy(void *data)
{
	pthread_mutex_destroy((pthread_mutex_t *)data);
}

void kl_pthread_mutex_lock(void *data, bool tryOnly)
{
	if (tryOnly)
	{
		pthread_mutex_trylock((pthread_mutex_t *)data);
	} else {
		pthread_mutex_lock((pthread_mutex_t *)data);
	}
}

void kl_pthread_mutex_unlock(void *data)
{
	pthread_mutex_unlock((pthread_mutex_t *)data);
}

////// RW Locks

void *kl_pthread_rwlock_new(void *data)
{
	pthread_rwlock_t *Mutex = calloc(1, sizeof(pthread_rwlock_t));;
	pthread_rwlock_init(Mutex, NULL);
	return Mutex;
}

void kl_pthread_rwlock_destroy(void *data)
{
	pthread_rwlock_destroy((pthread_rwlock_t *)data);
}

void kl_pthread_rwlock_rdlock(void *data, bool tryOnly)
{
	if (tryOnly)
	{
		pthread_rwlock_tryrdlock((pthread_rwlock_t *)data);
	} else {
		pthread_rwlock_rdlock((pthread_rwlock_t *)data);
	}
}

void kl_pthread_rwlock_wrlock(void *data, bool tryOnly)
{
	if (tryOnly)
	{
		pthread_rwlock_trywrlock((pthread_rwlock_t *)data);
	} else {
		pthread_rwlock_wrlock((pthread_rwlock_t *)data);
	}
}

void kl_pthread_rwlock_rdunlock(void *data)
{
	pthread_rwlock_unlock((pthread_rwlock_t *)data);
}

KLLockManager *kl_pthread_mutex_manager()
{
	KLLockManager *manager = calloc(1, sizeof(KLLockManager));
	manager->MutexNew = kl_pthread_mutex_new;
	manager->MutexDestroy = kl_pthread_mutex_destroy;
	manager->MutexLock = kl_pthread_mutex_lock;
	manager->MutexUnlock = kl_pthread_mutex_unlock;

	manager->RWLockNew = kl_pthread_rwlock_new;
	manager->RWLockDestroy = kl_pthread_rwlock_destroy;
	manager->RWLockReadLock = kl_pthread_rwlock_rdlock;
	manager->RWLockWriteLock = kl_pthread_rwlock_wrlock;
	manager->RWLockUnlock = kl_pthread_rwlock_rdunlock;

	return manager;
}
