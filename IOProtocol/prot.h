#pragma once

#include "states.h"

#include <IOClient/api.h>

#include <inttypes.h>


#define MAX_PROT_SOCKS 1024



typedef struct
{
  IO_t *io;
  int io_prot_state; // see states.h, enum io_prot_states
  uint8_t prv_key[400]; // HIS key

} IOProt_t;


void io_prot_init();
IOProt_t *io_prot_connect(IO_t *io, uint8_t *pub_key, uint8_t pub_key_len); // open connection to server which has the public key 'pub_key'
IOProt_t *io_prot_accept(IO_t *io);
int io_prot_receive(IOProt_t *client, void* buffer);
int io_prot_send(IOProt_t *client, void* buffer, uint8_t buffer_size);
