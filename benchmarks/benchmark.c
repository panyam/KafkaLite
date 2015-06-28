
#include "benchmark.h"

void setup(Benchmark *bm)
{
    loadTestFiles(bm);
    kl_rmdirs(bm->contextDir);
    bm->context = kl_context_open(bm->contextDir, NULL);
    bm->topic = kl_topic_open(bm->context, "topic");
}

/**
 * Single producer and single consumer
 */
void test_1p_1c_1thread(Benchmark *bm)
{
    int i = 0;
    KLIterator *iterator = kl_iterator_new(bm->context, "topic", 0);
    KLMessage *message = malloc(sizeof(KLMessage) + bm->maxMessageSize);

    // produce and consume
    for (;i < bm->leadAmount;i++)
    {
        publishMessage(bm);
    }

    // produce and consume
    for (;i < bm->numMessages;i++)
    {
        publishMessage(bm);
        consumeMessage(bm, iterator, message);
    }

    // finish with the remaining consumers
    for (i = 0;i < bm->leadAmount;i++)
    {
        consumeMessage(bm, iterator, message);
    }
}

int main(int argc, char *argv[])
{
    //int numCPU = sysconf( _SC_NPROCESSORS_ONLN ); printf("NC: %d\n", numCPU);
     Benchmark *benchmark = calloc(1, sizeof(Benchmark));;
    parseArgs(benchmark, argc, argv);
    setup(benchmark);

    long long beforeTime = kl_current_timestamp();
    test_1p_1c_1thread(benchmark);
    long long afterTime = kl_current_timestamp();
    kl_log("\nElapsed Time for %d messages: %lld\n", benchmark->numTestMessages, afterTime - beforeTime);
}
