
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <kafkalite.h>

#ifndef __KL_BENCHMARKS_H__
#define __KL_BENCHMARKS_H__

#define DELIM 	 ','

extern char *const contextDir;
extern int numMessages;
extern int numProducers;
extern int numConsumers;
extern KLContext *context;
extern int numTestFiles;
extern char *testFileNames;
extern size_t *testFileSizes;
extern char **testFiles;

void loadTestFiles(char *testFileNames);
void parseArgs(int argc, char *argv[]);

#endif


