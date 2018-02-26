#include "shared.h"
#include "client.h"
#include "watcher.h"

#include <IOLib/thread.h>
#include <IOLib/io.h>
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
  printf("<IOMaster>\n\n");

#ifdef MinGW
  signal(SIGPIPE, SIG_IGN); // ignore sigpipes (because broken pipes do not harm this program's execution)
#endif


  int svr = create_server(6453);

  if (svr < 0)
    exit(-1);


  run = 1;

  for (int i = 0; i < MAX_NODES; i++)
    nodes_list[i] = 0;

  pthread_mutex_init(&mutex_shared, 0);



  pthread_t watcher_thread;
  if (pthread_create(&watcher_thread, 0, thread_watcher, 0) != 0)
  {
    printf("could not start watcher thread\n");
    return -4;
  }




  threads_init();
  //threads_set_count();  <= TODO: arg!




//  printf("waiting for connections..\n");

  while (run)
  {
    usleep(1);

    threads_wait_free();

    int cli = accept(svr, 0, 0);

    if (cli < 0)
    {
//      printf("client: unconnected client\n");
      continue;
    }

    int* cli_ptr = (int*)malloc(sizeof(int));
    *cli_ptr = cli;
    threads_add_work(thread_client_connection, cli_ptr);
  }


  // app term
  threads_free();
  pthread_join(watcher_thread, 0);

  printf("normal exit!\n");

  return 0;
}











