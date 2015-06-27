
#include "benchmark.h"

char *const contextDir = "/tmp/kafka";
int numMessages = 1000000;
int numProducers = 1;
int numConsumers = 1;
KLContext *context;
int numTestFiles = 0;
char *testFileNames="messages/0.txt,messages/1.txt,messages/2.txt,messages/3.txt,messages/4.txt,"
						   "messages/5.txt,messages/6.txt,messages/7.txt,messages/8.txt,messages/9.txt";
size_t *testFileSizes = NULL;
char **testFiles = NULL;

void setup()
{
	rmdirs(contextDir);
	context = kl_context_open(contextDir, NULL);
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
	 loadTestFiles(testFileNames);
	 printf("NC: %d\n", numCPU);
	 test_1p_1c_1thread();
}

