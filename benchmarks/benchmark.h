
#include <kafkalite.h>

#ifndef __KL_BENCHMARKS_H__
#define __KL_BENCHMARKS_H__

#define DELIM      ','
#define ACTOR_STARTED -1
#define ACTOR_FINISHED -2

#include <pthread.h>

typedef struct Benchmark
{
    char *contextDir;
    int leadAmount;
    int numMessages;
    int numPublishers;
    int numConsumers;
    int numThreads;
    int numTestMessages;
    char *testFileNames;
    KLArray *testMessages;
    size_t maxMessageSize;
    KLContext *context;
    KLTopic *topic;
} Benchmark;

extern void loadTestFiles(Benchmark *bm);
extern void parseArgs(Benchmark *bm, int argc, char *argv[]);
extern void publishMessage(Benchmark *bm);
extern void consumeMessage(Benchmark *bm, KLIterator *iterator, KLMessage *message);
extern void stagger_actors(int numActors, int numMessages, int leadAmount,
						   void *data, void (*func)(int actor, int index, void *data));

#endif


