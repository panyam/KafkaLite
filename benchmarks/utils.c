
#include "benchmark.h"

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
