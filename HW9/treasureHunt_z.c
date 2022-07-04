#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <assert.h>
#include "uthread.h"
#include "queue.h"
#include "disk.h"

queue_t pending_read_queue;
volatile int pending_reads;
unsigned int value = 0;

void interrupt_service_routine() {
  // TODO
  void *val;
  void *count;
  void (*callback)(void*,void*);
  queue_dequeue(pending_read_queue, &val, &count, &callback);
  callback(val, count);
  pending_reads--;
}

void handleOtherReads(void *resultv, void *countv) {
  // TODO
  printf("%s\n","here");
  int *val = resultv;
  int *count = countv;
  
  for (int i = 0; i < *count; i++){
    queue_enqueue(pending_read_queue, val, count, handleOtherReads);
    disk_schedule_read(val, *val);
  }

  value += *val;    
  
}

void handleFirstRead(void *resultv, void *countv) {
  // TODO
  int *val = resultv;
  int *count = countv;
  *count = *val;
  if (*count == 0){
    printf("%d\n", *val);
  } else {
    queue_enqueue(pending_read_queue, val, count, handleOtherReads);
    disk_schedule_read(val, *val);
  }
  printf("%s\n","first");
}

int main(int argc, char **argv) {
  // Command Line Arguments
  static char* usage = "usage: treasureHunt starting_block_number";
  int starting_block_number;
  char *endptr;
  if (argc == 2)
    starting_block_number = strtol (argv [1], &endptr, 10);
  if (argc != 2 || *endptr != 0) {
    printf ("argument error - %s \n", usage);
    return EXIT_FAILURE;
  }

  // Initialize
  uthread_init (1);
  disk_start (interrupt_service_routine);
  pending_read_queue = queue_create();

  // Start the Hunt
  // TODO
  int result;
  int count;
  queue_enqueue(pending_read_queue, &result, &count, handleFirstRead);
  disk_schedule_read(&result, starting_block_number);
  
  while (pending_reads > 0); // infinite loop so that main doesn't return before hunt completes
  printf ("%d\n", value);
}
