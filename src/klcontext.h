
#ifndef __KL_CONTEXT_H__
#define __KL_CONTEXT_H__

#include "klfwddefs.h"

/**
 * Creates a new KL context starting at a particular folder where all data will
 * be stored.   This is what is shared by all consumers and producers in a
 * system.
 */
extern KLContext *kl_context_new(const char *basedir);

/**
 * Destroys a KL context and is no longer usable by the consumers and the
 * producers.
 */
extern void kl_context_destroy(KLContext *context);

/**
 * Gets the base folder for the KL context.
 */
extern const char *kl_context_basedir(KLContext *context);

#endif

