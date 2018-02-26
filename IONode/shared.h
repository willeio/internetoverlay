#pragma once

#include <IOLib/io.h>
#include <IOLib/list.h>

#include <pthread.h>


pthread_mutex_t mutex_shared;





list_t* nodes_list;
list_t* node_blob_list;
list_t* client_list;




/*
struct node_list nodes_list;

// list type: stack
#define MAX_NODE_BLOBS 128
int node_blob_list_pos;
struct node_blob* node_blob_list[MAX_NODE_BLOBS]; // a list of node blobs that have to be redirected
struct node_blob node_blob_list_take_last();
extern void node_blob_list_add(struct node_blob *blob);

// list type: open array
#define MAX_CONNECTED_CLIENTS 128
int connected_clients_list[MAX_CONNECTED_CLIENTS];
void connected_clients_list_add(int sock);
*/


int run;
