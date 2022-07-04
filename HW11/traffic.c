#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include "uthread.h"
#include "uthread_mutex_cond.h"

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
  // TODO
  uthread_mutex_t mut;
  uthread_cond_t east;
  uthread_cond_t west;
  int dirCount;
  int totalCount;
  enum Direction dir;
} Street;

void initializeStreet(void) {
  // TODO
  Street.mut = uthread_mutex_create();
  Street.east = uthread_cond_create(Street.mut);
  Street.west = uthread_cond_create(Street.mut);
  Street.dirCount = 0;
  Street.totalCount= 0;
  Street.dir = 0;
}

#define WAITING_HISTOGRAM_SIZE (NUM_ITERATIONS * NUM_CARS)
int             entryTicker;                                          // incremented with each entry
int             waitingHistogram [WAITING_HISTOGRAM_SIZE];
int             waitingHistogramOverflow;
uthread_mutex_t waitingHistogramLock;
int             occupancyHistogram [2] [MAX_OCCUPANCY + 1];

void recordWaitingTime (int waitingTime) {
  uthread_mutex_lock (waitingHistogramLock);
  if (waitingTime < WAITING_HISTOGRAM_SIZE)
    waitingHistogram [waitingTime] ++;
  else
    waitingHistogramOverflow ++;
  uthread_mutex_unlock (waitingHistogramLock);
}

void enterStreet (enum Direction g) {
  // TODO
  uthread_mutex_lock(Street.mut);
    if (Street.dirCount == 0 || (g == Street.dir && Street.dirCount < MAX_OCCUPANCY)) {
      Street.dir = g;
    } else {
      int c = Street.totalCount;
      uthread_cond_t sig;
      if(g == WEST) {
        sig = Street.west;
      } else {
        sig = Street.east;
      }
      while(g != Street.dir || Street.dirCount >= MAX_OCCUPANCY) {
        uthread_cond_wait(sig);
      }
      recordWaitingTime(Street.totalCount - c);  
    }
    Street.dirCount++;
    Street.totalCount++;
    occupancyHistogram[g][Street.dirCount]++;
  uthread_mutex_unlock(Street.mut);

}

void leaveStreet(void) {
  // TODO
  uthread_mutex_lock(Street.mut);
    Street.dirCount--;
    if(Street.dirCount == 0) {
      uthread_cond_t sig;
      if(Street.dir != 0) {
        sig = Street.east;
        Street.dir = 0;
      } else {
        sig = Street.west;
        Street.dir = 1;
      }
      // for(int i = 0; i < MAX_OCCUPANCY; i++) {
        uthread_cond_broadcast(sig);
        // uthread_yield();
      // }
    }
  uthread_mutex_unlock(Street.mut);
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

  waitingHistogramLock = uthread_mutex_create();
  
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

  printf("total car passes %i\n",Street.totalCount);
  
  printf ("Waiting Histogram\n");
  for (int i=0; i < WAITING_HISTOGRAM_SIZE; i++)
    if (waitingHistogram [i])
      printf ("  Cars waited for           %4d car%s to enter: %4d time(s)\n",
	      i, i==1 ? " " : "s", waitingHistogram [i]);
  if (waitingHistogramOverflow)
    printf ("  Cars waited for more than %4d cars to enter: %4d time(s)\n",
	    WAITING_HISTOGRAM_SIZE, waitingHistogramOverflow);
}
