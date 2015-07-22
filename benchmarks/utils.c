
#include "benchmark.h"

void loadTestFiles(Benchmark *bm)
{
    bm->maxMessageSize = 0;
    bm->testMessages = kl_array_new(sizeof(KLMessage *), 20);

    const char *start = bm->testFileNames;
    const char *realend = start + strlen(start);
    while (start < realend)
    {
        const char *end = start;
        while (*end && *end != ',') end++;
        char *token = strndup(start, end - start);
        start = end + 1;

        printf("Loading Test File: %s\n", token);
        struct stat fileStat;
        if (stat(token, &fileStat) != 0)
        {
            printf("Error with file: %s\n", strerror(errno));
        } else if ((fileStat.st_mode & S_IFREG) == 0) {
            printf("Not a valid file.");
        } else {
            int fd = open(token, O_RDONLY);
            if (fd <= 0)
            {
                printf("Error opening file: %s\n", strerror(errno));
            } else {
                if (fileStat.st_size > bm->maxMessageSize)
                    bm->maxMessageSize = fileStat.st_size;

                KLMessage **nextMsgSize = kl_array_insert_at(bm->testMessages, -1);
                nextMsgSize[0] = (KLMessage *)malloc(sizeof(KLMessage) + fileStat.st_size);
                nextMsgSize[0]->header.size = fileStat.st_size;
                read(fd, nextMsgSize[0]->data, fileStat.st_size);
                close(fd);
            }
        }


        free(token);
    }
}

void usage()
{
    printf("Usage: benchmarks <options>\n");
    printf("    Options:\n");
    printf("        -m    Number of messages to publish\n");
    printf("        -p    Number of producers\n");
    printf("        -c    Number of consumers\n");
    printf("        -nt   Number of threads.  1 Thread will be for publisher and the rest of the threads will be for consumers\n");
    printf("        -t    Comma separated names of test message files that will be published randomly\n");
    printf("        -l    The number of messages by which the consumer will lag the publisher.\n");
    exit(1);
}

void parseArgs(Benchmark *bm, int argc, char *argv[])
{
    bm->contextDir = strdup("/tmp/kafka");
    bm->leadAmount = 20;
    bm->numMessages = 1000000;
    bm->numPublishers = 1;
    bm->numConsumers = 1;
    bm->testMessages = NULL;
    bm->numThreads = 0;

    for (int i = 1;i < argc;i++)
    {
        if (strcmp(argv[i], "-m") == 0)
        {
            bm->numMessages = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-c") == 0)
        {
            bm->numConsumers = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-nt") == 0)
        {
            bm->numThreads = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-l") == 0)
        {
            bm->leadAmount = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0)
        {
            bm->numPublishers = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0)
        {
            bm->testFileNames = strdup(argv[++i]);
        } else {
            usage();
        }
    }

    if (bm->numThreads >= bm->numConsumers)
    {
        bm->numConsumers = bm->numThreads;
    }

    if (bm->testFileNames == NULL)
    {
        bm->testFileNames = strdup("messages/0.txt,messages/1.txt,messages/2.txt,messages/3.txt,messages/4.txt,"
                                   "messages/5.txt,messages/6.txt,messages/7.txt,messages/8.txt,messages/9.txt");
    }
}

void publishMessage(Benchmark *bm)
{
    KLMessage **message = kl_array_random(bm->testMessages);
    kl_topic_publish(bm->topic, message[0]->data, message[0]->header.size);
}

void consumeMessage(Benchmark *bm, KLIterator *iterator, KLMessage *message)
{
    kl_iterator_forward(iterator);
    kl_iterator_message(iterator, message);
}


void stagger_actors(int numActors, int numMessages, int leadAmount,
                    void *data, void (*func)(int actor, int index, void *data))
{
    // create all consumers here
    int startingActor = 0;
    int endingActor = 0;
    int totalActions = 0;
    int maxActions = numActors * numMessages;
#define USE_FIXED_SIZE
#ifdef USE_FIXED_SIZE
    int numPerformed[1024] = {0};
#else
    int *numPerformed = calloc(numActors, sizeof(int));
#endif
    while (totalActions < maxActions)
    {
        // Is it time to start a new actor?
        if (endingActor < numActors && (endingActor == startingActor || totalActions % leadAmount == 0))
        {
            endingActor++;
        }

        for (int actor = startingActor;actor < endingActor;actor++)
        {
            if (numPerformed[actor] < numMessages)
            {
                if (numPerformed[actor] == 0)
                    func(actor, ACTOR_STARTED, data);
                func(actor, numPerformed[actor], data);
                numPerformed[actor]++;
                totalActions++;
            }
            if (actor == startingActor && numPerformed[actor] >= numMessages)
            {
                func(actor, ACTOR_FINISHED, data);
                startingActor++;
            }
        }
    }
#ifndef USE_FIXED_SIZE
    free(numPerformed);
#endif
}

