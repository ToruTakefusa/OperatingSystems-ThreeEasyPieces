#include <stdio.h>

#include "mythreads.h"

int balance = 0;
pthread_mutex_t lock;

void* worker(void* arg) {
  pthread_mutex_lock(&lock);
  balance++; // unprotected access
  pthread_mutex_unlock(&lock);
  return NULL;
}

int main(int argc, char *argv[]) {
  Pthread_mutex_init(&lock, NULL);
  pthread_t p;
  Pthread_create(&p, NULL, worker, NULL);

  Pthread_mutex_lock(&lock);
  balance++; // unprotected access
  Pthread_mutex_unlock(&lock);

  pthread_join(p, NULL);
  pthread_mutex_destroy(&lock);
  return 0;
}
