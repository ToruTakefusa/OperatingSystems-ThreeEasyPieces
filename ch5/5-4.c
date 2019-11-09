#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int rc = fork();
    
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        char *args[2];
        args[0] = strdup("/bin/ls");
        args[1] = NULL;
        //exec(args[0], args);
        //execl(args[0], args);
        //execle(args[0], args);
        //execlp(args[0], args);
        //execv(args[0], args);
        execvp(args[0], args);
    } 
    return 0;
}