#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    int x = 100;
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("x is %d\n", x);
        x = 200;
        printf("x is now %d\n", x);
    } else {
        printf("x is %d\n", x);
        x = 300;
        printf("x is %d\n", x);
    }
    return 0;
}