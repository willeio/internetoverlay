#include <IOLib/io.h>

#include <stdlib.h> // malloc
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>





union chat_msg
{
  struct chat_msg_data
  {
    char magic[2];
    uint32_t id;
    uint8_t msg[256];
  } data;
  uint8_t buffer[sizeof(struct chat_msg_data)];
};





int run;
struct node_list nodes_list;


void handle_client_blob(struct client_blob* cb)
{
  RAND_MAX;

  // TODO: now redirect to API !

//  printf("received cb: \"%s\"\n", cb->blob);

  free(cb->blob);
}


void handle(int sock)
{
  while(run)
  {
    sleep(1);

    union protocol prot;

    if (get_protocol(sock, &prot) < 0)
      return;

    struct client_blob cb;

    if (get_client_blob(sock, &prot, &cb) < 0)
    {
      log_not_supported(&prot);
      return;
    }

    handle_client_blob(&cb);

//    printf("RECEIVED CLIENT BLOB: \"%s\"\n", cb.blob);
  }
}


int main()
{
  signal(SIGPIPE, SIG_IGN); // ignore sigpipes (because broken pipes do not harm this program's execution)
  srand(time(NULL)); // TODO: use safe random numbers in whole project !

  printf("<IOClient>\n\n");

  int node_count = get_node_list(&nodes_list);
  run = 1;

  if (node_count < 0)
    return -1;

  if (node_count < 1)
  {
    printf("too few nodes online!\n");
    return -2;
  }

  printf("node count: %d\n", node_count);



  // tests:
  struct node *random_node = get_random_node(&nodes_list);

  int sock = open_client(random_node);

  if (sock < 0)
    return -3;

  struct client_blob cb;


  union chat_msg cm;
  cm.data.magic[0] = 'C';
  cm.data.magic[1] = 'M';
  cm.data.id = rand(); // :)

  memset(cm.data.msg, 0, 256);

  const char* text = "Dies ist ein Test...! :)";
  strcpy(cm.data.msg, text);

  cb.blob_len = sizeof(union chat_msg);
  cb.blob = cm.buffer;

//  printf("sending test client blob..\n");

  if (send_client_blob(sock, &cb) < 0)
    return -1;

//  printf("done!\n");


  // we are now connected to the network, thanks to the IONode we are connected to !

  //handle(sock);
  close(sock);

  // TODO: don't exit if we disconnect! reconnect and go to RUN again!

  printf("normal exit!\n");
  return 0;
}
