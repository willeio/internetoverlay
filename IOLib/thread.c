#include "thread.h"
#include "list.h"
#include "io.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>


#define PRINT_MSG(x) printf("[%"PRIu32"] _threads_work: " x "\n", thread_id)


void* _threads_work(void* arg)
{
  uint32_t thread_id = 0;
  get_random_number(&thread_id, sizeof(uint32_t));

  thread_mgr_t *mgr = (thread_mgr_t *)arg;

  while (mgr->run)
  {
    // wait & block for work
    //PRINT_MSG("waiting for work...");
    pthread_mutex_lock(&mgr->mutex_work);
    pthread_cond_wait(&mgr->cond_wait_work, &mgr->mutex_work);
    //PRINT_MSG("I have work todo now!");
    pthread_mutex_unlock(&mgr->mutex_work);

    // get work
    pthread_mutex_lock(&mgr->mutex_thread);
    if (list_count(mgr->list_work) < 1)
    {
      //PRINT_MSG("work list full!");
      pthread_mutex_unlock(&mgr->mutex_thread);
      continue;
    }

    thread_work_t *work = list_take_last(mgr->list_work); // TODO: respect PRIORITY and SKIPPED_COUNT !!
    pthread_cond_signal(&mgr->cond_wait_free); // as there is now space in the work queue again, send signal for waiting threads
    pthread_mutex_unlock(&mgr->mutex_thread);
    (*work->f)(work->arg); // execute work

    //PRINT_MSG("work done!");
  }

  PRINT_MSG("run == 0 !");
}


thread_mgr_t *threads_create(uint16_t thread_count, uint16_t queue_size)
{
  thread_mgr_t *mgr = (thread_mgr_t *)malloc(sizeof(thread_mgr_t));

  mgr->run = 1;

  pthread_mutex_init(&mgr->mutex_thread, 0);
  pthread_mutex_init(&mgr->mutex_work, 0);

  pthread_cond_init(&mgr->cond_wait_free, 0);
  pthread_cond_init(&mgr->cond_wait_work, 0);

  mgr->list_threads = list_create();

  mgr->list_work = list_create(); // queue
  list_set_max_entries(mgr->list_work, queue_size);

  for (uint16_t i = 0; i < thread_count; i++)
  {
    pthread_t *t = (pthread_t *)malloc(sizeof(pthread_t));
    if (pthread_create(t, 0, _threads_work, mgr) != 0)
    {
      printf("threads_init: could not start thread #"PRIu16"!\n", i);
      //return 0;
    }

    list_add(mgr->list_threads, t); // TODO: check return value ?
  }

  return mgr;
}


void threads_wait_free(thread_mgr_t *mgr) // TODO: return value! dont add if run == false !
{
  while (mgr->run)
  {
    if (list_can_add(mgr->list_work) == 0)
      return; // space available

    // no space available, wait...
    usleep(1);
    pthread_cond_wait(&mgr->cond_wait_free, &mgr->mutex_thread);
    if (list_can_add(mgr->list_work) != 0)
      continue;

    return; // space available
  }
}


void threads_add_work(thread_mgr_t *mgr, thread_func_t f, void* arg)
{
  thread_work_t* w = (thread_work_t*)malloc(sizeof(thread_work_t));

  w->f = f;
  w->arg = arg;

  pthread_mutex_lock(&mgr->mutex_thread);

  if (list_append(mgr->list_work, w) < 0)
  {
    puts("threads_add_work: could not add work to list");
    free(w);
  }

  pthread_mutex_unlock(&mgr->mutex_thread);

  pthread_cond_signal(&mgr->cond_wait_work); // there's work todo !
}


void threads_free(thread_mgr_t *mgr) // call on app term
{
  mgr->run = 0;

  // stop all blocking waits on threads
  pthread_cond_broadcast(&mgr->cond_wait_free);
  pthread_cond_broadcast(&mgr->cond_wait_work);

  // wait for all threads to exit
  while (mgr->list_threads->count > 0)
  {
    pthread_t *t = (pthread_t *)mgr->list_threads->e; // TODO: e exists if count > 0 ?
    pthread_join(*t, 0); // wait for this thread to exit
  }




  // TODO: free everything here




  free(mgr);
}
