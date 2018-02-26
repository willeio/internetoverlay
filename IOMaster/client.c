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


void handle_client_connection(int cli)
{
  union protocol prot;

  if (get_protocol(cli, &prot) < 0)
    return;

  if (check_magic(&prot, "RN") == 0)
  {
    // TODO: copy list, otherwise clients can hang this process and the master cannot spread the nodes any further..

    pthread_mutex_lock(&mutex_shared);
    int sent = 0;
    for (int i = 0; i < MAX_NODES; i++) // TODO: safe types !
    {
      if (nodes_list[i])
      {
        if (send_node(cli, nodes_list[i]) < 0)
        {
          pthread_mutex_unlock(&mutex_shared);
          return;
        }
        else
        {
          sent++;
        }
      }
    }
    printf("client: sent %d nodes\n", sent);
    pthread_mutex_unlock(&mutex_shared);

    return;
  }

  struct node_announce na;
  if (get_node_announce(cli, &prot, &na) == 0)
  {
    int list_pos = -1;

    // TODO: also copy, or use a better list model

    pthread_mutex_lock(&mutex_shared);
    for (int i = 0; i < MAX_NODES; i++)
    {
      if (nodes_list[i] == 0)
      {
        list_pos = i;
        break;
      }
    }

    if (list_pos == -1)
    {
      printf("list full!\n");
      return;
    }

    // now fill the new entry
    // get ip from connected socket
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    if (getpeername(cli, (struct sockaddr *)&addr, &addr_size) < 0)
    {
      printf("cannot get ip for node announcement - cannot add node to list!\n");
      return;
    }


    nodes_list[list_pos] = (struct node*)malloc(sizeof(struct node));

    nodes_list[list_pos]->ip = addr.sin_addr.s_addr;
    nodes_list[list_pos]->node_port = na.node_port;
    nodes_list[list_pos]->client_port = na.client_port;

    printf("added %s (%d, %d) as #%d\n", inet_ntoa(addr.sin_addr), na.node_port, na.client_port, list_pos);
    pthread_mutex_unlock(&mutex_shared);

    return;
  }

  log_not_supported(&prot);
}


void* thread_client_connection(void* arg /* socket fd */)
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

