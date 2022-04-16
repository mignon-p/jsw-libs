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

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "test-containers.h"

#define N_MEMBERS 2048

static bool members[N_MEMBERS];

static const char green[] = "\033[32m";
static const char off[]   = "\033[0m";

static void step (void *c, unsigned i)
{
    char item[20];
    unsigned j = ((unsigned) rand()) % N_MEMBERS;
    snprintf (item, sizeof (item), "%u", j);

    bool found = lookup_item (c, item);
    if (found != members[j]) {
        fprintf (stderr,
                 "%s: step %u, index %u: was %d but should have been %d\n",
                 test_name(), i, j, (int) found, (int) members[j]);
        exit (2);
    }

    const char *failure = NULL;

    if (found) {
        if (! remove_item (c, item)) {
            failure = "remove";
        }
    } else {
        if (! insert_item (c, item)) {
            failure = "insert";
        }
    }

    if (failure) {
        fprintf (stderr,
                 "%s: step %u, index %u: failed to %s\n",
                 test_name(), i, j, failure);
        exit (3);
    }

    members[j] = !members[j];
}

int main (int argc, char **argv)
{
    unsigned seed;
    if (argc < 2) {
        seed = (unsigned) time(NULL);
    } else {
        seed = (unsigned) strtoul(argv[1], NULL, 0);
    }

    const char *name = test_name();
    printf ("%s: seed = %u\n", name, seed);
    srand (seed);               /* set libc's seed */
    set_seed (~seed);           /* set jsw_rand seed */

    void *c = new_container();
    if (! c) {
        fprintf (stderr, "%s: failed to allocate container\n", name);
        return 1;
    }

    unsigned i;
    for (i = 0; i < 2 * N_MEMBERS; i++) {
        step (c, i);
        if (i == N_MEMBERS) {
            if (! resize_container (c)) {
                fprintf (stderr, "%s: failed to resize container\n", name);
                return 4;
            }
        }
    }

    delete_container (c);
    printf ("%s: %sPASS%s\n", name, green, off);

    return 0;
}
