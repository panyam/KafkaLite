
#include "benchmark.h"

void *publisher_thread(void *data);
void *consumer_thread(void *data);

typedef struct ConsumerThreadInfo
{
    Benchmark *bm;
    pthread_t threadId;
    int threadIndex;
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
 * The publisher thread essentially publishes messages on its own thread
 * and spawns upto numThreads consumer threads, where each consumer 
 * thread runs (numConsumers / numThreads) consumers in a multiplexed 
 * fashion.
 */
void *publisher_thread(void *data)
{
    PublisherThreadInfo *pti = (PublisherThreadInfo *)data;
    Benchmark *bm = pti->bm;
    for (int i = 0;i < bm->numMessages;i++)
    {
        publishMessage(bm);
        if (bm->leadAmount ==  0 || (i != 0 && bm->leadAmount > 0 && i % bm->leadAmount == 0))
        {
            if (pti->numConsumersCreated < bm->numThreads)
            {
                pti->consumerThreads[i].threadIndex = pti->numConsumersCreated++;
                pthread_create(&pti->consumerThreads[i].threadId, NULL, consumer_thread, pti->consumerThreads + i);
            }
        }
    }
    return NULL;
}

void *consumer_thread(void *data)
{
    ConsumerThreadInfo *cti = (ConsumerThreadInfo *)data;
    Benchmark *bm = cti->bm;
    KLIterator **iterators = cti->iterators;
	KLMessage *message = cti->message;
    int startingConsumer = 0;
    int endingConsumer = 0;
    int numPublished = 0;
    int *numConsumed = calloc(bm->numConsumers, sizeof(int));
    int totalNumConsumed = 0;
    while (numPublished < bm->numMessages || totalNumConsumed < (bm->numConsumers * bm->numMessages))
    {
        if (numPublished < bm->numMessages)
        {
            numPublished++;
            // printf("  P (%05d)  ", numPublished);
        } else {
            // printf("  -          ");
        }

        // Is it time to start a consumer?
        if (endingConsumer < bm->numConsumers && numPublished % bm->leadAmount == 0)
        {
            // start another consumer
            if (numPublished % bm->leadAmount == 0)
            {
                endingConsumer++;
            }
        }

        // make all consumers consume!
        // for (int c = 0;c < startingConsumer;c++) printf("  -          ");

        for (int c = startingConsumer;c < endingConsumer;c++)
        {
            if (numConsumed[c] < bm->numMessages)
            {
                consumeMessage(bm, iterators[c], message);
                // printf("  %d (%05d)  ", c, numConsumed[c]);
                numConsumed[c]++;
                totalNumConsumed++;
            }
            if (c == startingConsumer && numConsumed[c] >= bm->numMessages)
            {
                printf("Consumer Finished: %d\n", c);
                startingConsumer++;
            }
        }

        // for (int c = endingConsumer;c < bm->numConsumers;c++) printf("  -          ");
        // puts("");
    }
    return NULL;
}

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
        int numConsumers = bm->numConsumers / bm->numThreads;
        if (bm->numConsumers % bm->numThreads != 0)
            numConsumers++;
        pti.consumerThreads[i].message = malloc(sizeof(KLMessage) + bm->maxMessageSize);
        pti.consumerThreads[i].iterators = calloc(numConsumers, sizeof(KLIterator *));
		for (int j = 0;j < numConsumers;j++)
		{
        	pti.consumerThreads[i].iterators[j] = kl_iterator_new(bm->context, "topic", 0);
		}
    }
    
    // now start the publisher and let it create the consumers depending on lead
    // amount and number of threads
    pthread_create(&pti.threadId, NULL, publisher_thread, &pti);

	// wait for all threads to finish
	void *retval = NULL;
	puts("Waiting for publisher...");
	pthread_join(pti.threadId, &retval);
    for (int i = 0;i < bm->numThreads;i++)
	{
		printf("Waiting for consumer thread %d...\n", i);
		pthread_join(pti.consumerThreads[i].threadId, &retval);
	}
}
