#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main() {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        int w = wait(NULL);
        printf("wait() returns %d\n", w);
        printf("errorno is %d: %s\n", errno, strerror(errno));
    } else {

    }
    return 0;
}