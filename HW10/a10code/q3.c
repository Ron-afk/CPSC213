#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

#define NUM_THREADS 5
uthread_t threads[NUM_THREADS];
uthread_mutex_t mut;
uthread_cond_t c;
int isAFinished = 0;


void randomStall() {
  int i, r = random() >> 16;
  while (i++<r);
}

void waitForAllOtherThreads() {
  uthread_mutex_lock(mut);
  isAFinished++;
  
  while(isAFinished < NUM_THREADS) {
    uthread_cond_wait(c);
  } 
  uthread_cond_broadcast(c);
  
  uthread_mutex_unlock(mut);
}

void* p(void* v) {
  randomStall();
  printf("a\n");
  waitForAllOtherThreads();
  printf("b\n");
  return NULL;
}

int main(int arg, char** arv) {
  uthread_init(4);
  mut = uthread_mutex_create();
  c = uthread_cond_create(mut);
  for (int i=0; i<NUM_THREADS; i++)
    threads[i] = uthread_create(p, NULL);
  for (int i=0; i<NUM_THREADS; i++)
    uthread_join (threads[i], NULL);
  printf("------\n");
}