#include <stdlib.h>
#include <stdio.h>

int main() {
  int *p = malloc(100);
  free(&p[10]);

}
