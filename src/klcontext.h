
#ifndef __KL_CONTEXT_H__
#define __KL_CONTEXT_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Creates a new KL context starting at a particular folder where all data will
 * be stored.   This is what is shared by all consumers and producers in a
 * system.
 */
extern KLContextRef kl_context_new(const char *basedir);

/**
 * Destroys a KL context and is no longer usable by the consumers and the
 * producers.
 */
extern void kl_context_destroy(KLContextRef context);

/**
 * Gets the base folder for the KL context.
 */
extern const char *kl_context_basedir(KLContextRef context);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

