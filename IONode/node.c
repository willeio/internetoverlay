#include "shared.h"

#include <IOLib/io.h>
#include <IOLib/thread.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>


void _handle_node_connection(int sock)
{
  union protocol prot;

  if (get_protocol(sock, &prot) < 0)
    return;

  struct node_blob nb;
  if (get_node_blob(sock, &prot, &nb) < 0)
  {
    printf("handle_node_connection: received non-node-blob %c%c on node port!\n", prot.data.magic[0], prot.data.magic[1]);
    return;
  }

  //puts("_handle_node_connection: handle node");

  //printf("node: received text was: %s\n", nb.blob);
  //printf("node: received node blob with %d hops left\n", nb.hops);

  if (nb.hops == 0) // FIXME: strange logic problem - any node has to send a node blob, even though it knows that the receiving node will not even process it if hops == 0 and will delete it.. very strange
  {
    //printf("node: no hops left! not redirecting.\n");
    return;
  }

  nb.hops -= 1; // TODO: RANDOMIZE !!

  struct node_blob* nb_ptr = (struct node_blob*)malloc(sizeof(struct node_blob));
  memcpy(nb_ptr, &nb, sizeof(struct node_blob));

  struct client_blob *cb_ptr = (struct client_blob *)malloc(sizeof(struct client_blob));
  memcpy(cb_ptr->blob, nb.blob, 256);

  pthread_mutex_lock(&mutex_shared);
  list_append(node_blob_list, nb_ptr);
  list_append(client_blob_list, cb_ptr);
  pthread_mutex_unlock(&mutex_shared);
}


void* _thread_node_connection(void *arg /* socket fd */)
{
  int *_node_sock = (int*)arg;
  int node_sock = *_node_sock;
  free(_node_sock);

  //printf("node: new connection! %d\n", node_sock);

  _handle_node_connection(node_sock);
  close(node_sock);

  return 0;
}


void* thread_node(void* arg) // receive from IONodes
{
  uint16_t port = *(uint16_t*)arg;

//  printf("thread_node: port = %hu\n", port);


  int server = create_server(port);

  if (server < 0)
    exit(-1);

  thread_mgr_t *mgr = threads_create(32, 128);

  while (run)
  {
    usleep(1);

    //printf("waiting for IONode connection..\n");

    threads_wait_free(mgr);

    int node_sock = accept_connection(server);

    if (node_sock < 0)
    {
      //printf("node: unconnected node\n");
      continue;
    }

    int *_node_sock = (int *)malloc(sizeof(int)); // will be free'd in-thread !
    *_node_sock = node_sock;

    threads_add_work(mgr, _thread_node_connection, _node_sock);
  }

  return 0;
}







