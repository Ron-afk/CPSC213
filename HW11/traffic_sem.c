#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include "uthread.h"
#include "uthread_sem.h"

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
#else
#define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
#endif

#define MAX_OCCUPANCY  3
#define NUM_ITERATIONS 100
#define NUM_CARS       20

// These times determine the number of times yield is called when in
// the street, or when waiting before crossing again.
#define CROSSING_TIME             NUM_CARS
#define WAIT_TIME_BETWEEN_CROSSES NUM_CARS

/**
 * You might find these declarations useful.
 */
enum Direction {EAST = 0, WEST = 1};
const static enum Direction oppositeEnd [] = {WEST, EAST};

struct Street {
  uthread_sem_t mut;
  uthread_sem_t west;
  uthread_sem_t east;
  int dirCount;
  int totalCount;
  enum Direction dir;
} Street;

void initializeStreet(void) {
  // TODO
  Street.mut = uthread_sem_create(1);
  Street.east = uthread_sem_create(3);
  Street.west = uthread_sem_create(0);
  Street.dirCount = 0;
  Street.totalCount = 0;
  Street.dir = 0;
}

#define WAITING_HISTOGRAM_SIZE (NUM_ITERATIONS * NUM_CARS)
int             entryTicker;                                          // incremented with each entry
int             waitingHistogram [WAITING_HISTOGRAM_SIZE];
int             waitingHistogramOverflow;
uthread_sem_t   waitingHistogramLock;
int             occupancyHistogram [2] [MAX_OCCUPANCY + 1];

void recordWaitingTime (int waitingTime) {
  uthread_sem_wait (waitingHistogramLock);
  if (waitingTime < WAITING_HISTOGRAM_SIZE)
    waitingHistogram [waitingTime] ++;
  else
    waitingHistogramOverflow ++;
  uthread_sem_signal (waitingHistogramLock);
}

void enterStreet (enum Direction g) {
  // TODO
  uthread_sem_wait(Street.mut);
    if(Street.dirCount == 0) {
      Street.dir = g;
      Street.dirCount++;
      Street.totalCount++;
      occupancyHistogram[g][Street.dirCount]++;
  // VERBOSE_PRINT("street enter finish\n");
    uthread_sem_signal(Street.mut);
    } else {
      int c = Street.totalCount;
      uthread_sem_t sig;
      if(g == WEST) {
        sig = Street.west;
      } else {
        sig = Street.east;
      }
      
      while(g != Street.dir || Street.dirCount >= MAX_OCCUPANCY) {
        uthread_sem_signal(Street.mut);
        uthread_sem_wait(sig);
        uthread_sem_wait(Street.mut);
      }
      
      recordWaitingTime(Street.totalCount - c);
      
      Street.dirCount++;
      Street.totalCount++;
      occupancyHistogram[g][Street.dirCount]++;
      uthread_sem_signal(Street.mut);
    }
  
}

void leaveStreet(void) {
  // TODO
  uthread_sem_wait(Street.mut);
  // VERBOSE_PRINT("street leave\n");
    Street.dirCount--;
    if(Street.dirCount == 0) {
      // enum Direction nextDir = 1 - Street.dir;
      uthread_sem_t sig;
      if(Street.dir != 0) {
        sig = Street.east;
      Street.dir = 0;
      } else {
        sig = Street.west;
        Street.dir = 1;
      }
      for(int i = 0; i < MAX_OCCUPANCY; i++) {
        uthread_sem_signal(sig);
      }
    }
    
  uthread_sem_signal(Street.mut);
}



//
// TODO
// You will probably need to create some additional procedures etc.
//
void* car(void* unuse) {
  enum Direction dir = random() % 2;
  for(int i = 0; i < NUM_ITERATIONS; i++) {
    enterStreet(dir);
    for(int i = 0; i < NUM_CARS; i++) {
      uthread_yield();
    }
    leaveStreet();
    for(int i = 0; i < NUM_CARS; i++) {
      uthread_yield();
    }
  } 
  return 0; 
}

int main (int argc, char** argv) {
  
  uthread_init(8);

  waitingHistogramLock = uthread_sem_create(1);

  initializeStreet();
  uthread_t pt [NUM_CARS];

  // TODO
  for(int i = 0; i < NUM_CARS; i++) {
    pt[i] = uthread_create(car, NULL);
  }
  for(int i = 0; i < NUM_CARS; i++) {
    uthread_join(pt[i], NULL);
  }
  
  printf ("Times with 1 car  going east: %d\n", occupancyHistogram [EAST] [1]);
  printf ("Times with 2 cars going east: %d\n", occupancyHistogram [EAST] [2]);
  printf ("Times with 3 cars going east: %d\n", occupancyHistogram [EAST] [3]);
  printf ("Times with 1 car  going west: %d\n", occupancyHistogram [WEST] [1]);
  printf ("Times with 2 cars going west: %d\n", occupancyHistogram [WEST] [2]);
  printf ("Times with 3 cars going west: %d\n", occupancyHistogram [WEST] [3]);
  
  printf ("Waiting Histogram\n");
  for (int i=0; i < WAITING_HISTOGRAM_SIZE; i++)
    if (waitingHistogram [i])
      printf ("  Cars waited for           %4d car%s to enter: %4d time(s)\n",
	      i, i==1 ? " " : "s", waitingHistogram [i]);
  if (waitingHistogramOverflow)
    printf ("  Cars waited for more than %4d cars to enter: %4d time(s)\n",
	    WAITING_HISTOGRAM_SIZE, waitingHistogramOverflow);
}
