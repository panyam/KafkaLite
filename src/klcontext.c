
#include "klprivate.h"

/**
 * Creates a new KL context starting at a particular folder where all data will
 * be stored.   This is what is shared by all consumers and producers in a
 * system.
 */
KLContext *kl_context_new(const char *basedir)
{
	KLContext *out = calloc(1, sizeof(KLContext));
	strcpy(out->basedir, basedir);
	out->topicIdCounter = 0;
	out->topics = kl_array_new(sizeof(KLTopic), 64);
	return out;
}

/**
 * Destroys a KL context and is no longer usable by the consumers and the
 * producers.
 */
void kl_context_destroy(KLContext *context)
{
	if (context)
		free(context);
}

/**
 * Gets the base folder for the KL context.
 */
const char *kl_context_basedir(KLContext *context)
{
	return context ? context->basedir : NULL;
}
