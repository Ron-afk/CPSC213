#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

const int N = 100000;
int cnt = 0;

uthread_t t0, t1, t2;
uthread_mutex_t mut;

void countUp() {
  for (int i=0; i<N; i++){
    uthread_mutex_lock(mut);
    cnt += 1;
    uthread_mutex_unlock(mut);
  }

    
}

void countDown() {
  for (int i=0; i<N; i++) {
    uthread_mutex_lock(mut);
    cnt -= 1;
    uthread_mutex_unlock(mut);
  }
  
}

void* count(void* v) {
  countUp();
  countDown();
  return NULL;
}

int main(int arg, char** arv) {
  uthread_init(4);
  mut = uthread_mutex_create();
  t0 = uthread_create(count, NULL);
  t1 = uthread_create(count, NULL);
  t2 = uthread_create(count, NULL);
  
  uthread_join (t0, NULL);
  uthread_join (t1, NULL);
  uthread_join (t2, NULL);
  printf("cnt = %d %s\n", cnt, cnt==0? "": "XXX");
}