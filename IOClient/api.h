#pragma once

#include "rsa.h"

#include <IOLib/io.h> // TODO: minimal header for types (struct client_blob!)

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


struct IO
{
  int sock;
  //char* pub_key;   <= coming soon
  RSA* prv_key;
};


void io_init();

struct IO* new_IO();
void free_IO(struct IO* io);


//int io_set_pub_key(struct IO* io, char* pub_key);
int io_set_prv_key(struct IO* io, char* prv_key);

int io_connect(struct IO* io);
int io_out(struct IO* io, char* recv_pub_key, void* buffer, uint16_t size);
int io_get(struct IO* io, struct client_blob *cb_out); // NOTE: will alloc buffer for you, please free it after use (I know, not ansi c ...)


#ifdef __cplusplus
}
#endif
