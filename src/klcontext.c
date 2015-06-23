
#include "kafkalite.h"

struct KLContext
{
	char basedir[PATH_MAX];
};

/**
 * Creates a new KL context starting at a particular folder where all data will
 * be stored.   This is what is shared by all consumers and producers in a
 * system.
 */
KLContext *kl_context_new(const char *basedir)
{
	KLContext *out = calloc(1, sizeof(KLContext));
	strcpy(out->basedir, basedir);
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

