#include "io.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <signal.h> // raise


int receive(int sock, uint8_t *buf, int size)
{
  ssize_t recvd = read(sock, buf, size); // TODO: write in buffer, on success copy buf to prot

  //if (size == 256)
    //printf("recv: '%s'\n", buf);

  if (recvd < 0)
  {
//    printf("receive: socket (%d) error! %s\n", sock, strerror(errno));
    return -1;
  }

  if (recvd == 0)
  {
//    printf("receive: remote socket disconnected!\n");   not really an error
    return -2;
  }

  if (recvd != size)
  {
    printf("receive: malformed message\n");
    return -3;
  }

//  printf("receive: received %d bytes [%s]\n", size, buf);

  return 0;
}


int out(int sock, uint8_t* buf, int size)
{
  //if (size == 256)
    //printf("out : '%s'\n", buf);

  ssize_t sent = write(sock, buf, size);

  if (sent < 0)
  {
    printf("out: socket (%d) error! %s\n", sock, strerror(errno));
    return -1;
  }

  if (sent == 0)
  {
//    printf("out: remote socket disconnected!\n");     not really an error
    return -2;
  }

  if (sent != size)
  {
    printf("out: sent buffer was not equal to size (sent %zu, prot size was %d)\n", sent, size);
    return -3;
  }

//  printf("out: sent %d bytes\n", size);

  //raise(SIGINT);

  return 0;
}


int _send_protocol(int sock, uint16_t msg_size, const char* magic /* feed with strlen == 2! */)
{
//  if (strlen(magic) != 2)
//  {
//    printf("send_protocol: magic length does not equal 2 !\n");
//    return -1;
//  }

  union protocol prot;
  prot.data.buf_len = msg_size;
  prot.data.magic[0] = magic[0];
  prot.data.magic[1] = magic[1];

  size_t size = sizeof(union protocol);

  if (out(sock, &prot.buffer[0], size) < 0)
    return -1;

  return 0;
}


int get_protocol(int sock, union protocol* prot)//, const char* magic /* feed with strlen == 2! */)
{
//  if (strlen(magic) != 2)
//  {
//    printf("get_protocol: magic length does not equal 2 !\n");
//    return -1;
//  }

  if (receive(sock, prot->buffer, sizeof(union protocol)) < 0)
    return -2;

//  if (prot->data.magic[0] != magic[0] ||
//      prot->data.magic[1] != magic[1])
//  {
//    printf("get_protocol: magic not equal (is %d %d, must be %d %d)\n",
//           prot->data.magic[0],
//           prot->data.magic[1],
//           magic[0],
//           magic[1]);

//    return -3;
//  }

  return 0;
}


struct node* get_random_node(list_t* nodes_list)
{
  if (nodes_list->count < 2)
  {
    printf("get_random_node: not enough nodes in list!\n");
    return 0;
  }

  int x = rand();
//printf("grn: rand() = %d  |  (nodes_list->count) = %d \n", x, (nodes_list->count));

  int pos = x % (nodes_list->count);

//printf("get_random_node: random numer = %d\n", pos);


  list_entry_t* e = 0;

  for (uint16_t i = 0; i < nodes_list->count; i++)
  {
    e = e->next;
  }

  if (!e)
  {
    printf("get_random_node: unknown list error!\n");
    return 0;
  }

  struct node* n = (struct node*)e->val;

  if (!n)
  {
    printf("get_random_node: no valid node here!\n");
    return 0;
  }

  return n;
}


int get_node_list(list_t* nodes) // returns count of nodes in the list
{
  // TODO: put in thread / timeout !

  // send request
  int sock = open_connection_string(MASTER_IP, 6453);

  if (sock < 0)
    return -1;

  if (send_request(sock, "RN") < 0)
    return -2;

  list_free(nodes); // delete ols list if exists

  // receive nodes
  while (1) // get nodes until the master disconnects (which means that we have received all nodes)  TODO: run var !
  {
    if (!list_can_add(nodes))
      return list_count(nodes);

    union protocol prot;

    if (get_protocol(sock, &prot) < 0)
      return nodes->count; // the master closes the connection if there are no nodes left - so this is not always an error

    struct node nd;
    if (get_node(sock, &prot, &nd) < 0)
      return nodes->count;

    struct node* n = (struct node*)malloc(sizeof(struct node*));
    memcpy(n, &nd, sizeof(nd));
    //printf("received new node!\n");

    list_append(nodes, n);
  }

  return list_count(nodes);
}


int create_server(uint16_t port)
{
  int server = io_socket();

  if (server < 0)
  {
    printf("create_server: unable to create socket!\n");
    return -1;
  }

  if (io_bind(server, port) < 0)
  {
    printf("create_server: cannot bind on port %d!\n", port);
    return -2;
  }

  if (io_listen(server, 10) < 0)
  {
    printf("create_server: cannot listen!\n");
    return -3;
  }

  return server;
}


int _open_connection(ipv4 ip, uint16_t port)
{
  int sock = io_socket();
  
  if (sock < 0)
  {
    printf("open: cannot create socket!\n");
    return -1;
  }
  
  if (io_sock_connect(sock, ip, port) < 0)
  {
    printf("open: connect to %s failed\n", "TODO: !!!");
    return -2;
  }

  return sock;
}


int open_connection_string(const char* ip, unsigned short port)
{
  ipv4 _ip;

  if (io_get_ipv4(ip, &_ip))
  {
    printf("open_string: cannot convert ip string to ipv4!\n");
    return -2;
  }

  //return -999;

  return _open_connection(_ip, port);
}


int _open_connection_node(struct node* node, uint16_t port)
{
  return _open_connection(node->ip, port);
}


int open_connection_client(struct node* node)
{
  if (!node)
  {
    printf("open_client: node does not exist!\n");
    return -1;
  }

  return _open_connection_node(node, node->client_port);
}


int open_connection_node(struct node* node)
{
  if (!node)
  {
    printf("open_node: node does not exist!\n");
    return -1;
  }

  return _open_connection_node(node, node->node_port);
}


int check_magic(union protocol* prot_in, const char* magic /* feed magic strlen == 2! */)
{
  if (prot_in->data.magic[0] != magic[0] ||
      prot_in->data.magic[1] != magic[1])
  {
    return -1; // not the requested message
  }

  return 0;
}


// TODO: still needed? as there are no non-storage data messages anymore?
int _get_blob(int sock, uint8_t* blob, uint16_t size) // WARNING: don't forget to alloc blob first!
{
  if (size < 1)
    return -1;

  if (!blob)
    return -2;

  if (receive(sock, blob, size) < 0)
    return -3;

  return 0;
}


int get_node_blob(int sock, union protocol *prot_in, struct node_blob* nb_out)
{
  if (check_magic(prot_in, "NB") < 0)
    return -1;

  union msg_node_blob nb;
  size_t size = sizeof(nb);

  if (receive(sock, nb.buffer, size) < 0)
    return -2;

  // FIXME: ugly af
  nb_out->hops = nb.data.hops;
  memcpy(nb_out->blob, nb.data.blob, sizeof(nb.data.blob));

  return 0;
}


int send_node_blob(int sock, struct node_blob* nb_in)
{
  union msg_node_blob nb;
  size_t size = sizeof(nb);

  if (_send_protocol(sock, size, "NB") < 0)
    return -1;

  // FIXME: ugly...:
  nb.data = *nb_in;

  if (out(sock, nb.buffer, size) < 0)
    return -2;

  return 0;
}


int get_client_blob(int sock, union protocol* prot_in, struct client_blob* cb_out)
{
  if (check_magic(prot_in, "CB") < 0)
    return -1;

  union msg_client_blob cb;
  size_t size = sizeof(cb);

  if (receive(sock, cb.buffer, size) < 0)
    return -2;

  memcpy(cb_out->blob, cb.data.blob, 256);

  return 0;
}


int send_client_blob(int sock, struct client_blob* cb_in)
{
  union msg_client_blob cb;
  size_t size = sizeof(cb);

  if (_send_protocol(sock, size, "CB") < 0)
    return -1;

  memcpy(cb.data.blob, cb_in->blob, 256);

  if (out(sock, cb.buffer, size) < 0)
    return -2;

  return 0;
}


int send_node_announce(int sock, struct node_announce* na_in)
{
  union msg_node_announce na;
  size_t size = sizeof(na);

  na.data = *na_in; // as the data struct is of the node ann storage type, copy it directly (data struct can be storage struct, because it contains no pointers)

  if (_send_protocol(sock, size, "NA") < 0)
    return -1;

  if (out(sock, na.buffer, size) < 0)
    return -1;

  return 0;
}


int get_node_announce(int sock, union protocol* prot_in, struct node_announce* na_out)
{
  if (check_magic(prot_in, "NA") < 0)
    return -1;

  union msg_node_announce na;
  size_t size = sizeof(na);

  if (receive(sock, na.buffer, size) < 0)
    return -2;

  *na_out = na.data;

  return 0;
}


int get_node(int sock, union protocol* prot_in, struct node* node_out)
{
  if (check_magic(prot_in, "ND") < 0)
    return -1;

  union msg_node nd;
  size_t size = sizeof(nd);

  if (receive(sock, &nd.buffer[0], size) < 0)
    return -2;

  *node_out = nd.data;

  return 0;
}


int send_node(int sock, struct node* node_in)
{
  union msg_node node;
  size_t size = sizeof(node);

  node.data = *node_in; // as the data struct is of the node storage type, copy it directly (data struct can be storage struct, because it contains no pointers)

  if (_send_protocol(sock, size, "ND") < 0)
    return -1;

  if (out(sock, node.buffer, size) < 0)
    return -1;

  return 0;
}


int send_request(int sock, const char* magic /* feed with strlen == 2! */)
{
  return _send_protocol(sock, 0, magic);
}


void log_not_supported(union protocol* prot_in)
{
  printf("received message %c%c not supported!\n",
         prot_in->data.magic[0],
         prot_in->data.magic[1]);
}
