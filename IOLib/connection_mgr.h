#pragma once

#include "list.h"
#include "connection.h"

#include <pthread.h>


typedef struct
{
  list_t *connection_list;
  pthread_mutex_t mutex;
  //pthread_t *meter_thread;

} connection_mgr_t;


int connection_mgr_init(connection_mgr_t *mgr);

