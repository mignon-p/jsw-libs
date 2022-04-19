/*
  Test for jsw-lib Andersson trees

    > Created (Patrick Pelletier): April 17, 2022

  This code is in the public domain. Anyone may
  use it or change it in any way that they see
  fit. The author assumes no responsibility for
  damages incurred through use of the original
  code or any variations thereof.

  It is requested, but not required, that due
  credit is given to the original author and
  anyone who has modified the code through
  a header comment, such as this one.
*/

#include <string.h>
#include <stdlib.h>

#include "jsw_atree.h"
#include "test-containers.h"

void *new_container (void)
{
    return jsw_anew ((cmp_f) strcmp, (dup_f) strdup, (rel_f) free);
}

void delete_container (void *c)
{
    jsw_adelete ((jsw_atree_t *) c);
}

bool insert_item (void *c, const char *item)
{
    return (0 != jsw_ainsert ((jsw_atree_t *) c, (void *) item));
}

bool remove_item (void *c, const char *item)
{
    return (0 != jsw_aerase ((jsw_atree_t *) c, (void *) item));
}

bool lookup_item (void *c, const char *item)
{
    return (NULL != jsw_afind ((jsw_atree_t *) c, (void *) item));
}

bool resize_container (void *c)
{
    return true;
}

const char *test_name (void)
{
    return "test-atree";
}

void set_seed (unsigned seed)
{
}
