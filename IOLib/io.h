#pragma once

// TODO: clients must send heartbeat, so that IONode does not disconnect / remove the connection !
// TODO: how to create hops count on nb creation? can be manipulated to see which node creates which nb !!
// TODO: increase FD_SETSIZE !!

#include "list.h"

#include <IOSock/net.h>

#include <stdint.h>


#define DEV

#ifdef DEV
  #define MASTER_IP "192.168.133.248"
#else
  #define MASTER_IP "138.201.184.241"
#endif


#ifdef __cplusplus
extern "C" {
#endif


struct node
{
  ipv4 ip;
  uint16_t client_port;
  uint16_t node_port;
};

//struct node_list
//{
//  struct node* n;
//  uint32_t count;
//};





union protocol
{
  struct protocol_data
  {
    char magic[2];
    uint16_t buf_len;
    //uint8_t *buf;
  } data;
  uint8_t buffer[sizeof(struct protocol_data)];
};

int get_protocol(int sock, union protocol* prot);//, const char* magic);
//int send_protocol(int sock, uint16_t msg_size, const char* magic);


// requests:

// GetNodeCount - magic: GC
// RequestNodes - magic: RN


// messages:

union msg_node_count // magic: NC
{
  struct msg_node_count_data
  {
    uint32_t node_count;
  } data;
  uint8_t buffer[sizeof(struct msg_node_count_data)];
};

union msg_node // magic: ND  TODO: arraify ?
{
  struct node data;
  uint8_t buffer[sizeof(struct node)];
};

int send_node(int sock, struct node* node_in);
int get_node(int sock, union protocol* prot_in, struct node* node_out);




struct node_announce
{
  uint16_t client_port;
  uint16_t node_port;
};

union msg_node_announce // magic: NA
{
  struct node_announce data;
  uint8_t buffer[sizeof(struct node_announce)];
};

int send_node_announce(int sock, struct node_announce* na_in);
int get_node_announce(int sock, union protocol* prot_in, struct node_announce* na_out);





struct client_blob // storage
{
  //uint16_t blob_len;
  //uint8_t* blob;
  char prot_magic[2]; // magic of the protocol inside this client_blob's buf
  uint8_t blob[254];
};

union msg_client_blob // magic: CB  message from client to random node (and vise versa) (until it finally arrives to the destination node) - only send the encrypted version of it!
{
  struct client_blob data;
  uint8_t buffer[sizeof(struct client_blob)];
};

int get_client_blob(int sock, union protocol *prot_in, struct client_blob* cb_out);
int send_client_blob(int sock, struct client_blob* cb_in);




struct node_blob // storage ! as msg_node_blob is not meant for storage
{
  uint8_t hops;
  uint8_t blob[256];
};

union msg_node_blob // magic: NB  message from a node to a random node
{
  struct node_blob data;
  uint8_t buffer[sizeof(struct node_blob)];
};

int get_node_blob(int sock, union protocol *prot_in, struct node_blob* nb_out);
int send_node_blob(int sock, struct node_blob* nb_in);


// requests and answers (are inside messages):


// For the first version, the full public key is used to contact an IOClient


//union request_public_key // asks an IOClient for his public key (to get the key for encryption)
//    // NOTE: must be verified by creating the IOAddress from the received public key and compare!
//{
//  struct _data
//  {
//    char magic[3]; // RPK
//  };
//  uint8_t buffer[sizeof(struct _data)];
//};

//union answer_public_key
//{
//  struct _data
//  {
//    char magic[3]; // APK
//    char public_key[4096/8 /* RSA 4096 bits */];
//  };
//  uint8_t buffer[sizeof(struct _data)];
//};





//struct linked_list_entry
//{
//  void* value;
//  struct linked_list_entry* next;
//};

//struct linked_list
//{
//  struct linked_list_entry* entry;
//};

//void linked_list_append(struct node_linked_list* list, struct node_blob *node_blob /* ptr is faster, like a ref */);
//struct node_blob node_linked_list_take_last(struct node_linked_list* list);
//int node_linked_list_count(struct node_linked_list* list);



int init();
int receive(int sock, uint8_t *buf, int size);
int out(int sock, uint8_t* buf, int size);
struct node* get_random_node(list_t *nodes_list);
int get_node_list(list_t *nodes);
int create_server(uint16_t port);
int open_connection_string(const char* ip, unsigned short port);
int _open_connection(ipv4 ip, uint16_t port);
int open_connection_client(struct node* node);
int open_connection_node(struct node* node);
int check_magic(union protocol* prot_in, const char* magic /* feed magic strlen == 2! */);
int send_request(int sock, const char* magic);
void log_not_supported(union protocol* prot_in);
int get_random_number(void* ptr, size_t size);
void get_ip_as_string(ipv4 ip, char *str);
int accept_connection(int sock);
int set_nonblock(int sock);
int wait_socket_read_ready(int sock);



#ifdef __cplusplus
}
#endif
