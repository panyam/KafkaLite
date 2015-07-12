
#include "benchmark.h"

/**
 * Single threaded test with single producer and one or more consumers
 * with each actor starting after a certain number of messages.
 */
void test_single_thread(Benchmark *bm)
{
	// create all consumers here
    KLIterator **iterators = malloc(sizeof(KLIterator *) * bm->numConsumers);
	for (int i = 0;i < bm->numConsumers;i++)
	{
		iterators[i] = kl_iterator_new(bm->context, "topic", 0);
	}
    KLMessage *message = malloc(sizeof(KLMessage) + bm->maxMessageSize);
	int startingConsumer = 0;
	int endingConsumer = 0;
	int numPublished = 0;
	int *numConsumed = calloc(bm->numConsumers, sizeof(int));
	int totalNumConsumed = 0;
	while (numPublished < bm->numMessages || totalNumConsumed < (bm->numConsumers * bm->numMessages))
	{
		if (numPublished < bm->numMessages)
		{
#ifdef TEST_ORDER
			numPublished++;
			printf("  P (%05d)  ", numPublished);
#else
			publishMessage(bm);
			numPublished++;
#endif
		} else {
#ifdef TEST_ORDER
			printf("  -          ");
#endif
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

#ifdef TEST_ORDER
		make all consumers consume!
		for (int c = 0;c < startingConsumer;c++) printf("  -          ");
#endif

		for (int c = startingConsumer;c < endingConsumer;c++)
		{
			if (numConsumed[c] < bm->numMessages)
			{
#ifdef TEST_ORDER
				printf("  %d (%05d)  ", c, numConsumed[c]);
#else
        		consumeMessage(bm, iterators[c], message);
#endif
				numConsumed[c]++;
				totalNumConsumed++;
			}
			if (c == startingConsumer && numConsumed[c] >= bm->numMessages)
			{
				printf("Consumer Finished: %d\n", c);
				startingConsumer++;
			}
		}

#ifdef TEST_ORDER
		for (int c = endingConsumer;c < bm->numConsumers;c++) printf("  -          ");
		puts("");
#endif
	}
}
