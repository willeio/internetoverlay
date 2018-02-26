#pragma once

/*/
 *
 * NOTE: Does not take ownership
 * TODO: iterator!!
 *
/*/

#include <stdint.h>


typedef void* (*eq_func_t)(void*, void*, int*); // equation function for list_remove (to find an entry inside the list)


typedef struct entry
{
  void* val;
  struct entry* next;
} list_entry_t;


typedef struct list
{
  list_entry_t* e;
  uint16_t count;
  uint16_t max; // default: UINT16_MAX;
} list_t;


list_t* list_create();
void list_set_max_entries(list_t* l, uint16_t max);
int _list_remove(list_t* l, list_entry_t* e);
void list_clear(list_t* l);
void list_free(list_t* l);
void* list_last(list_t* l);
void* list_take_last(list_t* l);
int list_can_add(list_t* l);
int list_add(list_t* l, void* val); // quatrillion times faster, because it does not have to find the last entry, but adds it as the first entry
int list_append(list_t* l, void* val);
int list_remove(list_t* l, eq_func_t eq_func, void* eq);
uint16_t list_count(list_t* l);




/*

  EQ example:

typedef struct _bla
{
  int x;
} bla;


void* bla_eq_func(void* val, void* eq, int* result)
{
  *result = ((((bla*)val)->x == (*(int*)eq))) ? 1 : 0;
  return 0;
}



int main()
{
  int x = 0;
  list* l = list_create();

  bla* b1 = (bla*)malloc(sizeof(bla));
  b1->x = 3;
  list_add(l, (void*)b1);



  int eq = 3;



  list_remove(l, bla_eq_func, &eq) ? printf("GOOD!\n") : printf("bad :(\n");



  printf("mallocing.. ");
  void* mcs[UINT16_MAX];

  for (int i = 0; i < UINT16_MAX; i++)
  {
    mcs[i] = (void*)malloc(5); // will be deleted by the list !
  }
  printf("DONE!\n");


  printf("adding.. ");
  int i = 0;
  while (list_append(l, mcs[i++]))
  {}
  printf("DONE!\n");


  printf("list full! count = %d\n", l->count);



  printf("freeing list.. ");
  list_free(l);
  printf("DONE!\n");
  l = 0;



  x++;

  return 0;
}


*/
