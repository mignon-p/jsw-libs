/*
  Test for jsw-lib containers

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

#ifndef TEST_CONTAINERS_H
#define TEST_CONTAINERS_H

#include <stdbool.h>

void *new_container (void);
void delete_container (void *c);
bool insert_item (void *c, const char *item);
bool remove_item (void *c, const char *item);
bool lookup_item (void *c, const char *item);
bool resize_container (void *c);
const char *test_name (void);
void set_seed (unsigned seed);

#endif  /* TEST_CONTAINERS_H */
