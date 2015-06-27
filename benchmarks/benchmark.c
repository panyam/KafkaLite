
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <kafkalite.h>

#define DELIM 	 ','
static char *const contextDir = "/tmp/kafka";
static int numMessages = 1000000;
static int numProducers = 1;
static int numConsumers = 1;
static KLContext *context;
static int numTestFiles = 0;
static char *testFileNames="messages/0.txt,messages/1.txt,messages/2.txt,messages/3.txt,messages/4.txt,"
						   "messages/5.txt,messages/6.txt,messages/7.txt,messages/8.txt,messages/9.txt";
static size_t *testFileSizes = NULL;
static char **testFiles = NULL;

void loadTestFiles(char *testFileNames)
{
	char *token, *tofree, *string;
	tofree = string = strdup(testFileNames);

	int numCommas = 0;
	for (char *tmp = testFileNames;*tmp;tmp++)
		if (*tmp == DELIM)
			numCommas++;

	numTestFiles = 0;
	testFileSizes = (size_t *)malloc(sizeof(size_t) * (numCommas + 1));
	testFiles = (char **)malloc(sizeof(char *) * (numCommas + 1));
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
				testFileSizes[numTestFiles] = fileStat.st_size;
				testFiles[numTestFiles] = (char *)malloc(fileStat.st_size);
				read(fd, testFiles[numTestFiles], fileStat.st_size);
				close(fd);
				numTestFiles++;
			}
		}
	}
	free(tofree);
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
		} else if (strcmp(argv[i], "-t") == 0)
		{
			testFileNames = argv[++i];
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
	 loadTestFiles(testFileNames);
	 printf("NC: %d\n", numCPU);
	 test_1p_1c_1thread();
}

