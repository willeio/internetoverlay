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

  // TODO: also reannounce ! e.g. the master was down

  while (run)
  {
    sleep(5);

    //puts("refresh: refreshing!");

    pthread_mutex_lock(&mutex_shared);

    int nodes_count = get_node_list(nodes_list);
    if (nodes_count < 0)
    {
      puts("refresh: unable to get node list from master!");
      pthread_mutex_unlock(&mutex_shared);
      continue;
    }

    pthread_mutex_unlock(&mutex_shared);

    printf("refresh: node count: %d\n", nodes_count);
  }

  return 0;
}
