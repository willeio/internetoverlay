/*/
 * IONode
/*/

#include "shared.h"
#include "client.h"
#include "node.h"
#include "watcher.h"
#include "refresh.h"
#include "select.h"

#include <IOLib/io.h>
#include <IOLib/list.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <inttypes.h>





//#error Moeglichst viel auf thread_mgr umstellen! ausserdem: wo kommt der thread_mgr im IONode her? ist nirgends implementiert





int announce(uint16_t client_port, uint16_t node_port)
{
  int sock = open_connection_string(MASTER_IP, 6453);

  if (sock < 0)
    return -1;

  struct node_announce na;
  na.client_port = client_port;
  na.node_port = node_port;

  if (send_node_announce(sock, &na) < 0)
    return -2;

  return 0;
}


int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  srand(time(NULL)); // FIXME: use safe random numbers in whole project !
  signal(SIGPIPE, SIG_IGN); // ignore sigpipes (because broken pipes do not harm this program's execution)

  printf("<IONode>\n\n");

  if (init() < 0)
    return -1;

  uint16_t node_port = 0;
  uint16_t client_port = 0;

  do
  {
    get_random_number(&node_port, sizeof(node_port));
  } while (node_port < 1024);

  printf("node_port: %"PRIu16"\n", node_port);

  do
  {
    get_random_number(&client_port, sizeof(client_port));
  } while (client_port == node_port || client_port < 1024);

  printf("client_port: %"PRIu16"\n", client_port);



  client_blob_list = list_create();
  list_set_max_entries(client_blob_list, 128);


  node_blob_list = list_create();
  list_set_max_entries(node_blob_list, 128);


  client_list = list_create();
  list_set_max_entries(client_list, 128);



  pthread_mutex_init(&mutex_shared, 0);

  run = 1;

//  node_blob_list_pos = -1;
//  for (int i = 0; i < MAX_NODE_BLOBS; i++)
//  {
//    node_blob_list[i] = 0;
//  }

//  for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++)
//  {
//    connected_clients_list[i] = -1;
//  }

//  nodes_list.count = 0;
//  nodes_list.n = 0;


  printf("getting node list ..\n"); // initial list
  nodes_list = list_create();
  int nodes_count = get_node_list(nodes_list);
  if (nodes_count < 0)
  {
    printf("unable to get node list from master!\n");
    return 87;
  }
  printf("node count: %d\n", nodes_count);


  pthread_t node_thread;
  if (pthread_create(&node_thread, 0, thread_node, &node_port) != 0)
  {
    printf("could not start node thread\n");
    return -66;
  }


  pthread_t select_thread;
  if (pthread_create(&select_thread, 0, thread_select, 0) != 0)
  {
    printf("could not start select thread\n");
    return -50;
  }


  pthread_t client_thread;
  if (pthread_create(&client_thread, 0, thread_client, &client_port) != 0)
  {
    printf("could not start client thread\n");
    return -55;
  }


  pthread_t watcher_thread;
  if (pthread_create(&watcher_thread, 0, thread_watcher, 0) != 0)
  {
    printf("could not start watcher thread\n");
    return -66;
  }


  pthread_t refresh_thread;
  if (pthread_create(&refresh_thread, 0, thread_refresh, 0) != 0)
  {
    printf("could not start refresh thread\n");
    return -88;
  }


  // success, connect to master server and tell him we participate on the network by sending our ip
  if (announce(client_port, node_port) < 0)
    return -77;


  pthread_join(node_thread, NULL);
  pthread_join(client_thread, NULL);
  pthread_join(watcher_thread, NULL);
  pthread_join(refresh_thread, NULL);

  // TODO: shutdown threads, then exit (no direct return)
  printf("normal exit!\n");

  return 0;
}
