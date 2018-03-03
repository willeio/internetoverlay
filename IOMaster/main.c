#include "shared.h"
#include "client.h"
#include "watcher.h"

#include <IOLib/thread.h>
#include <IOLib/io.h>
//#include <IOLib/client.h>
// TODO:  #include "cfg.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>





// TODO: if ONE of TWO ports are still alive, the master does not delete the node... change that



int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  printf("<IOMaster>\n\n");

#ifdef MinGW
  signal(SIGPIPE, SIG_IGN); // ignore sigpipes (because broken pipes do not harm this program's execution)
#endif


  int svr = create_server(6453);

  if (svr < 0)
    exit(-1);


  run = 1;

  pthread_mutex_init(&mutex_shared, 0);

  nodes_list = list_create();
  list_set_max_entries(nodes_list, 1024);




  pthread_t watcher_thread;
  if (pthread_create(&watcher_thread, 0, thread_watcher, 0) != 0)
  {
    printf("could not start watcher thread\n");
    return -4;
  }




  thread_mgr_t *thread_mgr = threads_create(4, 128);

  while (run)
  {
    /*u*/sleep(1);

    threads_wait_free(thread_mgr);

    int cli = accept_connection(svr);

    if (cli < 0)
    {
      //puts("client: unconnected client");
      continue;
    }

    int* cli_ptr = (int*)malloc(sizeof(int));
    *cli_ptr = cli;
    threads_add_work(thread_mgr, _thread_client_connection, cli_ptr);
    //puts("work added");
  }


  // app term
  threads_free(thread_mgr);
  //pthread_join(watcher_thread, 0);

  printf("normal exit!\n");

  return 0;
}











