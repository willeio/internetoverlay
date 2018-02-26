#include "shared.h"

#include <IOLib/io.h>

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
      return;

    printf(">> '%c''%c'", cb.blob[0], cb.blob[1]);

//    printf("client: received text was: %s\n", cb.blob);


    // create node_blob from this received message
    struct node_blob* nb = (struct node_blob*)malloc(sizeof(struct node_blob));
    memcpy(nb->blob, cb.blob, 256); // TODO: on all memcpy: sizeof WHAT?
    nb->hops = (rand() % 100) + 23; // TODO: better RANDOMIZE!   min: 23 - max: 123

    pthread_mutex_lock(&mutex_shared);
    list_append(node_blob_list, nb); // TODO: return error or ignore?
    pthread_mutex_unlock(&mutex_shared);

//    printf("client: recvd cb from %d\n", sock);
  }
}


void* thread_client_connection(void* arg /* socket fd */)
{
  int client_sock = *(int*)arg;

//  printf("client: new connection! %d\n", client_sock);

  /*
  pthread_mutex_lock(&mutex_shared);
  connected_clients_list_add(client_sock);
  pthread_mutex_unlock(&mutex_shared);
  */

  handle_client_connection(client_sock);
  close(client_sock);

  return 0;
}


void* thread_client(void* arg) // receive from IOClients
{
  uint16_t port = *(uint16_t*)arg;

//  printf("client_node: port = %hu\n", port);


  int server = create_server(port);

  if (server < 0)
    exit(-1);

  while (run)
  {
    usleep(1);

//    printf("waiting for IOClient connection..\n");
    int client_sock = accept(server, 0, 0);

    if (client_sock < 0)
    {
//      printf("client: unconnected client\n");
      continue;
    }

    pthread_t t;
    pthread_create(&t, 0, thread_client_connection, &client_sock);
  }

  return 0;
}
