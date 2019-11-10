#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/uio.h>

void systemCallChallenge(int fd);
void challengeFork();
double getDiffTime(struct timeval time1, struct timeval time2);

int main() {
    int fd1[2], fd2[2];

    int isError = (pipe(fd1) == -1) || (pipe(fd2) == -1);
    if (isError) {
        fprintf(stderr, "pipe() failed\n");
        exit(1);
    }

    struct timeval time1;
    struct timeval time2;
    int fd = open("foo.txt", O_RDWR);
    gettimeofday(&time1, NULL);

    const int SYS_CALL_COUNT = 30;
    for (int i = 0; i < SYS_CALL_COUNT ; i++ ) {
        systemCallChallenge(fd);
    }
    gettimeofday(&time2, NULL);
    double secSysCall = getDiffTime(time1, time2) / SYS_CALL_COUNT;
    printf("System call takes %lf sec\n", secSysCall);
    close(fd);

    const int CONTEXT_SWITCH_COUNT = 100;
    gettimeofday(&time1, NULL);
    for (int i = 0; i < CONTEXT_SWITCH_COUNT; i++ ) {
        challengeFork();
    }
    gettimeofday(&time2, NULL);
    double secContextSwitch = getDiffTime(time1, time2) / (2 * CONTEXT_SWITCH_COUNT);
    printf("Context switch takes %lf sec\n", secContextSwitch);
    return 0;
}

/*
* Do read() system call, but actually read nothing.
*/
void systemCallChallenge(int fd) {
    read(fd, NULL, 0);
}

void challengeFork() {
    int fd1[2], fd2[2];
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork() failed\n");
    } else if (rc == 0) {
        int count = 0;
        // change fd1[1] standard output
        dup2(fd1[1], 1);
        // change fd2[0] standard input
        dup2(fd2[0], 0);
        close(fd1[0]);
        close(fd2[1]);
        printf("\0");
        char line[3];
        fgets(line, sizeof(line), stdin);
        exit(0);
    } else {
        int count = 0;
        // change fd1[0] standard input
        dup2(fd1[0], 0);
        // cchange fd2[1] standard output
        dup2(fd2[1], 1);
        close(fd1[1]);
        close(fd2[0]);

        printf("\0");
        return;
    }

}

/*
* Get the time between time1 and time2 in second(time2-time1). time2 must be later than time1.
*/
double getDiffTime(struct timeval time1, struct timeval time2) {
    return time2.tv_sec - time1.tv_sec + (double)(time2.tv_usec - time1.tv_usec) / 1000000;
}