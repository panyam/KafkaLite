
#ifndef __KAFKA_LITE_FWD_DEFS_H__
#define __KAFKA_LITE_FWD_DEFS_H__

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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

typedef struct KLMessageInfo KLMessageInfo;
typedef KLMessageInfo * KLMessageInfoRef;

typedef struct KLMessage KLMessage;
typedef KLMessage * KLMessageRef;

typedef struct KLMutex KLMutex;
typedef KLMutex * KLMutexRef;

typedef struct KLMutexFactory KLMutexFactory;
typedef KLMutexFactory * KLMutexFactoryRef;


#endif

