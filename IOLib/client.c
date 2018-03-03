#include "client.h"

#include <unistd.h>
#include <fcntl.h>


void *_client_mgr_thread(void *arg) // checks if read, write and/or connection is/are ready - also deletes clients which timed out, were metered too high, etc..
{
  client_mgr_t *client_mgr = (client_mgr_t *)arg;

  while (client_mgr->run)
  {
    usleep(1); // sec

    fd_set connections;
    FD_ZERO(&connections);

    pthread_mutex_lock(&client_mgr->mutex);

    list_entry_t *e = client_mgr->client_list->e;
    int connection_count = 0;

    while (e)
    {
      client_t *client = e->val;
      FD_SET(client->sock, &connections);
      connection_count++;
    }

    pthread_mutex_unlock(&client_mgr->mutex);


    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    int result = select(connection_count, &connections, &connections, &connections, &tv);

    if (result > 0)
    {

    }
  }
}


client_mgr_t *client_mgr_create(thread_mgr_t *thread_mgr)
{
  client_mgr_t *client_mgr = (client_mgr_t *)malloc(sizeof(client_mgr_t));

  if (pthread_create(&client_mgr->thread, 0, _client_mgr_thread, client_mgr) != 0)
  {
    puts("client_mgr_create: could not create client manager thread");
    return 0;
  }

  pthread_mutex_init(&client_mgr->mutex, 0);

  client_mgr->client_list = list_create();
  client_mgr->thread_mgr = thread_mgr;
  client_mgr->run = 1;

  return client_mgr;
}


client_t *client_add(client_mgr_t *mgr, int sock)
{
  if (fcntl(sock, F_SETFL, O_NONBLOCK) != 0)
    return 0;

  client_t *client = (client_t *)malloc(sizeof(client_t));

  client->sock = sock;

  list_append(mgr->client_list, client);

  return client;
}


void client_mgr_free(client_mgr_t *mgr)
{
  mgr->run = 0;
  pthread_join(&mgr->thread, 0); // waiting for the thread to finish..
  list_free(mgr->client_list);
  pthread_mutex_destroy(&mgr->mutex);
  free(mgr);
}
