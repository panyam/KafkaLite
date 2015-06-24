
#ifndef __KL_UTILS_H__
#define __KL_UTILS_H__

#include "klfwddefs.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

extern bool ensure_directory(const char *path);

/**
 * Remove a directory and its children recursively.
 */
extern void rmdirs(const char *path);

#define kl_log 		printf

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

#endif

