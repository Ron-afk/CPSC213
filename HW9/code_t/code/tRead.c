#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/errno.h>
#include <assert.h>
#include "queue.h"
#include "disk.h"
#include "uthread.h"

queue_t      pending_read_queue;
unsigned int sum = 0;

void interrupt_service_routine () {
  // TODO
  void* val;
  // void (*callback)(void*,void*);
  queue_dequeue (pending_read_queue, &val, NULL, NULL);
  // callback (val, NULL);
  uthread_unblock(val);
}


void* read_block (void* blocknov) {
  // TODO schedule read and the update (correctly)
  int *np = blocknov;
  int num_blocks = *np;
  int result;
  
  queue_enqueue(pending_read_queue, uthread_self(), NULL, NULL);
  disk_schedule_read(&result, num_blocks);

  uthread_block();
  sum += result;
  return NULL;
}

int main (int argc, char** argv) {

  // Command Line Arguments
  static char* usage = "usage: tRead num_blocks";
  int num_blocks;
  char *endptr;
  if (argc == 2)
    num_blocks = strtol (argv [1], &endptr, 10);
  if (argc != 2 || *endptr != 0) {
    printf ("argument error - %s \n", usage);
    return EXIT_FAILURE;
  }

  // Initialize
  uthread_init (1);
  disk_start (interrupt_service_routine);
  pending_read_queue = queue_create();

  uthread_t t[num_blocks];
  int result[num_blocks];
  for(int i = 0; i < num_blocks; i++) {
    result[i] = i;
    t[i] = uthread_create(read_block, &result[i]);
  }

  // uthread_yield();

  for(int i = 0; i < num_blocks; i++) {
    // uthread_unblock(t[i]);
    uthread_join(t[i], 0);
  }

  // Sum Blocks
  // TODO
  // int sum = 0;
  // for(int i = 0; i < num_blocks; i++) {
  //   sum += *result[i];
  //   free(result[i]);
  // }

  
  printf("%d\n", sum);
}

