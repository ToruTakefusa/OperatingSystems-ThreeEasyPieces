#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#define NUMCPUS 2

typedef struct __counter_t {
  int global;
  pthread_mutex_t glock;
  int local[NUMCPUS];
  pthread_mutex_t llock[NUMCPUS];
  int threshold;
} counter_t;

void init(counter_t *c, int threshold) {
  c->threshold = threshold;
  c->global = 0;
  pthread_mutex_init(&c->glock, NULL);
  int i;
  for (int i = 0; i < NUMCPUS; ++i) {
    c->local[i] = 0;
    pthread_mutex_init(&c->llock[i], NULL);
  }
}

void update(counter_t *c, int threadID, int amt){
  int cpu = threadID % NUMCPUS;
  pthread_mutex_lock(&c->llock[cpu]);
  c->local[cpu] += amt;
  if (c->local[cpu] >= c->threshold) {
    pthread_mutex_lock(&c->glock);
    c->global += c->local[cpu];
    pthread_mutex_unlock(&c->glock);
    c->local[cpu] = 0;
  }
  pthread_mutex_unlock(&c->llock[cpu]);
}

int get(counter_t *c) {
  pthread_mutex_lock(&c->glock);
  int val = c->global;
  pthread_mutex_unlock(&c->glock);
  return val;
}

void *job(void *c) {
  counter_t *counter = (counter_t *)c;
  for (int i = 0; i < 1000; ++i) {
    int tmp = get(counter);
    update(counter, syscall(SYS_gettid), ++tmp);
  }
}

int main(int argc, char *argv[]) {
  int thread_num = atoi(argv[1]);
  pthread_t threads[thread_num];
  int threshold = atoi(argv[2]);
  counter_t counter;
  init(&counter, threshold);

  struct timespec ts1;
  clock_gettime(CLOCK_MONOTONIC, &ts1);

  for (int i = 0; i < thread_num; ++i) {
    pthread_create(&threads[i], NULL, job, &counter);
  }

  for (int i = 0; i < thread_num; ++i) {
    pthread_join(threads[i], NULL);
  }

  struct timespec ts2;
  clock_gettime(CLOCK_MONOTONIC, &ts2);
  long long timespan = (ts2.tv_sec * 1000000000 + ts2.tv_nsec) - (ts1.tv_sec * 1000000000 + ts1.tv_nsec);
  printf("time: %lld\n", timespan);
  return 0;
}
