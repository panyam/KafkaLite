
#ifndef __KAFKA_LITE_FWD_DEFS_H__
#define __KAFKA_LITE_FWD_DEFS_H__

/*
#define Int64 int64_t
#define UInt64 uint64_t

#ifdef _WIN32		// Win specific defs
// #define Int64 int64
// #define UInt64 uint64
#elif __APPLE__		// Apple specific defs
// #include <objc/NSObjcRuntime.h>
// #define Int64 NSInteger
// #define UInt64 NSUInteger
#else				// Treat __linux, __unix, __posix and the rest as posix for now
*/
#define _POSIX_C_SOURCE 200809L
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

// #endif

#include <inttypes.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <limits.h>

typedef struct KLContext KLContext;
typedef KLContext * KLContextRef;

typedef struct KLArray KLArray;
typedef KLArray * KLArrayRef;

typedef struct KLBuffer KLBuffer;
typedef KLBuffer * KLBufferRef;

typedef struct KLTopic KLTopic;
typedef KLTopic * KLTopicRef;

typedef struct KLMessageMetadata KLMessageMetadata;
typedef KLMessageMetadata * KLMessageMetadataRef;

typedef struct KLMessage KLMessage;
typedef KLMessage * KLMessageRef;

typedef struct KLMessageHeader KLMessageHeader;
typedef KLMessageHeader * KLMessageHeaderRef;

typedef struct KLMutex KLMutex;
typedef KLMutex * KLMutexRef;

typedef struct KLLockManager KLLockManager;
typedef KLLockManager * KLLockManagerRef;

typedef struct KLIterator KLIterator;
typedef KLIterator * KLIteratorRef;

#endif

