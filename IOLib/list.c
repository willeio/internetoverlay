#include "list.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


list_t* list_create()
{
  list_t* l = 0;
  l = (list_t*)malloc(sizeof(list_t));

  l->e = 0;
  l->count = 0;
  l->max = UINT16_MAX;

  return l;
}


void list_set_max_entries(list_t* l, uint16_t max)
{
  l->max = max;
}


int _list_remove(list_t* l, list_entry_t* e)
{
  if (!e || !l->e)
    return -1;

  if (l->e == e) // is list's first entry
    l->e = l->e->next; // is either 0 or a value
  else
  {
    list_entry_t* last = 0;
    list_entry_t* cur = l->e;

    while (cur)
    {
      last = cur;
      cur  = cur->next;

      if (cur == e)
        break;
    }

    if (!last) // l->e has no next (but l->e was not e!)
      return -2;

    last->next = cur->next; // can be 0, don't care
  }

  l->count--;
  //free(e->val);
  free(e);
  return 0;
}


void* list_last(list_t* l)
{
  list_entry_t* e = l->e;

  if (!e)
    return 0;

  while (e->next)
  {
    e = e->next;
  }

  return e;
}


void* list_take_last(list_t* l)
{
  list_entry_t* e = list_last(l);

  if (!e)
    return 0;

  void* val = e->val;

  if (!val)
    return 0;

  if (_list_remove(l, e) < 0)
    return 0; // error, not found!

  return val;
}


void list_clear(list_t* l)
{
  // TODO: improve....

  while (l->e)
    _list_remove(l, l->e);
}


void list_free(list_t* l)
{
  list_clear(l);
  free(l);
}


int list_can_add(list_t* l) // BOOL!!
{
  if (l->count < l->max)
    return 0;

  return -1;
}


int list_add(list_t* l, void* val) // quatrillion times faster, because it does not have to find the last entry, but adds it as the first entry
{
  if (list_can_add(l) != 0)
    return -1; // list full

  list_entry_t *e = (list_entry_t*)malloc(sizeof(list_entry_t));

  e->val = val;
  e->next = l->e; // is either 0 or a valid entry

  l->e = e;
  l->count++;

  return 0;
}


int list_append(list_t* l, void* val)
{
  if (list_can_add(l) != 0)
    return -1; // list full

  list_entry_t *e = (list_entry_t*)malloc(sizeof(list_entry_t));

  e->val = val;
  e->next = 0;

  if (!l->e) // list empty, set first entry
  {
    l->e = e;
    l->count = 1;
    return 0;
  }

  list_entry_t* tmp = l->e;

  while (tmp->next)
  {
    tmp = tmp->next;
  }

  tmp->next = e;
  l->count++;

  return 0;
}


int list_remove(list_t* l, eq_func_t eq_func, void* eq)
{
  list_entry_t* e = l->e;

  for (uint16_t i = 0; i < l->count; i++)
  {
    if (!e)
      break;

    int result = 0;
    eq_func(e->val, eq, &result);

    if (result)     //e->val == (void*)0x9999)
    {
      _list_remove(l, e);
      printf("deleted!\n");
      return 0;
    }

    e = l->e->next;
  }

  return -1;
}


uint16_t list_count(list_t* l)
{
  return l->count;
}
