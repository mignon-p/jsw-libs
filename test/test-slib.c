/*
  Test for jsw-lib skip lists

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

#include "jsw_slib.h"
#include "jsw_rand.h"
#include "test-containers.h"

void *new_container (void)
{
    return jsw_snew (12, (cmp_f) strcmp, (dup_f) strdup, (rel_f) free);
}

void delete_container (void *c)
{
    jsw_sdelete ((jsw_skip_t *) c);
}

bool insert_item (void *c, const char *item)
{
    return (0 != jsw_sinsert ((jsw_skip_t *) c, (void *) item));
}

bool remove_item (void *c, const char *item)
{
    return (0 != jsw_serase ((jsw_skip_t *) c, (void *) item));
}

bool lookup_item (void *c, const char *item)
{
    return (NULL != jsw_sfind ((jsw_skip_t *) c, (void *) item));
}

bool resize_container (void *c)
{
    return true;
}

const char *test_name (void)
{
    return "test-slib";
}

void set_seed (unsigned seed)
{
    jsw_seed (seed);
}
