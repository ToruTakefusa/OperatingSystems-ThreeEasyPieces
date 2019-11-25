#include <stdlib.h>
#include <stdio.h>

int main() {
  int *p = malloc(100);
  free(p);
  printf("data[0] is %d\n", p[0]);
}
