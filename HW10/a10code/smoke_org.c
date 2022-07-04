#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "uthread.h"
#include "uthread_mutex_cond.h"

#define NUM_ITERATIONS 1

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
#else
#define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
#endif

struct Agent {
  uthread_mutex_t mutex;
  uthread_cond_t  match;
  uthread_cond_t  paper;
  uthread_cond_t  tobacco;
  uthread_cond_t  smoke;
};

struct Agent* createAgent() {
  struct Agent* agent = malloc (sizeof (struct Agent)); 
  agent->mutex  = uthread_mutex_create();
  agent->smoke   = uthread_cond_create(agent->mutex);
  agent->match   = uthread_cond_create(agent->mutex);
  agent->paper   = uthread_cond_create(agent->mutex);
  agent->tobacco   = uthread_cond_create(agent->mutex);

  return agent;
}

//
// TODO
// You will probably need to add some procedures and struct etc.
//
struct Smoker {
  int paper_count;
  int tobacco_count;
  int match_count;
  uthread_cond_t p_producer;
  uthread_cond_t t_producer;
  uthread_cond_t m_producer;
  struct Agent *a;
};

struct Smoker* createSmoker(struct Agent* agent) {
  struct Smoker* smoker = malloc(sizeof(struct Smoker));
  smoker->a = agent;
  smoker->paper_count = 0;
  smoker->tobacco_count = 0;
  smoker->match_count = 0;
  smoker->p_producer = uthread_cond_create(agent->mutex);
  smoker->t_producer = uthread_cond_create(agent->mutex);
  smoker->m_producer = uthread_cond_create(agent->mutex);
  return smoker;
}

/**
 * You might find these declarations helpful.
 *   Note that Resource enum had values 1, 2 and 4 so you can combine resources;
 *   e.g., having a MATCH and PAPER is the value MATCH | PAPER == 1 | 2 == 3
 */
enum Resource            {    MATCH = 1, PAPER = 2,   TOBACCO = 4};
char* resource_name [] = {"", "match",   "paper", "", "tobacco"};

// # of threads waiting for a signal. Used to ensure that the agent
// only signals once all other threads are ready.
int num_active_threads = 0;

int signal_count [5];  // # of times resource signalled
int smoke_count  [5];  // # of times smoker with resource smoked

void* paper_produced(void* s) {
  struct Smoker *smoker = s;
  printf("paper producer\n");
  uthread_mutex_lock(smoker->a->mutex);
  while(1){
    // num_active_threads++;
    uthread_cond_wait(smoker->a->paper);
    // num_active_threads--;
    smoker->paper_count++;
    if(smoker->tobacco_count > 0 || smoker->match_count > 0) {
      smoker->paper_count--;
      if(smoker -> tobacco_count > 0) {
        smoker -> tobacco_count--;
        uthread_cond_signal(smoker->m_producer);
      }
      if(smoker -> match_count > 0) {
        smoker -> match_count--;
        uthread_cond_signal(smoker->t_producer);
      }
    }
  }
  uthread_mutex_unlock(smoker->a->mutex);
  printf("paper produce finished\n");
  return NULL;
}


void* match_produced(void* s) {
  struct Smoker *smoker = s;
  printf("match producer\n");
  uthread_mutex_lock(smoker->a->mutex);
  while(1){
    // num_active_threads++;
    uthread_cond_wait(smoker->a->match);
    // num_active_threads--;

    // smoker->match_count++;
    if(smoker->tobacco_count > 0 || smoker->paper_count > 0) {
      // smoker->match_count--;
      if(smoker -> tobacco_count > 0) {
        smoker -> tobacco_count--;
        uthread_cond_signal(smoker->p_producer);
      }
      if(smoker -> paper_count > 0) {
        smoker -> paper_count--;
        uthread_cond_signal(smoker->t_producer);
      }
    }
  }
  uthread_mutex_unlock(smoker->a->mutex);
  printf("match produce finished\n");
  return NULL;
}


void* tobacco_produced(void* s) {
  struct Smoker *smoker = s;
  printf("tobacco produce\n");
  uthread_mutex_lock(smoker->a->mutex);
  while(1){
    // num_active_threads++;
    uthread_cond_wait(smoker->a->tobacco);
    // num_active_threads--;

    smoker->tobacco_count++;
    if(smoker->paper_count > 0 || smoker->match_count > 0) {
      smoker->tobacco_count--;
      if(smoker -> paper_count > 0) {
        smoker -> paper_count--;
        uthread_cond_signal(smoker->m_producer);
      }
      if(smoker -> match_count > 0) {
        smoker -> match_count--;
        uthread_cond_signal(smoker->p_producer);
      }
    }
  }
  uthread_mutex_unlock(smoker->a->mutex);
  printf("tobacco produce finished\n");
  return NULL;
}

void* p_smoke(void *s) {
  struct Smoker *smoker = s;
  printf("p smoking\n");
  uthread_mutex_lock(smoker->a->mutex);
  while(1){
    // num_active_threads++;

    uthread_cond_wait(smoker->p_producer);
    // num_active_threads--;

    smoke_count[PAPER]++;
    uthread_cond_signal(smoker->a->smoke);
  }
  uthread_mutex_unlock(smoker->a->mutex);
  printf("p smoked\n");
  return NULL;
}

void* t_smoke(void *s) {
  struct Smoker *smoker = s;
  printf("t smoking\n");
  uthread_mutex_lock(smoker->a->mutex);
  while(1){
    // num_active_threads++;

    uthread_cond_wait(smoker->t_producer);
    // num_active_threads--;

    smoke_count[TOBACCO]++;
    uthread_cond_signal(smoker->a->smoke);
  }
  uthread_mutex_unlock(smoker->a->mutex);
  printf("t smoked\n");
  return NULL;
}

void* m_smoke(void *s) {
  struct Smoker *smoker = s;
  printf("m smoking\n");
  uthread_mutex_lock(smoker->a->mutex);
  while(1){
    // num_active_threads++;

    uthread_cond_wait(smoker->m_producer);
    // num_active_threads--;

    smoke_count[MATCH]++;
    uthread_cond_signal(smoker->a->smoke);
  }
  uthread_mutex_unlock(smoker->a->mutex);
  printf("m smoked\n");
  return NULL;
}

/**
 * This is the agent procedure.  It is complete and you shouldn't change it in
 * any material way.  You can modify it if you like, but be sure that all it does
 * is choose 2 random resources, signal their condition variables, and then wait
 * wait for a smoker to smoke.
 */
void* agent (void* av) {
  struct Agent* a = av;
  static const int choices[]         = {MATCH|PAPER, MATCH|TOBACCO, PAPER|TOBACCO};
  static const int matching_smoker[] = {TOBACCO,     PAPER,         MATCH};

  srandom(time(NULL));
  printf("agent thread entered\n");
  uthread_mutex_lock (a->mutex);
  // Wait until all other threads are waiting for a signal
  while (num_active_threads < 3)
    uthread_cond_wait (a->smoke);

  printf("agent start\n");
  for (int i = 0; i < NUM_ITERATIONS; i++) {
    int r = random() % 6;
    switch(r) {
    case 0:
      signal_count[TOBACCO]++;
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      break;
    case 1:
      signal_count[PAPER]++;
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      break;
    case 2:
      signal_count[MATCH]++;
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      break;
    case 3:
      signal_count[TOBACCO]++;
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      break;
    case 4:
      signal_count[PAPER]++;
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      break;
    case 5:
      signal_count[MATCH]++;
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      break;
    }
    VERBOSE_PRINT ("agent is waiting for smoker to smoke\n");
    uthread_cond_wait (a->smoke);
  }
  
  uthread_mutex_unlock (a->mutex);
  return NULL;
}

int main (int argc, char** argv) {
  
  struct Agent* a = createAgent();
  uthread_t agent_thread;

  uthread_init(5);
  
  // TODO
  struct Smoker* smoker = createSmoker(a);
  agent_thread = uthread_create(agent, a);
  uthread_t paper_counter = uthread_create(paper_produced, smoker);
  num_active_threads++;
  uthread_t tobacco_counter = uthread_create(tobacco_produced, smoker);
  num_active_threads++;
  uthread_t match_counter = uthread_create(match_produced, smoker);
  num_active_threads++;
  uthread_t p_producer = uthread_create(p_smoke, smoker);
  uthread_t t_producer = uthread_create(t_smoke, smoker);
  uthread_t m_producer = uthread_create(m_smoke, smoker);

  uthread_join(agent_thread, NULL);
  

  assert (signal_count [MATCH]   == smoke_count [MATCH]);
  assert (signal_count [PAPER]   == smoke_count [PAPER]);
  assert (signal_count [TOBACCO] == smoke_count [TOBACCO]);
  assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS);

  printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
          smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);

  return 0;
}
