
#include "klcontexttests.h"
#include "kltopictests.h"
#include "kliteratortests.h"

int main(void)
{
	return run_context_tests() +
			run_topic_tests() +
			run_iterator_tests();
}
