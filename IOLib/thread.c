#include "thread.h"
#include "list.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



typedef struct work
{
  threads_func_t f;
  void* arg;
} work_t;


pthread_mutex_t mutex_thread;
list_t *list_threads;
list_t *list_work;


void* _threads_work(void* arg)
{
  (void)arg;

  while (run) // TODO: do not use 'run', as threads have to be stopped on other reasons than the app term!?
  {
    usleep(1);
    pthread_mutex_lock(&mutex_thread);

    if (list_count(list_work) > 0)
    {
      work_t* w = list_take_last(list_work);

      (*w->f)(w->arg); // execute work's function and give him the argument

      free(w);
    }

    pthread_mutex_unlock(&mutex_thread);
  }

  // TODO: stop thread in any way here?

  return 0;
}


void threads_set_count(uint16_t count)
{
  // TODO: lock!

  list_set_max_entries(list_threads, count);
  list_set_max_entries(list_work, count);

  // TODO: in case this function is called more than once, delete all old threads, and create new ones..

  printf("threads_set_count: creating %du threads\n", count);
  for (uint16_t i = 0; i < list_threads->max; i++)
  {
    pthread_t* t = (pthread_t*)malloc(sizeof(pthread_t));

    //list_threads

    if (pthread_create(t, 0, _threads_work, 0) != 0)
    {
      printf("threads_set_count: could not create new thread !!!\n"); // TODO: "If at first you don't succeed (first you don't succeed) ... "
      free(t);
      continue;
    }

    if (list_append(list_threads, (void*)t) < 0)
    {
      printf("threads_set_count: could not add thread to list !!!\n"); // TODO: " ... Dust yourself off, and try again", Aaliyah.
      free(t);
      continue;
    }
  }
}


void threads_init()
{
  run = 1;
  pthread_mutex_init(&mutex_thread, 0);
  list_threads = list_create();
  list_work = list_create();
  threads_set_count(10);
}


void threads_wait_free()
{
  while (run)
  {
    usleep(1);
    pthread_mutex_lock(&mutex_thread);

    if (!list_can_add(list_work))
    {
      pthread_mutex_unlock(&mutex_thread);
      continue;
    }

    pthread_mutex_unlock(&mutex_thread);
    return;
  }
}


void threads_add_work(threads_func_t f, void* arg)
{
  work_t* w = (work_t*)malloc(sizeof(work_t));

  w->f = f;
  w->arg = arg;

  pthread_mutex_lock(&mutex_thread);

  if (list_append(list_work, w) < 0)
    printf("could not add work to list!!!\n");

  pthread_mutex_unlock(&mutex_thread);
}


void threads_free() // call on app term
{
  // TODO: stop & delete everything here
}
