#pragma once

#include <IOLib/io.h>

#include <pthread.h>


pthread_mutex_t mutex_shared;

// TODO: USE A LINKED LIST !!
#define MAX_NODES 2048
struct node* nodes_list[MAX_NODES];

int run;
