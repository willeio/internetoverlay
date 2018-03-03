#include "pool.h"

#include <pthread.h>


int pool_init(pool_t *pool, uint16_t reserved, uint16_t elastic)
{
  (void)elastic; // NIY

  pool->thread_list = list_create();

  for (uint16_t i = 0; i < reserved; i++)
  {
    pthread_t* t = (pthread_t*)malloc(sizeof(pthread_t));

    if (pthread_create(t, 0, _threads_work, tt) != 0)
    {
      printf("threads_set_count: could not create new thread !!!\n"); // TODO: "If at first you don't succeed (first you don't succeed) ... "
      free(t);
      free(tt);
      continue;
    }

    if (list_append(mgr->list_threads, (void*)tt) < 0)
    {
      printf("threads_set_count: could not add thread to list !!!\n"); // TODO: " ... Dust yourself off, and try again", Aaliyah.
      free(t);
      free(tt);
      continue;
    }
  }


  return 0;
}
