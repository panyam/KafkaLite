
#include "klprivate.h"

static void kl_context_set_mutex_factory(KLContext *ctx, KLLockManager *lockManager);

/**
 * Creates a new KL context starting at a particular folder where all data will
 * be stored. This is what is shared by all consumers and producers in a
 * system.
 */
KLContext *kl_context_open(const char *baseDir, KLLockManager *lockManager)
{
	if (!kl_ensure_dir(baseDir))
		return NULL;
	KLContext *out = calloc(1, sizeof(KLContext));
	out->baseDir = strdup(baseDir);
	out->topicsDir = malloc(strlen(baseDir) + 1 + strlen("/topics"));
	sprintf(out->topicsDir, "%s/topics", baseDir);
	if (!kl_ensure_dir(out->topicsDir))
	{
		free(out->baseDir);
		free(out->topicsDir);
		free(out);
		return NULL;
	}

	out->topics = kl_array_new(sizeof(KLTopic), 32);
	kl_context_set_mutex_factory(out, lockManager);
	return out;
}

/**
 * Sets the mutex factory.
 */
void kl_context_set_mutex_factory(KLContext *ctx, KLLockManager *lockManager)
{
	if (ctx->lockManager)
	{
		// remove old mutexes
		ctx->lockManager->MutexDestroy(ctx->topicsMutex);
	}
	ctx->lockManager = lockManager;
	if (ctx->lockManager)
	{
		// create new mutexes
		ctx->topicsMutex = ctx->lockManager->MutexNew(NULL);
	}
}

/**
 * Destroys a KL context and is no longer usable by the consumers and the
 * producers.
 */
void kl_context_close(KLContext *context)
{
	if (context)
	{
		for (off_t i = 0, count = kl_array_count(context->topics);i < count;i++)
		{
			// close the context
			KLTopic *topic = (KLTopic *)kl_array_element_at(context->topics, i);
			kl_topic_finalize(topic);
		}
		kl_array_destroy(context->topics);
		kl_context_set_mutex_factory(context, NULL);
		free(context->baseDir);
		free(context->topicsDir);
		free(context);
	}
}

/**
 * Gets the base folder for the KL context.
 */
const char *kl_context_basedir(KLContext *context)
{
	return context ? context->baseDir : NULL;
}
