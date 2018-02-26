#pragma once

#include <stdint.h>


int run;

typedef void* (*threads_func_t)(void* arg);

void threads_init();
void threads_set_count(uint16_t count);
void threads_wait_free();
void threads_add_work(threads_func_t f, void* arg);
void threads_free(); // call on app term
