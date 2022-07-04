#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

#define MAX_THINKING_TIME 25000

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
#else
#define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
#endif

typedef struct fork {
  uthread_mutex_t lock;
  uthread_cond_t forfree;
  long free;
} fork_t;

int num_phils, num_meals;    
fork_t *forks;

void deep_thoughts() {
  usleep(random() % MAX_THINKING_TIME);
}

void initfork(int i) {
  forks[i].lock    = uthread_mutex_create();
  forks[i].forfree = uthread_cond_create(forks[i].lock);
  forks[i].free    = 1;
  // VERBOSE_PRINT("fork created%d\n",i);
}

int leftfork(long i) {
  return i;
}

int rightfork(long i) {
  return (i + 1) % num_phils;
}

long getfork(long i) {
  /* TO BE IMPLEMENTED BY THE STUDENTS. */
  // VERBOSE_PRINT("get fork for %li\n",i);
  
  int leftfork_id = leftfork(i);
  int rightfork_id = rightfork(i);
  deep_thoughts();
  uthread_mutex_lock(forks[rightfork_id].lock);
  uthread_mutex_lock(forks[leftfork_id].lock);
  // if(forks[rightfork_id].free || forks[leftfork_id].free) {
    // uthread_mutex_lock(forks[rightfork_id].lock);
  // if(forks[rightfork_id].free) {
  //   forks[rightfork_id].free = 0;
  //   // VERBOSE_PRINT("get rightfork for %li\n",i);
  // } else {
  while(!forks[rightfork_id].free) {
      // VERBOSE_PRINT("wait rightfork for %li\n",i);
    uthread_cond_wait(forks[rightfork_id].forfree);
  }
  forks[rightfork_id].free = 0;
  deep_thoughts();
  // }
  while(!forks[leftfork_id].free) {
      // VERBOSE_PRINT("wait leftfork for %li\n",i);
      uthread_cond_wait(forks[leftfork_id].forfree);
    }
  forks[leftfork_id].free = 0;
  // VERBOSE_PRINT("get leftfork for %li\n",i);
  uthread_mutex_unlock(forks[rightfork_id].lock);
  uthread_mutex_unlock(forks[leftfork_id].lock); 
  return 1;
}

void putfork(long i) {
  /* TO BE IMPLEMENTED BY THE STUDENTS. */
  int leftfork_id = leftfork(i);
  int rightfork_id = rightfork(i);
  uthread_mutex_lock(forks[leftfork_id].lock);
    forks[leftfork_id].free = 1;
    uthread_cond_signal(forks[leftfork_id].forfree);
  uthread_mutex_unlock(forks[leftfork_id].lock);

  uthread_mutex_lock(forks[rightfork_id].lock);
    forks[rightfork_id].free = 1;
    uthread_cond_signal(forks[rightfork_id].forfree);
  uthread_mutex_unlock(forks[rightfork_id].lock);
}


void *phil_thread(void *arg) {
  uintptr_t id = (uintptr_t) arg;
  int meals = 0;
  
  while (meals < num_meals) {
    /* TO BE IMPLEMENTED BY THE STUDENTS. */
    
    // VERBOSE_PRINT("phil %li try to pick up forks\n",id);
    if(getfork((long)id)){
      // VERBOSE_PRINT("phil %li start to eat\n",id);
      meals++;
      putfork((long)id);
      // VERBOSE_PRINT("phil finish eat\n");
      deep_thoughts();
    }    
  }
  VERBOSE_PRINT("phil %li finished\n", id);
  return 0;
}

int main(int argc, char **argv) {

  uthread_t *p;
  uintptr_t i;
  
  if (argc != 3) {
    fprintf(stderr, "Usage: %s num_philosophers num_meals\n", argv[0]);
    return 1;
  }

  num_phils = strtol(argv[1], 0, 0);
  num_meals = strtol(argv[2], 0, 0);
  
  forks = malloc(num_phils * sizeof(fork_t));
  p = malloc(num_phils * sizeof(uthread_t));

  uthread_init(num_phils);
  
  srandom(time(0));
  for (i = 0; i < num_phils; ++i) {
    initfork(i);
  }

  /* TODO: Create num_phils threads, all calling phil_thread with a
   * different ID, and join all threads.
   */
  // VERBOSE_PRINT("thread init\n");
  for(long j = 0; j < num_phils; j++) {
    void *id = (void*) j;
    // VERBOSE_PRINT("thread created%i\n",j);
    p[j] = uthread_create(phil_thread,id);
  }

  for(int j = 0; j < num_phils; j++) {
    
    uthread_join(p[j],0);
  }
  
  return 0;
}
