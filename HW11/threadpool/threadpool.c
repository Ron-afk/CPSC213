#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include "uthread.h"
#include "uthread_mutex_cond.h"
#include "uthread_sem.h"

#include "threadpool.h"

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
#else
#define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
#endif

struct task {
  void (*process) (struct tpool* pool, void* arg);
  void *arg;
  struct task* next;
};                 

struct tpool {

  /* TO BE COMPLETED BY THE STUDENT */
  uthread_mutex_t mut;
  uthread_sem_t queue_lock;
  uthread_cond_t working_lock;
  struct task* queue_head;
  int finished;
  uthread_t* thread;
  int max_working;
  int queue_size;
  int index;
};

/* Function executed by each pool worker thread. This function is
 * responsible for running individual tasks. The function continues
 * running as long as either the pool is not yet joined, or there are
 * unstarted tasks to run. If there are no tasks to run, and the pool
 * has not yet been joined, the worker thread must be blocked.
 * 
 * Parameter: param: The pool associated to the thread.
 * Returns: nothing.
 */
static void *worker_thread(void *param) {
  tpool_t pool = param;

  /* TO BE COMPLETED BY THE STUDENT */
  VERBOSE_PRINT("task excuting\n");
  
  while(1) {
    struct task* curr_work = NULL;
    
    uthread_mutex_lock(pool -> mut);
    while(pool->queue_size == 0 && !pool->finished) {
      VERBOSE_PRINT("thread wait\n");
      uthread_cond_wait(pool->working_lock);
    }
    // VERBOSE_PRINT("bp func excuting\n");
    // !pool->finished ||
    if(pool-> queue_size > 0) {
      pool -> queue_size--;
      uthread_sem_wait(pool->queue_lock);
      curr_work = pool->queue_head;
      pool->queue_head = curr_work -> next;
      uthread_sem_signal(pool-> queue_lock);
      curr_work -> next = NULL;

      uthread_mutex_unlock(pool -> mut);
      // VERBOSE_PRINT("bp func excuting\n");
      (*(curr_work -> process))(pool, curr_work -> arg);
      free(curr_work);
      curr_work = NULL;
    } else {
      uthread_mutex_unlock(pool->mut);
      break;
    }
  }

  VERBOSE_PRINT("task excuted\n");
  return NULL;
}

/* Creates (allocates) and initializes a new thread pool. Also creates
 * `num_threads` worker threads associated to the pool, so that
 * `num_threads` tasks can run in parallel at any given time.
 *
 * Parameter: num_threads: Number of worker threads to be created.
 * Returns: a pointer to the new thread pool object.
 */
tpool_t tpool_create(unsigned int num_threads) {

  /* TO BE COMPLETED BY THE STUDENT */
  VERBOSE_PRINT("pool creating\n");
  struct tpool* pool = malloc(sizeof(pool));
  pool -> queue_head = NULL;
  pool -> max_working = num_threads;
  pool -> mut = uthread_mutex_create(); 
  pool -> working_lock = uthread_cond_create(pool -> mut);
  pool -> queue_lock = uthread_sem_create(1);
  
  pool -> queue_size = 0;
  pool -> finished = 0;
  pool -> thread =  malloc(num_threads * sizeof(uthread_t));
  for(int i = 0; i < num_threads; i++) {
    pool -> thread[i] = uthread_create(worker_thread, pool);
  }
  VERBOSE_PRINT("pool created\n");
  return pool;
}

/* Queues a new task, to be executed by one of the worker threads
 * associated to the pool. The task is represented by function `fun`,
 * which receives the pool and a generic pointer as parameters. If any
 * of the worker threads is available, `fun` is started immediately by
 * one of the worker threads. If all of the worker threads are busy,
 * `fun` is scheduled to be executed when a worker thread becomes
 * available. Tasks are retrieved by individual worker threads in the
 * order in which they are scheduled, though due to the nature of
 * concurrency they may not start exactly in the same order. This
 * function returns immediately, and does not wait for `fun` to
 * complete.
 *
 * Parameters: pool: the pool that is expected to run the task.
 *             fun: the function that should be executed.
 *             arg: the argument to be passed to fun.
 */

void enqueue(struct task *t, tpool_t p) {
   if(p -> queue_head == NULL) {
      VERBOSE_PRINT("bp 2\n");

      p -> queue_head = t;
    } else {
      struct task* tailWork = NULL;
      tailWork = p -> queue_head;

      while(tailWork -> next != NULL) {
        tailWork = tailWork -> next;
      VERBOSE_PRINT("bp 3\n");
      }
      
      tailWork -> next = t;
      
    }
    p -> queue_size++;
}


void tpool_schedule_task(tpool_t pool, void (*fun)(tpool_t, void *),
                         void *arg) {
      
  /* TO BE COMPLETED BY THE STUDENT */
  VERBOSE_PRINT("task adding\n");
  struct task* newwork = (struct task*) malloc(sizeof(struct task));
  newwork -> process = fun;
  newwork -> arg = arg;
  newwork -> next = NULL;
  uthread_mutex_lock(pool -> mut);
    // VERBOSE_PRINT("bp 1\n");
    uthread_sem_wait(pool->queue_lock);
    enqueue(newwork, pool);
    uthread_sem_signal(pool -> queue_lock);

    // if(pool -> queue_head == NULL) {
    //   VERBOSE_PRINT("bp 2\n");

    //   pool -> queue_head = newwork;
    // } else {
    //   struct task* tailWork = NULL;
    //   tailWork = pool -> queue_head;

    //   while(tailWork -> next != NULL) {
    //     tailWork = tailWork -> next;
    //   VERBOSE_PRINT("bp 3\n");
    //   }
      
    //   tailWork -> next = newwork;
      
    // }
    // pool -> queue_size++;

    uthread_cond_broadcast(pool->working_lock);
  uthread_mutex_unlock(pool -> mut);
  VERBOSE_PRINT("task added\n");
}

/* Blocks until the thread pool has no more scheduled tasks; then,
 * joins all worker threads, and frees the pool and all related
 * resources. Once this function returns, the pool cannot be used
 * anymore.
 *
 * Parameters: pool: the pool to be joined.
 */
void tpool_join(tpool_t pool) {

  VERBOSE_PRINT("joining thread\n");
  /* TO BE COMPLETED BY THE STUDENT */
  uthread_mutex_lock(pool -> mut);
    // while(pool -> queue_size != 0) {

    // }
    pool -> finished = 1;
    uthread_cond_broadcast(pool -> working_lock);
  uthread_mutex_unlock(pool -> mut);
  VERBOSE_PRINT("wait for join thread\n");
  
  // uthread_mutex_lock(pool -> mut);

  for(int i = 0; i < pool -> max_working; i++) {
    uthread_join(pool->thread[i],NULL);
  } 
  // VERBOSE_PRINT("thread joined\n");
  // uthread_mutex_unlock(pool -> mut);


  struct task* head = NULL;
  while(pool->queue_head != NULL) {
    head = pool->queue_head;
    pool -> queue_head = pool -> queue_head -> next;
    free(head);
  }
  free(pool -> thread);
  free(pool);

}
