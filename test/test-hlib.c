/*
  Test for jsw-lib hash tables

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

#include "jsw_hlib.h"
#include "test-containers.h"

static int somewhere;

static unsigned hashfunc (const void *key)
{
    /* FNV-1a hash function for NUL-terminated strings */
    const unsigned char *p = key;
    unsigned h = 2166136261;
    int i;

    for (i = 0; p[i]; i++) {
        h = (h ^ p[i]) * 16777619;
    }

    return h;
}

static void *identity (const void *item)
{
    return (void *) item;
}

static void nop (void *item)
{
}

void *new_container (void)
{
    return jsw_hnew (1993, hashfunc, (cmp_f) strcmp, (keydup_f) strdup,
                     identity, (keyrel_f) free, nop);
}

void delete_container (void *c)
{
    jsw_hdelete ((jsw_hash_t *) c);
}

bool insert_item (void *c, const char *item)
{
    return (0 != jsw_hinsert ((jsw_hash_t *) c, (void *) item,
                              (void *) &somewhere));
}

bool remove_item (void *c, const char *item)
{
    return (0 != jsw_herase ((jsw_hash_t *) c, (void *) item));
}

bool lookup_item (void *c, const char *item)
{
    return (NULL != jsw_hfind ((jsw_hash_t *) c, (void *) item));
}

bool resize_container (void *c)
{
    return (0 != jsw_hresize ((jsw_hash_t *) c, 37619));
}

const char *test_name (void)
{
    return "test-hlib";
}

void set_seed (unsigned seed)
{
}
