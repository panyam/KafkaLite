
#include "benchmark.h"

void loadTestFiles(Benchmark *bm)
{
    char *token, *tofree, *string;
    tofree = string = strdup(bm->testFileNames);

    int numCommas = 0;
    for (char *tmp = string;*tmp;tmp++)
        if (*tmp == DELIM)
            numCommas++;

    bm->maxMessageSize = 0;
    bm->numTestMessages = 0;
    bm->testMessageSizes = (size_t *)malloc(sizeof(size_t) * (numCommas + 1));
    bm->testMessages = (char **)malloc(sizeof(char *) * (numCommas + 1));
    while ((token = strsep(&string, ",")) != NULL)
    {
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
                bm->testMessageSizes[bm->numTestMessages] = fileStat.st_size;
                bm->testMessages[bm->numTestMessages] = (char *)malloc(fileStat.st_size);
                read(fd, bm->testMessages[bm->numTestMessages], fileStat.st_size);
                close(fd);
                bm->numTestMessages++;
            }
        }
    }
    free(tofree);
}

void usage()
{
    printf("Usage: benchmarks <options>\n");
    printf("    Options:\n");
    printf("        -m    Number of messages to publish\n");
    printf("        -p    Number of producers\n");
    printf("        -c    Number of consumers\n");
    exit(1);
}

void parseArgs(Benchmark *bm, int argc, char *argv[])
{
    bm->contextDir = strdup("/tmp/kafka");
    bm->leadAmount = 20;
    bm->numMessages = 1000000;
    bm->numProducers = 1;
    bm->numConsumers = 1;
    bm->numTestMessages = 0;
    bm->testMessageSizes = NULL;
    bm->testMessages = NULL;

    for (int i = 1;i < argc;i++)
    {
        if (strcmp(argv[i], "-m") == 0)
        {
            bm->numMessages = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-c") == 0)
        {
            bm->numConsumers = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0)
        {
            bm->numProducers = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0)
        {
            bm->testFileNames = strdup(argv[++i]);
        } else {
            usage();
        }
    }
    if (bm->testFileNames == NULL)
    {
        bm->testFileNames = strdup("messages/0.txt,messages/1.txt,messages/2.txt,messages/3.txt,messages/4.txt,"
                                   "messages/5.txt,messages/6.txt,messages/7.txt,messages/8.txt,messages/9.txt");
    }
}

void publishMessage(Benchmark *bm)
{
    int msgIndex = rand() % bm->numTestMessages;
    const char *message = bm->testMessages[msgIndex];
    size_t msgsize = bm->testMessageSizes[msgIndex];
    kl_topic_publish(bm->topic, message, msgsize);
}

void consumeMessage(Benchmark *bm, KLIterator *iterator, KLMessage *message)
{
    kl_iterator_forward(iterator);
    kl_iterator_message(iterator, message);
}

