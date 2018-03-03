#include "watcher.h"
#include "shared.h"
#include <IOLib/thread.h>

#include <string.h> // otherwise strerror will not work
#include <unistd.h> // sleep
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h> // free
#include <errno.h>


void *__take_last_mutexed(list_t *list)
{
  pthread_mutex_lock(&mutex_shared);
  void *value = list_take_last(list);
  pthread_mutex_unlock(&mutex_shared);

  return value;
}


void __put_back_mutexed(list_t *list, void *value)
{
  pthread_mutex_lock(&mutex_shared);
  list_add(list, value);
  pthread_mutex_unlock(&mutex_shared);
}


void *_thread_client_blob_processing(void *arg)
{
  // TODO: connect to another node or dismiss for security reasons ?

  (void*)arg;

  struct client_blob *cb = __take_last_mutexed(client_blob_list);

  if (cb)
  {
    pthread_mutex_lock(&mutex_shared);
    list_entry_t * e = client_list->e;

    while (e)
    {
      int *_client_sock = (int*)e->val;
      int client_sock = *_client_sock;

      printf("watcher: sending nb as cb to %d\n" /*- with text: %s\n"*/, client_sock);//, cb.blob);

#error hier bin ich stehen geblieben

      if (send_client_blob(client_sock, cb) == 0)
      { // success
        pthread_mutex_unlock(&mutex_shared);
        free(cb);
      }
      else
      { // failed
        // delete client as it is defective !
        close(client_sock);
        _list_remove(client_list, e);
        free(_client_sock);
        e = client_list->e; // go back to the start (sec)!
        continue;
        //printf("watcher: connection %d closed\n", client_sock);
      }

      e = e->next;
    } // while (e)

    pthread_mutex_unlock(&mutex_shared);
  }

  __put_back_mutexed(client_blob_list, cb);
  puts("watcher: unable to send cb to this client!");
  return 0;
}


void *_thread_node_blob_processing(void *arg)
{
  // TODO: connect to another node or dismiss for security reasons ?
  // TODO: redirect to more than just one node
  (void*)arg;

  struct node_blob *nb = __take_last_mutexed(node_blob_list);

  if (nb)
  {
    pthread_mutex_lock(&mutex_shared);
    struct node *random_node = get_random_node(nodes_list);

    if (random_node)
    {
      struct node *copy = (struct node *)malloc(sizeof(struct node));
      memcpy(copy, random_node, sizeof(struct node));
      random_node = copy;

      pthread_mutex_unlock(&mutex_shared);

      int node_sock = open_connection_node(random_node);

      if (node_sock)
      {
        if (send_node_blob(node_sock, nb) == 0)
        {
          free(nb); // done!
          close(node_sock);
          return 0;
        }

        close(node_sock);
      }
    }
    else // if (random_node)
    {
      pthread_mutex_unlock(&mutex_shared);
    }
  }

  __put_back_mutexed(node_blob_list, nb);
  printf("watcher: unable to send nb to random node!\n");

  return 0;
}


void* thread_watcher(void* arg) // send client blobs to other nodes
{
  (void)arg;

  thread_mgr_t *mgr = threads_create(32, 128);

  while (run)
  {
    usleep(100);

    if (list_count(client_blob_list) > 0)
      threads_add_work(mgr, _thread_client_blob_processing, 0 /* no arg needed */);

    if (list_count(node_blob_list) > 0)
      threads_add_work(mgr, _thread_node_blob_processing, 0 /* no arg needed */);
  }

  threads_free(mgr);

  return 0;
}
