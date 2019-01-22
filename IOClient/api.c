#include "api.h"
#include "rsa.h"

#include <IOLib/io.h>
#include <IOLib/list.h>

#include <stdint.h>
#include <time.h>
#include <stdlib.h> // srand
#include <string.h> // memcpy & memset
#include <ctype.h>


list_t* nodes_list;


void io_init()
{
  init();
  nodes_list = list_create();
}


IO_t* new_IO()
{
  IO_t* io = (IO_t*)malloc(sizeof(IO_t));

  io->sock = -1;
  io->prv_key = 0;
  io->attempted_connections = 0;

  return io;
}


void free_IO(IO_t* io)
{
  (void)io;
  // TODO: anything with: io->prv_key
}


int io_set_prv_key(IO_t* io, char* prv_key)
{
  RSA* prv = rsa_private_init(prv_key);

  if (!prv)
    return -1;

  io->prv_key = prv;
}


int io_connect(IO_t* io)
{
  //srand(time(0));

  int node_count = get_node_list(nodes_list);

  if (node_count < 1)
    return node_count;

//  printf("node count: %d\n", node_count);

  struct node *random_node = get_random_node(nodes_list);

  io->sock = open_connection_client(random_node);

  if (io->sock < 0)
    return -3;

  return node_count;
}


int io_out(IO_t* io, char *recv_pub_key, void* buffer, uint16_t size)
{
  if (size > 256) // TODO: manage to send many packages if > 256
    return -4;

  // RSA encryption
  RSA* pub = rsa_public_init(recv_pub_key);

  if (!pub)
    return -2;

  struct client_blob cb; // TODO: or unsigned char for platform compatibility with OpenSSL ?

  uint8_t tmp_buf[256];
  memset(tmp_buf, 0, 256);
  memcpy(tmp_buf, buffer, size);
//  memcpy(cb.blob, tmp_buf, 256);

  int enc = RSA_public_encrypt(256, tmp_buf, cb.blob, pub, rsa_padding);
  if (enc != 256) // encrypt buffer and write the encrypted buffer directly into the client_blob's blob - 2048 bits
    return -3;

  // TODO: CLEANUP!


  // IO stuff
  if (send_client_blob(io->sock, &cb) < 0)
    return -1;

  /*printf("SENT: '");
  for (int i = 0; i < 256; i++)
  {
    char x = cb.blob[i];

    if (isalpha(x))
      printf("%c", x);
    else
    {
      if (x == 0)
        printf("_");
      else
        printf("?");
    }
  }
  printf("'\n");
  */

  return 0;
}


int io_get(IO_t* io, struct client_blob *cb_out)
{
  if (!io->prv_key)
  {
    printf("io_get: set private key before calling io_get\n");
    return -3;
  }


  // IO stuff
  union protocol prot;

  if (get_protocol(io->sock, &prot) < 0)
    return -1;

  struct client_blob cb;

  if (get_client_blob(io->sock, &prot, &cb) < 0)
  {
    log_not_supported(&prot);
    return -2;
  }

  uint8_t dec_buffer[256];
  memset(dec_buffer, 0, 256);
  //memcpy(cb_out->blob, cb.blob, 256);


//  printf("GOT: '");
//  for (int i = 0; i < 256; i++)
//  {
//    char x = cb.blob[i];

//    if (isalpha(x))
//      printf("%c", x);
//    else
//    {
//      if (x == 0)
//        printf("_");
//      else
//        printf("?");
//    }
//  }
//  printf("'\n");


  if (RSA_private_decrypt(256 /* 2048 bit */, cb.blob, dec_buffer, io->prv_key, rsa_padding) != 256)
    return -3;

  memcpy(cb_out->blob, dec_buffer, sizeof(dec_buffer));

  return 0;
}
