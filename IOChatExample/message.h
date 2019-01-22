#pragma once

#include <stdint.h>



union chat_msg
{
  struct chat_msg_data
  {
    char magic[2];
    uint32_t msg_id;
    uint32_t partner_id; // unique number, one time generated by the sender PER CONTACT, to identify the message to its sender
    uint8_t msg[128];
  } data;
  uint8_t buffer[sizeof(struct chat_msg_data)];
};
