#include "client.h"
#include "shared.h"

#include <IOLib/io.h>
#include <IOLib/thread.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h> // memcpy
#include <inttypes.h>


void* thread_client(void* arg) // receive from IOClients
{
  uint16_t port = *(uint16_t*)arg;

  //printf("client_node: port = %hu\n", port);


  int server = create_server(port);

  if (server < 0)
    exit(-1);


  while (run)
  {
    usleep(1);

    //printf("waiting for IOClient connection..\n");

    int client_sock = accept_connection(server);

    if (client_sock < 0)
    {
      //printf("client: unconnected client\n");
      continue;
    }

    int *_client_sock = (int *)malloc(sizeof(int)); // will de free'd in watcher thread if connection fails (client closed connection)
    *_client_sock = client_sock;

    pthread_mutex_lock(&mutex_shared);
    list_append(client_list, _client_sock);
    pthread_mutex_unlock(&mutex_shared);

    //puts("thread_client: new client added!");
    //printf("client.c: %"PRIu64" ", client_list);

    // only add the new client to the client list. clients are read by select.h, written to and checked (/ managed) by watcher.h  :)
  }

  return 0;
}


















