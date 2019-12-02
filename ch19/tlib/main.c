#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    // In my environment getconf PAGESIZE = 4096
    const int PAGESIZE = 4096;
    const long long NUM_OF_PAGES = atoi(argv[1]);
    const int NUM_OF_TRIALS = atoi(argv[2]);
    int jump = PAGESIZE / sizeof (int);
    long long a[NUM_OF_PAGES * jump];
    struct timeval start;
    struct timeval end;
    long long int diff[NUM_OF_PAGES];

    for (long long i = 0; i < NUM_OF_TRIALS; ++i) {
        for (long long j = 0; j < NUM_OF_PAGES * jump; j += jump) {
            gettimeofday(&start, NULL);
            a[j] += 1;
            // If we don't use usleep, diff will be 0
            usleep(100);
            gettimeofday(&end, NULL);
            long long int diff_time = end.tv_usec - start.tv_usec;
            diff[j] += diff_time;
        }
    }

    long long answer = 0;
    for (long long i = 0; i < NUM_OF_PAGES * jump; i += jump) {
        diff[i] = diff[i] / NUM_OF_TRIALS;
        answer += diff[i];
    }

    printf("%lld\n", answer / NUM_OF_PAGES);
}
