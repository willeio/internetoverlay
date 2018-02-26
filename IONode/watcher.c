#include "watcher.h"
#include "shared.h"

#include <string.h> // otherwise strerror will not work ...
#include <unistd.h> // sleep
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h> // free
#include <errno.h>


void* thread_watcher(void* arg) // send client blobs to other nodes
{
  (void)arg;

//  printf("watcher: waiting for client blobs to redirect..\n");

  while (run)
  {
    sleep(1);

    pthread_mutex_lock(&mutex_shared);

    if (list_count(node_blob_list) < 1)
    {
      pthread_mutex_unlock(&mutex_shared);
      continue;
    }

//    printf("watcher: new node blob to redirect!\n");

    struct node_blob* nb = list_take_last(node_blob_list);
    pthread_mutex_unlock(&mutex_shared);

    // redirect node_blob to a random node
    pthread_mutex_lock(&mutex_shared);
    int node_sock = open_connection_node(get_random_node(nodes_list));
    pthread_mutex_unlock(&mutex_shared);

    if (node_sock < 0)
    {
      printf("watcher: cannot connect to that random node!\n");

      // TODO: connect to next random node !!!

      continue;
    }

    if (send_node_blob(node_sock, nb) < 0)
    {
      // TODO: connect to another node or dismiss for security reasons ?
      printf("watcher: unable to send nb to random node!\n");
      free(nb); // TODO: function auslagern! sonst muss immer free an jeder kondition aufgerufen werden ....
      continue;
    }

    close(node_sock);


    // redirect that node_blob (as client_blob) to all connected clients !
    // create client_blob so that the clients can read the message
    struct client_blob cb;
    memcpy(cb.blob, nb->blob, 256);
    free(nb);

    // TODO: auslagern in threads ??? oder als workload iwo speichern

    if (list_count(client_list) < 1)
      continue;

    list_entry_t* e = client_list->e;

    while (e->next)
    {
      int client_sock = *((int*)e->val);

      printf("watcher: sending nb as cb to %d\n" /*- with text: %s\n"*/, client_sock);//, cb.blob);

      if (send_client_blob(client_sock, &cb) < 0)
      {
        // delete client as it is defective !
        close(client_sock);
        _list_remove(client_list, e);
        //printf("watcher: connection %d closed\n", client_sock);
        continue;
      }

      e = e->next;
    }

    // TODO: process all node blobs first, and then call continue?
  }

  return 0;
}
