
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <kafkalite.h>

static char *const contextDir = "/tmp/kafka";
static int numMessages = 1000000;
static int numProducers = 1;
static int numConsumers = 1;
static KLContext *context;

/**
 * Returns a small message 
const char *smallMessage(int *length)
{
}

void setup()
{
	rmdirs(contextDir);
	context = kl_context_open(contextDir, NULL);
}

void usage()
{
	printf("Usage: benchmarks <options>\n");
	printf("	Options:\n");
	printf("		-m	Number of messages to publish\n");
	printf("		-p	Number of producers\n");
	printf("		-c	Number of consumers\n");
	exit(1);
}

void parseArgs(int argc, char *argv[])
{
	for (int i = 1;i < argc;i++)
	{
		if (strcmp(argv[i], "-m") == 0)
		{
			numMessages = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-c") == 0)
		{
			numConsumers = atoi(argv[++i]);
		} else if (strcmp(argv[i], "-p") == 0)
		{
			numProducers = atoi(argv[++i]);
		} else {
			usage();
		}
	}
}

/**
 * Single producer and single consumer
 */
int test_1p_1c_1thread()
{
	return 0;
}

int main(int argc, char *argv[])
{
	 int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
	 parseArgs(argc, argv);
	 printf("NC: %d\n", numCPU);
	 test_1p_1c_1thread();
}

