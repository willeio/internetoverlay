#include "prot.h"

#include <IOClient/api.h>

#include <string.h>






union connection_request_msg
{
  struct connection_request_msg_data
  {
    char magic[2]; // "CR"
    uint8_t part;  // part x / n of the sender's public addr
    uint64_t seq;  // unique sequence number for next msg, for the receiver and v.v.
    uint8_t buf[200]; // public key (part x / n)
  } data;
  uint8_t buffer[sizeof(struct connection_request_msg_data)];
};


union connection_request_ack_msg
{
  struct connection_request_ack_msg_data
  {
    char magic[2];    // "CA"
    uint64_t seq;     // unique sequence number for next msg, for the receiver and v.v. (for assignment)
  } data;
  uint8_t buffer[sizeof(struct connection_request_ack_msg_data)];
};


union connection_msg
{
  struct connection_msg_data
  {
    char magic[2]; // "CM"
    uint8_t len;
    uint8_t buf[255]; // full 255 not possible   FIXME: how much can be taken ??!!
  } data;
  uint8_t buffer[sizeof(struct connection_msg_data)];
};


union connection_msg_ack
{
  struct connection_msg_ack_data
  {
    char magic[2]; // "MA"
  } data;
  uint8_t buffer[sizeof(struct connection_msg_ack_data)];
};






const char *test_part1 = "First  Part";
const char *test_part2 = "Second Part";



#define attempted_connection_t_BUF_SIZE 200

typedef struct
{
  //uint8_t set_parts;  // bit 0 = part 0, bit 1 = part 1
  uint64_t seq;  // unique sequence number for next msg, for the receiver and v.v.

  // uaf:
  uint8_t buf0_set;
  uint8_t buf0[attempted_connection_t_BUF_SIZE]; // public key (part 0 / n)

  // uaf:
  uint8_t buf1_set;
  uint8_t buf1[attempted_connection_t_BUF_SIZE]; // public key (part 1 / n)

} attempted_connection_t; // plis no put on stack






int _check_prot_magic(struct client_blob *cb, const char *magic)
{
  if (cb->prot_magic[0] != magic[0] || // TODO: use strcmp
      cb->prot_magic[1] != magic[1])
    return -1;

  return 0;
}




void _new_ac(IO_t *io, int part, uint64_t seq, char *buffer)
{
  attempted_connection_t *ac = (attempted_connection_t *)malloc(sizeof(attempted_connection_t));

  ac->seq = seq;
  ac->buf0_set = 0;
  ac->buf1_set = 0;

  switch (part)
  {
    case 0:
    {
      memcpy(ac->buf0, buffer, attempted_connection_t_BUF_SIZE);
      ac->buf0_set = 1;
      break;
    }

    case 1:
    {
      memcpy(ac->buf1, buffer, attempted_connection_t_BUF_SIZE);
      ac->buf1_set = 1;
      break;
    }

    default:
    {
      return;
    }
  }

  list_append(io->attempted_connections, ac);
}




int _is_ac_completed(attempted_connection_t *ac)
{
  return !(ac->seq > 0 && ac->buf0_set && ac->buf1_set);
}




attempted_connection_t *_handle_cr(IO_t *io, struct client_blob *cb) // returns 0 if ac is completed after this message
{
  union connection_request_msg cr_msg;

  memcpy(cr_msg.buffer, cb->blob, sizeof(cr_msg.buffer));


  list_entry_t *e = io->attempted_connections->e;

  while (e)
  {
    attempted_connection_t *ac = (attempted_connection_t *)e->val;

    if (ac->seq == cr_msg.data.seq)
    {
      // uaf:
      switch (cr_msg.data.part)
      {
        case 0:
        {
          memcpy(ac->buf0, cr_msg.data.buf, attempted_connection_t_BUF_SIZE);
          ac->buf0_set = 1;
          break;
        }

        case 1:
        {
          memcpy(ac->buf1, cr_msg.data.buf, attempted_connection_t_BUF_SIZE);
          ac->buf1_set = 1;
          break;
        }

        default:
        {
          return 0;
        }
      }


//#error man man ... muss alles modularisiert werden!

      if (_is_ac_completed(ac) == 0)
        return ac;

      return 0; // found, done.
    }

    e = e->next;
  }

  // not found:
  _new_ac(io, cr_msg.data.part, cr_msg.data.seq, cr_msg.data.buf);
  return 0;
}


IOProt_t *io_prot_accept(IO_t *io)
{
  IOProt_t *io_prot = (IOProt_t *)malloc(sizeof(IOProt_t)); // TODO: free !
  io_prot->io = io;
  io_prot->io_prot_state = io_prot_state_connected;

  if (!io->attempted_connections)
    io->attempted_connections = list_create();

  int run = 1;

  while (run)
  {
    struct client_blob cb;

    if (io_get(io, &cb) != 0)
      continue;

    if (_check_prot_magic(&cb, "CR") == 0)
    {
      attempted_connection_t *ac = _handle_cr(io, &cb);

      if (ac)
      {
        memcpy(io_prot->prv_key, ac->buf0, attempted_connection_t_BUF_SIZE);
        memcpy(io_prot->prv_key + attempted_connection_t_BUF_SIZE, ac->buf0, attempted_connection_t_BUF_SIZE);
        return io_prot;
      }

      continue;
    }
  }

  return io_prot;
}







// TODO: timeout for wait-state !
IOProt_t *io_prot_connect(IO_t *io, uint8_t *pub_key, uint8_t pub_key_len) // open connection to server which has the public key 'pub_key'
{
  (void)pub_key;
  (void)pub_key_len; // TODO: respect that when, some day, pub_key is a bin key !

  IOProt_t *io_prot = (IOProt_t *)malloc(sizeof(IOProt_t)); // TODO: free !

  io_prot->io = io;
  io_prot->io_prot_state = io_prot_state_unconnected;

  uint64_t seq = 0;
  get_random_number(&seq, sizeof(uint64_t));


  // send pub 1
  {
    union connection_request_msg cr_msg;
    cr_msg.data.magic[0] = 'C';
    cr_msg.data.magic[1] = 'R';
    cr_msg.data.seq = seq;
    cr_msg.data.part = 0; // first

    memcpy(cr_msg.data.buf, test_part1, strlen(test_part1));

    if (io_out(io_prot->io, pub_key, cr_msg.buffer, sizeof(cr_msg.buffer)) != 0)
      return 0;

    io_prot->io_prot_state = io_prot_state_sent_pub_1;
  }



  // send pub 2
  {
    union connection_request_msg cr_msg;
    cr_msg.data.magic[0] = 'C';
    cr_msg.data.magic[1] = 'R';
    cr_msg.data.seq = seq;
    cr_msg.data.part = 1; // second

    memcpy(cr_msg.data.buf, test_part2, strlen(test_part2));

    if (io_out(io_prot->io, pub_key, cr_msg.buffer, sizeof(cr_msg.buffer)) != 0)
      return 0;

    io_prot->io_prot_state = io_prot_state_sent_pub_1;
  }




  // wait for connection request ack
  // TODO: timeout and send again (client seems to have not received the message) !
  int run = 1;

  while (run)
  {
    struct client_blob cb;

    if (io_get(io_prot->io, &cb) != 0)
      continue;

    union connection_request_ack_msg cra_msg;
    memcpy(cra_msg.buffer, cb.blob, sizeof(cra_msg.buffer) /* cra_msg is smaller - only copy that */);

    if (cra_msg.data.magic[0] != 'C' || // TODO: use strcmp
        cra_msg.data.magic[1] != 'A')
    {
      continue;
    }

    if (cra_msg.data.seq != seq) // not the seq # we provided
    {
      continue;
    }

    break;
  }





  puts("Glueckwunsch .. :) !");
  io_prot->io_prot_state = io_prot_state_connected;

  return io_prot;
}







int io_prot_receive(IOProt_t *client, void* buffer)
{
//union connection_msg
}



int io_prot_send(IOProt_t *client, void* buffer, uint8_t buffer_size)
{
  // send msg
  {
    union connection_msg c_msg;
    cr_msg.data.magic[0] = 'C';
    cr_msg.data.magic[1] = 'M';

    memset(c_msg.data.buf, 0, sizeof(c_msg.data.buf)); // SEC !
    memcpy(c_msg.data.buf, buffer, buffer_size);

    if (io_out(client->io, client->prv_key, cr_msg.buffer, sizeof(cr_msg.buffer)) != 0)
      return -1;

    client->io_prot_state = io_prot_state_sent_msg;
  }

  // wait for send msg ack
  {
    struct client_blob cb;

    if (io_get(io_prot->io, &cb) != 0)
      continue;


#error was ist wenn noch andere nachrichten rein kommen? wie synct man das ??!!


    union connection_msg_ack cmack;
    memcpy(cmack.buffer, cb.blob, sizeof(cmack.buffer) /* cmack is smaller - only copy that */);

    if (cra_msg.data.magic[0] != 'M' || // TODO: use strcmp
        cra_msg.data.magic[1] != 'A')
    {
      continue;
    }

    if (cra_msg.data.seq != seq) // not the seq # we provided
    {
      continue;
    }
  }

  return 0;

}



































