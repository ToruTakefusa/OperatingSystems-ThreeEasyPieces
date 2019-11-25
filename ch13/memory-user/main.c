#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    const long SIZE = (int)strtol(argv[1], NULL, 10) / 4;
    long arrays[SIZE];

    long long int count = 0;

    while(1) {
        for (int i = 0; i < SIZE; ++i) {
            arrays[i] = count;
            ++count;
        }
    }
}
