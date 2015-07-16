
#include "benchmark.h"

typedef struct SingleThreadedTest
{
    Benchmark *bm;
    KLIterator **iterators;
    KLMessage *message;
} SingleThreadedTest;

void pubsub(int actor, int index, void *data)
{
    SingleThreadedTest *ctx = data;
    if (index >= 0)
    {
        if (actor == 0)
            publishMessage(ctx->bm);
        else
            consumeMessage(ctx->bm, ctx->iterators[actor], ctx->message);
    }
}


/**
 * Single threaded test with single producer and one or more consumers
 * with each actor starting after a certain number of messages.
 */
void test_single_thread(Benchmark *bm)
{
    // create all consumers here
    SingleThreadedTest ctx;
    ctx.bm = bm;
    ctx.iterators = malloc(sizeof(KLIterator *) * bm->numConsumers);
    for (int i = 0;i < bm->numConsumers;i++)
    {
        ctx.iterators[i] = kl_iterator_new(bm->context, "topic", 0);
    }
    ctx.message = malloc(sizeof(KLMessage) + bm->maxMessageSize);

    stagger_actors(bm->numConsumers + bm->numPublishers, bm->numMessages, bm->leadAmount, &ctx, pubsub);
}

