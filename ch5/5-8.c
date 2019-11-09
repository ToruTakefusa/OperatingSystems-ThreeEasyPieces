#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "pipe() failed\n");
        exit(1);
    }

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork() failed\n");
        exit(1);
    } else if (rc == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        printf("aaaa");
    } else {
        dup2(fd[0], 0);
        close(fd[1]);
        char line[255];
        fgets(line, sizeof(line), stdin);
        printf("%s\n", line);
    }
    return 0;
}