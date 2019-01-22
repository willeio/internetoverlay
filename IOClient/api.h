#pragma once

#include "rsa.h"

#include <IOLib/io.h> // TODO: minimal header for types (struct client_blob!)

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  int sock;
  //char* pub_key;   <= coming soon
  RSA* prv_key; // ours
  list_t *attempted_connections; // TODO: move me elsewhere??

} IO_t;


void io_init();

IO_t* new_IO();
void free_IO(IO_t* io);


//int io_set_pub_key(IO_t io, char* pub_key);
int io_set_prv_key(IO_t* io, char* prv_key);

int io_connect(IO_t* io);
int io_out(IO_t* io, char* recv_pub_key, void* buffer, uint16_t size);
int io_get(IO_t* io, struct client_blob *cb_out); // NOTE: will alloc buffer for you, please free it after use (I know, not ansi c ...)


#ifdef __cplusplus
}
#endif
