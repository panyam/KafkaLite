
#include "benchmark.h"

void setup(Benchmark *bm)
{
    loadTestData(bm);
    kl_rmdirs(bm->contextDir);
    bm->context = kl_context_open(bm->contextDir, bm->numThreads == 0 ? NULL : kl_pthread_lock_manager());
    bm->topic = kl_topic_open(bm->context, "topic");
}

void test_single_thread(Benchmark *bm);
void test_multiple_threads(Benchmark *bm);

int main(int argc, char *argv[])
{
    //int numCPU = sysconf( _SC_NPROCESSORS_ONLN ); printf("NC: %d\n", numCPU);
    Benchmark *benchmark = calloc(1, sizeof(Benchmark));;
    parseArgs(benchmark, argc, argv);
    setup(benchmark);

    long long beforeTime = kl_current_timestamp();
    if (benchmark->numThreads < 1)
    {
        test_single_thread(benchmark);
    } else {
        test_multiple_threads(benchmark);
    }
    long long afterTime = kl_current_timestamp();
    kl_log("\nElapsed Time for %d messages: %lld\n", benchmark->numMessages, afterTime - beforeTime);
}
