#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>

int main(int argc, char *argv[]) {
    // In my environment getconf PAGESIZE = 4096
    const int PAGESIZE = 4096;
    const long long NUM_OF_PAGES = atoi(argv[1]);
    const int NUM_OF_TRIALS = atoi(argv[2]);
    int jump = PAGESIZE / sizeof (int);
    int *to_free, *a;
    to_free = a = malloc(NUM_OF_PAGES * jump * sizeof (int));
    struct timeval start;
    struct timeval end;
    long long int diff[NUM_OF_PAGES * jump];

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(0, &cpu_set);
    sched_setaffinity(getpid(), sizeof (cpu_set), &cpu_set);

    for (long long i = 0; i < NUM_OF_PAGES * jump; ++i) a[i] = 0;

    for (long long i = 0; i < NUM_OF_TRIALS; ++i) {
        for (long long j = 0; j < NUM_OF_PAGES * jump; j += jump) {
            gettimeofday(&start, NULL);
            a[j] += 1;
            // If we don't use usleep, diff will be 0
            usleep(100);
            gettimeofday(&end, NULL);
            long long int diff_time = (end.tv_sec - start.tv_sec) * 1000000
                    + (end.tv_usec - start.tv_usec);
            diff[j] += diff_time;
        }
    }

    long long answer = 0;
    for (long long i = 0; i < NUM_OF_PAGES * jump; i += jump) {
        diff[i] = diff[i] / NUM_OF_TRIALS - 100;
        answer += diff[i];
    }

    printf("%lld\n", answer / NUM_OF_PAGES);
    free(to_free);
}
