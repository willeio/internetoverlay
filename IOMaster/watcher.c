#include "watcher.h"

#include <unistd.h> // sleep
#include <stdio.h>
#include <stdlib.h>


void delete_node(int i)
{
  free(nodes_list[i]);
  nodes_list[i] = 0;
  printf("watcher: deleted #%d\n", i);
}


void* thread_watcher(void* arg) // heartbeat for listed nodes
{
  (void)arg;

//  printf("watcher: ready\n");

  while (run)
  {
    sleep(35); // check every x secs

//    printf("watcher: checking...\n");

    pthread_mutex_lock(&mutex_shared);
    // TODO: copy list here also..

    for (int i = 0; i < MAX_NODES; i++)
    {
      struct node* n = nodes_list[i];

      if (!n)
        continue;

//      printf("> trying...\n");

      if (n->client_port) // only check if client port > 0 (port is open)
      {
        int sock = open_connection_client(n);

        if (sock < 0)
        {
          delete_node(i);
          continue;
        }

        close(sock);
      }

      if (n->node_port) // only check if noe port > 0
      {
        int sock = open_connection_node(n);

        if (sock < 0)
        {
          delete_node(i);
          continue;
        }

        close(sock);
      }
    }
    pthread_mutex_unlock(&mutex_shared);
//    printf("done!\n");
  }

  return 0;
}
