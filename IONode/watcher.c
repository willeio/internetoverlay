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


void _thread_client_blob_processing()
{
  // TODO: connect to another node or dismiss for security reasons ?

  pthread_mutex_lock(&mutex_shared);
  int client_blob_count = list_count(client_blob_list);
  pthread_mutex_unlock(&mutex_shared);

  if (client_blob_count < 1)
    return;


  struct client_blob *cb = __take_last_mutexed(client_blob_list);




  pthread_mutex_lock(&mutex_shared);
  int client_count = list_count(client_list);
  pthread_mutex_unlock(&mutex_shared);

  if (client_count < 1)
    return;

  // take client, he has to 'prove' itself before it can get back into the list, by reading our bytes without an error
  int *_client_sock = (int *)__take_last_mutexed(client_list);
  //puts("_thread_client_blob_processing: took client from list");

  if (!_client_sock)
    return;


  int client_sock = *_client_sock;
  free(_client_sock);





  //printf("watcher: sending nb as cb to %d\n" /*- with text: %s\n"*/, client_sock);//, cb.blob);



  if (send_client_blob(client_sock, cb) != 0) // client failed
  {
    //puts("_thread_client_blob_processing: send_client_blob failed - deleting client");
    close(client_sock); // close socket
  }
  else
  {
    int *putback_client = (int *)malloc(sizeof(int)); // put client back into list, successfully sent
    *putback_client = client_sock;

    __put_back_mutexed(client_list, putback_client);
    //puts("_thread_client_blob_processing: put client back into list");
  }



  return;
}


void _thread_node_blob_processing()
{
  // TODO: connect to another node or dismiss for security reasons ?
  // TODO: redirect to more than just one node

  pthread_mutex_lock(&mutex_shared);
  struct node *random_node = get_random_node(nodes_list);

  if (!random_node)
  {
    pthread_mutex_unlock(&mutex_shared);
    return;
  }

  // copy the node struct, so that the mutex can be unlocked again
  struct node copy;
  memcpy(&copy, random_node, sizeof(struct node));

  pthread_mutex_unlock(&mutex_shared);


  struct node_blob *nb = __take_last_mutexed(node_blob_list);

  if (!nb)
    return;


  int node_sock = open_connection_node(&copy);

  if (node_sock > 0)
  {
    if (send_node_blob(node_sock, nb) == 0)
      free(nb); // done!
    else
      __put_back_mutexed(node_blob_list, nb); // failed to send, put nb back!

    close(node_sock);
  }
  else
  {
    __put_back_mutexed(node_blob_list, nb); // failed to open connection, put nb back!
  }

  return;
}


void* thread_watcher(void* arg) // send client blobs to other nodes
{
  (void)arg;

  while (run)
  {
    usleep(100); // 100µs

    _thread_client_blob_processing();
    _thread_node_blob_processing();
  }

  return 0;
}
