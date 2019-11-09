#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
    int fd = open("foo.txt", O_WRONLY);
    int rc = fork();
    
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        char str1[] = "aaaa";
        write(fd, str1, sizeof(str1));
    } else {
        char str2[] = "bbb";
        write(fd, str2, sizeof(str2));
    }
    close(fd);
    return 0;
}