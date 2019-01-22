#include "select.h"
#include "shared.h"

#include <IOLib/io.h>
#include <IOLib/thread.h>

#include <sys/select.h>
#include <sys/socket.h> // socklen_t
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>



int __handle_client_read(int client_sock)
{
  union protocol prot;

  if (get_protocol(client_sock, &prot) < 0)
  {
    //puts("__handle_client_read: get_protocol failed");
    return -1;
  }

  struct client_blob cb;
  if (get_client_blob(client_sock, &prot, &cb) < 0)
  {
    printf("__handle_client_read: received non-client-blob %c%c on client port!\n", prot.data.magic[0], prot.data.magic[1]);
    return -2;
  }

  //printf("handle_client_connection: magic was: %c%c\n", prot.data.magic[0], prot.data.magic[1]);
  //printf("client: received text was: %s\n", cb.blob);
  //puts("__handle_client_read: handle client read");

  // create node_blob from this received message
  struct node_blob* nb = (struct node_blob*)malloc(sizeof(struct node_blob));
  memcpy(nb->blob, cb.blob, 256); // TODO: on all memcpy: sizeof WHAT?
  nb->hops = (rand() % 100) + 23; // TODO: better RANDOMIZE!   min: 23 - max: 123

  struct client_blob *cb_ptr = (struct client_blob*)malloc(sizeof(struct client_blob));
  memcpy(cb_ptr->blob, cb.blob, 256); // TODO: on all memcpy: sizeof WHAT?

  pthread_mutex_lock(&mutex_shared);
  list_append(node_blob_list, nb); // TODO: return error or ignore?
  list_append(client_blob_list, cb_ptr); // TODO: return error or ignore?
  pthread_mutex_unlock(&mutex_shared);

  //puts("__handle_client_read: done!");

  return 0;
}


void *_handle_client_read(void *arg)
{
  int *_client_sock = (int *)arg;
  int client_sock = *_client_sock;

  if (__handle_client_read(client_sock) == 0) // client has not made any errors? then put him back into the client list
  {
    pthread_mutex_lock(&mutex_shared);
    //puts("_handle_client_read: put client back into the list");
    list_append(client_list, _client_sock);
    pthread_mutex_unlock(&mutex_shared);
  }
  else // client failed
  {
    free(_client_sock);
  }

  return 0;
}


void _select(thread_mgr_t *mgr)
{
  fd_set read_fds;
  int max_client_sock = 0; // would be stdout, INVALID !!
  FD_ZERO(&read_fds);

  // NOTE: clients are NOT removed from the client list, only if a client is read-ready, but after the read was SUCCESSFUL, the client is put back into the client list

  //printf("select.c: %"PRIu64" ", client_list);
  list_entry_t *e = client_list->e;

  while (e)
  {
    int *_client_sock = (int *)e->val;
    int client_sock = *_client_sock;

    FD_SET(client_sock, &read_fds);

    //puts("_select: FOUND!");

    if (client_sock > max_client_sock)
      max_client_sock = client_sock;

    e = e->next;
  }


  if (max_client_sock < 1)
  {
    // nothing to select for..
    //puts("thread_select: nothing to select for");
    return;
  }




  // now select
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0; //1 * 1000 * 1000; // TODO: 250 msecs (is currently 1sec)

  if (select(max_client_sock + 1, &read_fds, 0, 0, &tv) < 1) // TODO: fetch incoming connections HERE - so also select svr socket !
  {
    //puts("thread_select: no buffers to read");
    return;
  }



  int so_error;
  socklen_t len = sizeof(so_error);

  for (int client = 0; client < FD_SETSIZE; client++) // forgot all clients, so look if anything between 0 and FD_SETSIZE is set in the list .. TODO: change that?
  {
    if (FD_ISSET(client, &read_fds)) // is this a client that is ready-read ?
    {
      //printf("thread_select: client %d ready read!\n", client);

      getsockopt(client, SOL_SOCKET, SO_ERROR, &so_error, &len);

      if (so_error != 0)
      {
        printf("thread_select: %s\n", strerror(so_error) /* FIXME: threadsafe ! */);
        continue;
      }


      // READY READ FOR THIS CLIENT !
      //puts("thread_select: wait for free threads..");
      threads_wait_free(mgr);


      // NOTE: because threading would not work, the work-added client is removed from the list and if reading was successful in the handle client read-thread, then the client is put back into the client list
      e = client_list->e;

      while (e)
      {
        if (client == (*(int *)e->val))
        {
          if (_list_remove(client_list, e) != 0)
          {
            puts("_select: error while removing client from client list - work NOT added!");
            return; // FAILED!
          }

          int *client_sock = (int *)malloc(sizeof(int)); // _handle_client_read deletes this after he is done
          *client_sock = client;

          threads_add_work(mgr, _handle_client_read, client_sock);
          //puts("thread_select: work added / client removed from list - vorerst!");

          return; // SUCCESSFULLY DONE!
        }

        e = e->next;
      }

      // if PC goes here, client was not found!
      puts("_select: unknown client .. ignoring");
    }
  }
}


void *thread_select(void* arg) // receive blobs from clients
{
  (void)arg; // no arg(s)

  thread_mgr_t *mgr = threads_create(32, 128);

  while (run)
  {
    /*u*/sleep(1);

    pthread_mutex_lock(&mutex_shared);
    _select(mgr);
    pthread_mutex_unlock(&mutex_shared);
  }

  return 0;
}



















