#include "connection_mgr.h"


int connection_mgr_init(connection_mgr_t *mgr)
{
  mgr->connection_list = list_create();
  pthread_mutex_init(&mgr->mutex, 0);

  return 0;
}


void connection_mgr_close(connection_mgr_t *mgr, connection_t *con)
{
  pthread_mutex_lock(&mgr->mutex);

  list_entry_t *e = mgr->connection_list->e;

  while (e)
  {
    if (e->val == con)
    {
      close(con->socket);
      free(con);
      _list_remove(mgr->connection_list, e);
      pthread_mutex_unlock(&mgr->mutex);
      return;
    }

    e = e->next;
  }

  pthread_mutex_unlock(&mgr->mutex);
  puts("connection_mgr_close: connection not found");
}
