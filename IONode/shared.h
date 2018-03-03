#pragma once

#include <IOLib/io.h>
#include <IOLib/list.h>

#include <pthread.h>


pthread_mutex_t mutex_shared;

list_t* nodes_list;
list_t* node_blob_list;
list_t* client_blob_list;
list_t* client_list;

int run;
