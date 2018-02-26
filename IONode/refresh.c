#include "refresh.h"
#include "shared.h"

#include <IOLib/io.h>

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


void* thread_refresh(void* arg) // regets the node list
{
  (void)arg;

//  printf("refresh: ready to refresh nodes list\n");

  while (run)
  {
    sleep(20);

    pthread_mutex_lock(&mutex_shared);

    int nodes_count = get_node_list(nodes_list);
    if (nodes_count < 0)
    {
      printf("refresh: unable to get node list from master!\n");
      pthread_mutex_unlock(&mutex_shared);
      continue;
    }
    printf("refresh: node count: %d\n", nodes_count);

    pthread_mutex_unlock(&mutex_shared);
  }

  return 0;
}
