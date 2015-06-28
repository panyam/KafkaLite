
#include <kafkalite.h>

#ifndef __KL_BENCHMARKS_H__
#define __KL_BENCHMARKS_H__

#define DELIM      ','

typedef struct Benchmark
{
    char *contextDir;
    int leadAmount;
    int numMessages;
    int numProducers;
    int numConsumers;
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

#endif


