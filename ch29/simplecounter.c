#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct __counter_t {
  int value;
  pthread_mutex_t lock;
} counter_t;

void init(counter_t *c) {
  c->value = 0;
  pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c) {
  pthread_mutex_lock(&c->lock);
  c->value++;
  pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c) {
  pthread_mutex_lock(&c->lock);
  c->value--;
  pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c) {
  pthread_mutex_lock(&c->lock);
  int rc = c->value;
  pthread_mutex_unlock(&c->lock);
  return rc;
}

void *job(void *c) {
  counter_t *counter = (counter_t *) c; 
  for (int i = 0; i < 1000; ++i) {
    increment(counter);
    decrement(counter);
  }
  get(counter);
}


int main(int argc, char *argv[]){
  cpu_set_t cpu_set;
  CPU_ZERO(&cpu_set);
  CPU_SET(0, &cpu_set);
  sched_setaffinity(getpid(), sizeof(cpu_set), &cpu_set);

  int thread_num = atoi(argv[1]);
  pthread_t threads[thread_num];
  counter_t counter;
  init(&counter);

  struct timespec ts1;
  clock_gettime(CLOCK_MONOTONIC, &ts1);

  for(int i = 0; i < thread_num; ++i) {
    pthread_create(&threads[i], NULL, job, &counter);
  }

  for (int i = 0; i < thread_num; ++i) {
    pthread_join(threads[i], NULL);
  }

  struct timespec ts2;
  clock_gettime(CLOCK_MONOTONIC, &ts2);
  long span_sec = ts2.tv_sec - ts1.tv_sec;
  long span_nsec = ts2.tv_nsec - ts1.tv_nsec;
  printf("time: %10ld.%09ld\n", span_sec, span_nsec);
  return 0;
}
