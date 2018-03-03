#pragma once

#include <IOLib/thread.h>


void *_thread_client_connection(void* arg /* socket fd */); // receive from IONodes
