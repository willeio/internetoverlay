#pragma once

#include <inttypes.h>


enum io_prot_states
{
  io_prot_state_unconnected = 0,

  // connecting:
  io_prot_state_sent_pub_1,
  //io_prot_state_sent_pub_2,

  // ... => connected

  // connected:
  io_prot_state_connected, // default state

  // sent msg:
  io_prot_state_sent_msg,
  io_prot_state_ack_sent_msg, // => connected

  io_prot_state_rcvd_msg,
  io_prot_state_ack_rcvd_msg, // => connected
};



