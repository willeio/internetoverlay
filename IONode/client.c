#include "shared.h"

#include <IOLib/io.h>
#include <IOLib/thread.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h> // memcpy


void handle_client_connection(int sock)
{
  while (run)
  {
    union protocol prot;

    if (get_protocol(sock, &prot) < 0)
      return;

    struct client_blob cb;
    if (get_client_blob(sock, &prot, &cb) < 0)
    {
      printf("handle_client_connection: received non-client-blob %c%c on client port!\n", prot.data.magic[0], prot.data.magic[1]);
      return;
    }

    //printf("handle_client_connection: magic was: %c%c\n", prot.data.magic[0], prot.data.magic[1]);

//    printf("client: received text was: %s\n", cb.blob);

    //puts("handle client");


    // create node_blob from this received message
    struct node_blob* nb = (struct node_blob*)malloc(sizeof(struct node_blob));
    memcpy(nb->blob, cb.blob, 256); // TODO: on all memcpy: sizeof WHAT?
    nb->hops = (rand() % 100) + 23; // TODO: better RANDOMIZE!   min: 23 - max: 123

    struct client_blob *cb_ptr = (struct client_blob*)malloc(sizeof(struct client_blob));
    memcpy(cb_ptr->blob, cb.blob, 256); // TODO: on all memcpy: sizeof WHAT?

    pthread_mutex_lock(&mutex_shared);
    list_append(node_blob_list, nb); // TODO: return error or ignore?
    list_append(client_blob_list, cb_ptr); // TODO: return error or ignore?
    pthread_mutex_unlock(&mutex_shared);

//    printf("client: recvd cb from %d\n", sock);
  }
}


void* thread_client_connection(void* arg /* socket fd */)
{
  int *_client_sock = (int *)arg;

  //printf("client: new connection! %d\n", client_sock);

  pthread_mutex_lock(&mutex_shared);
  list_append(client_list, _client_sock);
  pthread_mutex_unlock(&mutex_shared);

  handle_client_connection(*_client_sock);
  close(*_client_sock); // FIXME: could a termination-function be that slow, that THIS close is called, other client connects on the same socket and the termination-function then calls close on that client sock, because the thread has to be terminated??!
  // no need to delete the client from the list, the watcher will do this for us

  return 0;
}


void* thread_client(void* arg) // receive from IOClients
{
  uint16_t port = *(uint16_t*)arg;

  //printf("client_node: port = %hu\n", port);


  int server = create_server(port);

  if (server < 0)
    exit(-1);


  thread_mgr_t *mgr = threads_create(32, 128);

  while (run)
  {
    usleep(1);

    //printf("waiting for IOClient connection..\n");

    threads_wait_free(mgr);

    int client_sock = accept_connection(server);

    if (client_sock < 0)
    {
      //printf("client: unconnected client\n");
      continue;
    }

    int *_client_sock = (int *)malloc(sizeof(int)); // will de free'd in watcher thread if connection fails (client closed connection)
    *_client_sock = client_sock;
    threads_add_work(mgr, thread_client_connection, _client_sock);
  }

  return 0;
}



















