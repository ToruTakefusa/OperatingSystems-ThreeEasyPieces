#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main() {
    int pid = getpid();
    int rc = fork();

    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        int w = waitpid(pid, NULL, WUNTRACED);
        printf("waitpid returns %d\n", w);
        printf("errorno is %d: %s\n", errno, strerror(errno));
    } else {

    }
    return 0;
}