
#include "benchmark.h"

void *publisher_thread(void *data);
void *consumer_thread(void *data);

typedef struct ConsumerThreadInfo
{
    Benchmark *bm;
    pthread_t threadId;
    int threadIndex;
    int numConsumersInThread;
    // Iterator for the consumer
    KLIterator **iterators;
    // Message fetched by the consumer
    KLMessage *message;
} ConsumerThreadInfo;

typedef struct PublisherThreadInfo
{
    Benchmark *bm;
    pthread_t threadId;
    int numConsumersCreated;
    ConsumerThreadInfo *consumerThreads;
} PublisherThreadInfo;

/**
 * Single threaded test with single publisher and one or more consumers
 * with each actor starting after a certain number of messages.
 */
void test_multiple_threads(Benchmark *bm)
{
    // create all consumers here
    PublisherThreadInfo pti;
    memset(&pti, 0, sizeof(pti));
    pti.bm = bm;
    pti.consumerThreads = calloc(bm->numConsumers, sizeof(ConsumerThreadInfo));
    for (int i = 0;i < bm->numThreads;i++)
    {
        // How many consumers is this thread handling
        ConsumerThreadInfo *cti = pti.consumerThreads + i;
        cti->bm = bm;
        cti->threadIndex = i;
        cti->numConsumersInThread = bm->numConsumers / bm->numThreads;
        if (bm->numConsumers % bm->numThreads != 0)
        {
            if (i == bm->numThreads - 1)
            {
                // last thread is gonna server lesser number of consumers than
                // the others
                cti->numConsumersInThread = bm->numConsumers % bm->numThreads;
            } else {
                cti->numConsumersInThread ++;
            }
        }
        cti->message = malloc(sizeof(KLMessage) + bm->maxMessageSize);
        cti->iterators = calloc(cti->numConsumersInThread, sizeof(KLIterator *));
        printf("Creating %d consumers for thread %d\n", cti->numConsumersInThread, i);
        for (int j = 0;j < cti->numConsumersInThread;j++)
        {
            cti->iterators[j] = kl_iterator_new(bm->context, "topic", 0);
        }
    }
    
    // now start the publisher and let it create the consumers depending on lead
    // amount and number of threads
    int result = pthread_create(&pti.threadId, NULL, publisher_thread, &pti);
    printf("Publisher thread created.  Result: %d, ID: %lu\n", result, pti.threadId);

    // wait for all threads to finish
    void *retval = NULL;
    puts("Waiting for publisher...");
    pthread_join(pti.threadId, &retval);
    puts("Waiting for consumers...");
    for (int i = 0;i < bm->numThreads;i++)
    {
        printf("Waiting for consumer thread %d...\n", i);
        pthread_join(pti.consumerThreads[i].threadId, &retval);
    }
}

/**
 * The publisher thread essentially publishes messages on its own thread
 * and spawns upto numThreads consumer threads, where each consumer 
 * thread runs (numConsumers / numThreads) consumers in a multiplexed 
 * fashion.
 */
void *publisher_thread(void *data)
{
    PublisherThreadInfo *pti = (PublisherThreadInfo *)data;
    Benchmark *bm = pti->bm;
    printf("Publisher thread started. ID: %ld\n", pti->threadId);
    for (int i = 0;i < bm->numMessages;i++)
    {
        publishMessage(bm);
        if (bm->leadAmount ==  0 || (i != 0 && bm->leadAmount > 0 && i % bm->leadAmount == 0))
        {
            if (pti->numConsumersCreated < bm->numThreads)
            {
                ConsumerThreadInfo *cti = pti->consumerThreads + pti->numConsumersCreated;
                int result = pthread_create(&cti->threadId, NULL, consumer_thread, cti);
                printf("Consumer thread %d [%p] created.  Result: %d, ID: %lu\n", cti->threadIndex, cti, result, cti->threadId);
                pti->numConsumersCreated++;
            }
        }
    }
    puts("Publisher thread finished.");
    return NULL;
}

void *consumer_thread(void *data)
{
    ConsumerThreadInfo *cti = (ConsumerThreadInfo *)data;
    printf("Consumer thread %d [%p] started. ID: %lu\n", cti->threadIndex, cti, cti->threadId);

    Benchmark *bm = cti->bm;
    int *numConsumed = calloc(bm->numConsumers, sizeof(int));

    int numConsumers = cti->numConsumersInThread;
    int startingConsumer = 0;
    int endingConsumer = 0;
    int totalNumConsumed = 0;

    int currConsumer = startingConsumer;
    while (totalNumConsumed < numConsumers * bm->numMessages)
    {
        // Is it time to start a consumer?
        if (endingConsumer < numConsumers && (bm->leadAmount == 0 || totalNumConsumed % bm->leadAmount == 0))
        {
            // start another consumer
            printf("Thread %d, Starting consumer: %d, MsgIndex: %d\n", cti->threadIndex, endingConsumer, totalNumConsumed);
            endingConsumer++;
        }

        if (numConsumed[currConsumer] < bm->numMessages)
        {
            consumeMessage(bm, cti->iterators[currConsumer], cti->message);
            numConsumed[currConsumer]++;
            totalNumConsumed++;
        }

        if (currConsumer == startingConsumer && numConsumed[currConsumer] >= bm->numMessages)
        {
            printf("Thread %d, Consumer Finished: %d, MsgIndex: %d\n", cti->threadIndex, currConsumer, totalNumConsumed);
            startingConsumer++;
        }
        currConsumer++;
        if (currConsumer == endingConsumer)
            currConsumer = startingConsumer;
    }
    printf("Consumer thread %d finished.\n", cti->threadIndex);
    return NULL;
}
