#pragma once

#include "list.h"


typedef struct
{
  list_t *thread_list;
  uint16_t elastic; // NIY

} pool_t;


int pool_init(pool_t *pool, uint16_t reserved, uint16_t elastic);
