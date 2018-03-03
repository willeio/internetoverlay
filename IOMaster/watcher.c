#include "watcher.h"

#include <unistd.h> // sleep
#include <stdio.h>
#include <stdlib.h>


void* thread_watcher(void* arg) // heartbeat for listed nodes
{
  (void)arg;

//  printf("watcher: ready\n");

  while (run)
  {
    sleep(1); // check every x secs

    // NOTE: not the best solution for production, but for the quick impl that's perfectly okay

    pthread_mutex_lock(&mutex_shared);
    struct node *n = list_take_last(nodes_list); // client has to prove itself before getting put back into the list

    if (n)
    {
      int deleted = 0;

      if (n->client_port) // only check if client port > 0 (port is open)
      {
        int sock = open_connection_client(n);

        if (sock > 0) // success?
          close(sock);
        else
          deleted = 1;
      }

      if (deleted == 0 && n->node_port) // only check if noe port > 0  AND IF NODE NOT DELETED ALREADY
      {
        int sock = open_connection_node(n);

        if (sock > 0) // success?
          close(sock);
        else
          deleted = 1;
      }

      if (deleted == 0)
        list_add(nodes_list, n); // put node back if it's not marked for deletion (but as first entry - take last, add first)

    } // if (n)

    pthread_mutex_unlock(&mutex_shared);

  } // while (run)

  return 0;
}
