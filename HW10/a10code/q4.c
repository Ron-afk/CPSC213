#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

uthread_t t0, t1, t2;
uthread_mutex_t mx;
uthread_cond_t c0, c1, c2;
int nextNum = 0;

void randomStall() {
  int i, r = random() >> 16;
  while (i++<r);
}

void* p0(void* v) {
  randomStall();
  uthread_mutex_lock(mx);
  printf("zero\n");
  nextNum++;
  uthread_cond_broadcast(c0);
  uthread_mutex_unlock(mx);
  return NULL;
}

void* p1(void* v) {
  randomStall();
  uthread_mutex_lock(mx);
  while(nextNum < 1) {
    uthread_cond_wait(c0);
  }
  printf("one\n");
  nextNum++;
  uthread_cond_broadcast(c1);
  uthread_mutex_unlock(mx);
  return NULL;
}

void* p2(void* v) {
  randomStall();
  // uthread_cond_wait(c0);
  uthread_mutex_lock(mx);
  while(nextNum<2) {
    uthread_cond_wait(c1);
  }
  printf("two\n");
  nextNum++;
  uthread_cond_broadcast(c2);
  uthread_mutex_unlock(mx);
  return NULL;
}

int main(int arg, char** arv) {
  uthread_init(4);
  mx = uthread_mutex_create();
  c0 = uthread_cond_create(mx);
  c1 = uthread_cond_create(mx);
  c2 = uthread_cond_create(mx);
  t0 = uthread_create(p0, NULL);

  // uthread_cond_wait(c0);
  t1 = uthread_create(p1, NULL);
  // uthread_cond_wait(c1);
  t2 = uthread_create(p2, NULL);
  // uthread_cond_wait(c2);

 
  randomStall();
  
  // uthread_cond_wait(c1);
  // uthread_cond_wait(c2);
  uthread_join (t2, NULL);
  uthread_join (t1, NULL);
  uthread_join (t0, NULL);  
  uthread_mutex_lock(mx);
  while(nextNum < 3) {
    uthread_cond_wait(c2);
  }
  printf("three\n");
  uthread_mutex_unlock(mx);
  printf("------\n");
}
