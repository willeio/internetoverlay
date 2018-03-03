#pragma once

#include <IOLib/list.h>

#include <stdint.h>
#include <pthread.h>


typedef void* (*thread_func_t)(void* arg);

typedef struct thread_work
{
  thread_func_t f;
  void* arg;
  uint8_t priority;
  uint8_t skipped_count;

} thread_work_t;


typedef struct thread_mgr
{
  pthread_mutex_t mutex_work; // is only used by the work threads waiting.. nothing else


  pthread_mutex_t mutex_thread;


  list_t *list_threads;
  list_t *list_work; // todos
  volatile int run;

  pthread_cond_t cond_wait_free; // new work can be queued
  pthread_cond_t cond_wait_work; // new work can be executed by a thread

} thread_mgr_t;


thread_mgr_t *threads_create(uint16_t thread_count, uint16_t queue_size);
void threads_wait_free(thread_mgr_t *mgr);
void threads_add_work(thread_mgr_t *mgr, thread_func_t f, void* arg);
void threads_free(thread_mgr_t *mgr); // call on app term
