
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

void kl_pthread_mutex_lock(void *data)
{
	pthread_mutex_lock((pthread_mutex_t *)data);
}

void kl_pthread_mutex_unlock(void *data)
{
	pthread_mutex_unlock((pthread_mutex_t *)data);
}

KLMutexFactory *kl_pthread_mutex_factory()
{
	KLMutexFactory *factory = calloc(1, sizeof(KLMutexFactory));
	factory->MutexNew = kl_pthread_mutex_new;
	factory->MutexDestroy = kl_pthread_mutex_destroy;
	factory->MutexLock = kl_pthread_mutex_lock;
	factory->MutexUnlock = kl_pthread_mutex_unlock;
	return factory;
}
