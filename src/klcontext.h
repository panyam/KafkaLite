
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
extern KLContext *kl_context_open(const char *basedir, KLLockManager *lockManager);

/**
 * Closes a KL context and is no longer usable by the consumers and the producers.
 * To reuse this context call kl_context_open again.
 */
extern void kl_context_close(KLContext *context);

/**
 * Gets the base folder for the KL context.
 */
extern const char *kl_context_basedir(KLContext *context);

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

