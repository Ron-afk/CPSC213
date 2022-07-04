#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <assert.h>
#include "uthread.h"
#include "queue.h"
#include "disk.h"

queue_t pending_read_queue;

void interrupt_service_routine()
{
  // TODO
  void *val;
  void *count;
  void (*callback)(void *, void *);
  queue_dequeue(pending_read_queue, &val, &count, &callback);
  // printf("%d", *(int *)count);
  callback(val, count);
}

void handleOtherReads(void *resultv, void *countv)
{
  // TODO
  int *val = (int *)resultv;
  // int val = 0;
  int *count = (int *)countv;
  *count -=1;
  if (*count == 0) {
    int result = *val;
    printf("%d\n", result);
    exit(EXIT_SUCCESS);
  } else {
    queue_enqueue(pending_read_queue, val, count, handleOtherReads);
    disk_schedule_read(val, *val);
  }
}

void handleFirstRead(void *resultv, void *countv)
{
  // TODO
  int *val = (int *)resultv;
  int *count = (int *)countv;
  *count = *val;
  if(*count) {
    queue_enqueue(pending_read_queue, val, count, handleOtherReads);
    disk_schedule_read(val, *val);
  } else {
    printf("%d\n",*val);
    exit(EXIT_SUCCESS);
  }
  
}

int main(int argc, char **argv)
{
  // Command Line Arguments
  static char *usage = "usage: treasureHunt starting_block_number";
  int starting_block_number;
  char *endptr;
  if (argc == 2)
    starting_block_number = strtol(argv[1], &endptr, 10);
  if (argc != 2 || *endptr != 0)
  {
    printf("argument error - %s \n", usage);
    return EXIT_FAILURE;
  }

  // Initialize
  uthread_init(1);
  disk_start(interrupt_service_routine);
  pending_read_queue = queue_create();

  // Start the Hunt
  // TODO
  int result_read = 0;
  int count;
  queue_enqueue(pending_read_queue, &result_read, &count, handleFirstRead);
  disk_schedule_read(&result_read, starting_block_number);
  while (1)
    ; // infinite loop so that main doesn't return before hunt completes
}
