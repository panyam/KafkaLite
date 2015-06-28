
#ifndef __KL_UTILS_H__
#define __KL_UTILS_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

extern bool kl_ensure_dir(const char *path);

/**
 * Remove a directory and its children recursively.
 */
extern void kl_rmdirs(const char *path);

extern long long kl_current_timestamp();

#define kl_log 		printf

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

