
#include "klprivate.h"

/**
 * Creates a new KL context starting at a particular folder where all data will
 * be stored. This is what is shared by all consumers and producers in a
 * system.
 */
KLContext *kl_context_open(const char *baseDir, KLMutexFactory *mutexFactory)
{
	if (!ensure_directory(baseDir))
		return NULL;
	KLContext *out = calloc(1, sizeof(KLContext));
	out->baseDir = strdup(baseDir);
	out->topicsDir = malloc(strlen(baseDir) + 1 + strlen("/topics"));
	sprintf(out->topicsDir, "%s/topics", baseDir);

	out->mutexFactory = mutexFactory;

	out->topics = kl_array_new(sizeof(KLTopic), 32);
	if (out->mutexFactory)
	{
		out->topicsMutex = out->mutexFactory->MutexNew(NULL);
		out->rwLock = out->mutexFactory->RWLockNew(NULL);
	}
	return out;
}

/**
 * Destroys a KL context and is no longer usable by the consumers and the
 * producers.
 */
void kl_context_close(KLContext *context)
{
	if (context)
	{
		kl_log("Closing context...\n");
		for (int i = 0, count = kl_array_count(context->topics);i < count;i++)
		{
			// close the context
			KLTopic *topic = (KLTopic *)kl_array_element_at(context->topics, i);
			kl_topic_finalize(topic);
		}
		kl_array_destroy(context->topics);
		if (context->mutexFactory)
			context->mutexFactory->MutexDestroy(context->topicsMutex);
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
