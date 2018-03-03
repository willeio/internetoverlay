#pragma once

#include "list.h"
#include "thread.h"

#include <pthread.h>


typedef struct
{
  int run;
  list_t *client_list;

  pthread_t thread;
  pthread_mutex_t mutex;

  thread_mgr_t *thread_mgr;

} client_mgr_t;


typedef struct
{
  int sock;
  // TODO: meters ..

} client_t;


client_mgr_t *client_mgr_create(thread_mgr_t *thread_mgr);
client_t *client_add(client_mgr_t *mgr, int sock);
void client_mgr_free(client_mgr_t *mgr);
