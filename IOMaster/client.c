#include "client.h"
#include "shared.h"

#include <IOLib/io.h>
#include <IOLib/thread.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


void handle_client_connection(int cli)
{
  union protocol prot;

  if (get_protocol(cli, &prot) < 0)
    return;

  if (check_magic(&prot, "RN") == 0)
  {
    // node info must be copied here, as contacted clients could hang (by locking the shared mutex) and stop the server from working )=UDFU=)D=?=????????????????????????

    list_t *temp_list = list_create();

    pthread_mutex_lock(&mutex_shared);

    list_entry_t *e = nodes_list->e;

    while (e)
    {
      struct node *temp_node = (struct node *)malloc(sizeof(struct node));
      memset(temp_node, 0, sizeof(temp_node));
      memcpy(temp_node, e->val, sizeof(struct node));
      list_append(temp_list, temp_node);

      e = e->next;
    }

    pthread_mutex_unlock(&mutex_shared);


    int sent = 0;

    e = temp_list->e; // get the beginning of our own, new, unthreaded temp node list

    while (e)
    {
      if (send_node(cli, e->val) == 0)
        sent++;

      free(e->val); // free all used temporary nodes!

      e = e->next;
    }

    free(temp_list); // done, finally free the temp node list




    printf("handle_client_connection: sent %d nodes\n", sent);

    return;
  }




  struct node_announce na;
  if (get_node_announce(cli, &prot, &na) == 0)
  {
    // fill the new entry
    // get ip from connected socket
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    if (getpeername(cli, (struct sockaddr *)&addr, &addr_size) < 0)
    {
      printf("handle_client_connection: node announce: cannot get ip for node announcement - cannot add node to list!\n");
      return;
    }

    struct node *new_node = (struct node*)malloc(sizeof(struct node));

    new_node->ip = addr.sin_addr.s_addr;
    new_node->node_port = na.node_port;
    new_node->client_port = na.client_port;

    pthread_mutex_lock(&mutex_shared);
    list_append(nodes_list, new_node); // FIXME: scramble list!! sleep for some msecs!
    printf("handle_client_connection: added %s (%d, %d)\n", inet_ntoa(addr.sin_addr), na.node_port, na.client_port);
    pthread_mutex_unlock(&mutex_shared);

    return;
  }

  log_not_supported(&prot);
}


void* _thread_client_connection(void* arg /* socket fd */)
{
  int cli = *(int*)arg;

//  printf("client: new connection! %d\n", cli);

  handle_client_connection(cli);
  close(cli);

  free(arg); // cli_ptr, previously malloced in main

  return 0;
}


//void* thread_client(void* arg) // receive from IONodes
//{
//  (void)arg;



//  return 0;
//}

