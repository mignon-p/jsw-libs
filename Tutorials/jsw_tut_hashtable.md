# Hash Tables

  

A hash table, put simply, is an abstraction of an array that allows any
value to be used as an index. While an array requires that indices be
integers, a hash table can use a floating-point value, a string, another
array, or even a structure as the index. This index is called the key,
and the contents of the array element at that index is called the value.
So a hash table is a data structure that stores key/value pairs and can
be quickly searched by the key. Because insertion and removal are
operations dependent on the speed of the search, they tend to be fast as
well.

To achieve this magic, a hash table uses a helper function that converts
any object into an integral index suitable for subscripting the array.
For example, to index an array with strings representing the numbers 0
through 9, a hash function might look like this:

```c
unsigned hash(const char key)
{
    return key - '0';
}
```

The first character is expected to always be '0', '1', '2', '3', '4',
'5', '6', '7', '8', or '9', and the trick of subtracting '0' from one of
those characters evaluates to the integer value that the character
represents. This works because you can subtract the lowest value in a
contiguous range from another value in that range and find the
zero-based distance between them. So ('2'-'0') results in 2, ('0'-'0')
results in 0, and ('9'-'0') results in 9.

For those of you who aren't familiar with this trick, it might help to
work with the actual values rather than character constants. Let's take
an arbitrary range of \[12, 16). You want 12 to represent 0, 13 to
represent 1, 14 to represent 2, and so on up to 16 which should
represent 4. To do this you can take any value in the range and subtract
the lowest value, 12, from it. Try it with a calculator.

The hash function shown above can then be used to index a suitable
array. For illustrative purposes, I'll include a complete test program
for you to play with. Notice how problems occur when two entries are
hashed to the same index. This is called a collision, and properly
handling collisions is where most of the effort in implementing hash
tables comes in. Here is the example program:

```c
#include <stdio.h>

/* Find the number of elements in an array */
#define length(a) (sizeof a / sizeof *a)

static size_t hash(const char key)
{
    return key - '0';
}

static void jsw_flush(void);
static int get_digit(char *ch);

static void fill_table(char table[]);
static void show_table(const char table[], size_t size);

int main(void)
{
    /* This is our hash table */
    char table[10] = { 0 };

    fill_table(table);
    show_table(table, length(table));

    return 0;
}

/*
Discard remaining characters in stdin
up to the next newline or end-of-file
*/
void jsw_flush(void)
{
    int ch;

    do
    {
        ch = getchar();
    } while (ch != '\n' && ch != EOF);

    clearerr(stdin);
}

/*
Get a single character digit, '0'-'9'.
Return 1 on success, 0 on input error,
end-of-file, or invalid input
*/
int get_digit(char *ch)
{
    int rc;

    printf("Enter a single digit : ");
    fflush(stdout);

    if (rc = scanf(" % 1[0123456789]", ch) == 1)
    {
        /* At least a newline is left over; clear it all */
        jsw_flush();
    }

    return rc;
}

/* Populate the hash table */
void fill_table(char table[])
{
    char ch;

    while (get_digit(&ch))
    {
        /* Create an index from the character */
        unsigned i = hash(ch);

        if (table[i] != 0)
        {
            /* Duplicate indexes are called collisions */
            printf("That index has been filled\n");
        }
        else
        {
            /* The element is empty; fill it in */
            table[i] = ch;
        }
    }
}

/* Display the contents of the hash table */
void show_table(const char table[], size_t size)
{
    size_t i;

    for (i = 0; i < size; i++)
    {
        /* Mark empty elements as 'null' with the ~ character */
        printf("%c\n", table[i] != 0 ? table[i] : '~');
    }
}
```

Unfortunately, not all cases are as clean cut as this contrived example.
In reality, a hash function will be expected to convert a much broader
range of items that don't fall right into indices. In general, it's not
possible to create such a perfect hashing scheme that every item can be
converted into a unique index. For details about why this is so, see The
Art of Hashing on this website.

In the example above, all of the valid keys mapped perfectly to an
index. However, when you have a larger range of values, two keys that
are not equal may be mapped to the same index. For example, if we
continue to use an array of 10, but want to hash all values in the range
of \[0,20), we might use a hash function like the following:

```c
size_t hash(const unsigned int key)
{
    return key % 10;
}
```

This will cause collisions because there's now way to fit twenty
possible values into only ten indices. For example, 0 and 10 will both
be hashed into an index of 0, 5 and 15 will both be hashed into an index
of 5. There are some situations where a collision can be resolved simply
by replacing the item already present in the table with the new item,
such as in a cache implementation, but most of the time we want both
items to remain in the table, and that is where more sophisticated
measures are needed.

#### Table size and range finding

The hash functions introduced in The Art of Hashing were designed to
return a value in the full unsigned range of an integer. For a 32-bit
integer, this means that the hash functions will return a value in the
range \[0..4,294,967,296). Because it is extremely likely that your
table will be smaller than this, it is possible that the hash value may
exceed the boundaries of the array.

The solution to this problem is to force the range down so that it fits
the table size. A table size should not be chosen randomly because most
of the collision resolution methods require that certain conditions be
met for the table size or they will not work correctly. Most of the
time, this required size is either a power of two, or a prime number.

A table size of a power of two may be desirable on some implementations
where bitwise operations offer performance benefits. The way to force a
value into the range of a power of two can be performed quickly with a
masking operation. For example, to force the range of any value into
eight bits, you simply use the bitwise **AND** operation on a mask of
**0xff** (hexadecimal for 255):

```c
table[hash(key) & 0xff]
```

This is a fast operation, but it only works with powers of two. If the
table size is not a power of two, the remainder of division can be used
to force the value into a desired range with the remainder operator.
Note that this is slightly different than masking because while the mask
was the upper value that you will allow, the divisor must be one larger
than the upper value to include it in the range. This operation is also
slower in theory than masking (in practice, most compilers will optimize
both into the same machine code):

```c
table[hash(key) % 256]
```

When it comes to hash tables, the most recommended table size is any
prime number. This recommendation is made because hashing in general is
misunderstood, and poor hash functions require an extra mixing step of
division by a prime to resemble a uniform distribution. Another reason
that a prime table size is recommended is because several of the
collision resolution methods require it to work. In reality, this is a
generalization and is actually false (a power of two with odd step sizes
will typically work just as well for most collision resolution
strategies), but not many people consider the alternatives and in the
world of hash tables, prime rules.

#### Collision resolution

In a hash table, collisions, where two keys map to the same index, are
resolved by finding another place to put the new key without affecting
the old key, and still allowing for quick lookup of the new key.
Probably the most obvious way to do this is to create a second table for
collisions and place the key at the first empty location. This method is
called resolution by overflow, where an “overflow” table is used to hold
collisions. Insertion into the hash table is relatively simple with this
scheme:

```c
struct hash_table
{
    void **table;
    size_t size;
};

struct overflow_table
{
    void **table;
    size_t size;
    size_t last;
};

int jsw_insert(struct hash_table *table, struct overflow_table *overflow, void *key, size_t size)
{
    /* Get an index for the key */
    size_t h = hash(key, size) % table->size;

    if (table->table[h] != EMPTY)
    {
        /* The index is being used, send the key to overflow */
        if (overflow->last == overflow->size)
        {
            /* The overflow is full, throw an error */
            return 0;
        }

        overflow->table[overflow->last++] = key;
    }
    else
    {
        /* The index is free; fill it in */
        table->table[h] = key;
    }

    return 1;
}
```

Rather than use a cursor to mark the end of the overflow table, we can
also search from the beginning each time. This makes deletions simpler,
but also complicates updating the overflow table:

```c
struct hash_table
{
    void **table;
    size_t size;
};

struct overflow_table
{
    void **table;
    size_t size;
};

int jsw_insert(struct hash_table *table, struct overflow_table *overflow, void *key, size_t size)
{
    /* Get an index for the key */
    size_t h = hash(key, size) % table->size;

    if (table->table[h] != EMPTY)
    {
        size_t i;

        /* Find the first open overflow slot */
        for (i = 0; i < overflow->size; i++)
        {
            if (overflow->table[i] == EMPTY)
            {
                overflow->table[i] = key;
                break;
            }
        }

        if (i == overflow->size)
        {
            /* There are no open slots; throw an error */
            return 0;
        }
    }
    else
    {
        /* The index is free; fill it in */
        table->table[h] = key;
    }

    return 1;
}
```

However, this solution is brittle at best because insertions will be
slow if there is a collision with an overflow table that is large and
mostly full. Searching will be slow for this same reason. If you allow
deletions then what happens when searching for an item that overflowed,
but the key in the hash table that caused the collision was deleted? In
such a case you would have to search the overflow table even if a search
in the hash table showed that that index was empty. So the most obvious
method has subtle problems that would be awkward to fix. Fortunately,
there are other ways to resolve collisions that are much easier to work
with.

 

#### Separate chaining

An experienced programmer will probably consider using an array of
linked lists as the hash table. When there is a collision, the colliding
key can be pushed onto the list, thus preserving both values without a
lot of awkwardness. When searching for a key, the index is hashed and
then the list is searched, and deletion is as simple as deletion from a
linked list. In theory this sounds like a good way to resolve
collisions, and in practice, it is. The method is called separate
chaining, where each linked list is called a chain. These chains are
“separate” because they are not technically a part of the hash table,
simply additions that are tacked on as necessary.

The immediate question is how should the list be structured? Since the
performance of a search depends on the performance of searching a linked
list, it makes sense in theory to keep the lists ordered. That way for a
minor cost during insertion, the average case for both a successful and
unsuccessful search is the same, on average only half the list would be
searched. If the lists are unordered, an unsuccessful search must look
through the entire chain.

This sounds good in theory, but in practice it is more work than
necessary because with a good hash function, the chains are likely to be
short enough that the difference between an ordered list and an
unordered list is negligable. Even more importantly, if new keys are
inserted at the front of the list, aside from very fast insertion
performance, the chain can then be used as a stack, which has desirable
properties for frequency of access. In practice, front insertion is the
easiest to implement, and has the best performance with a hash function
that minimizes collisions.

Another option that has been toyed with is using a binary search tree
instead of a linked list for the chains. This is, much like the ordered
list, more trouble than it is worth, but you are welcome to experiment.
Another variation is a list with a header. My jsw\_hlib uses this
variation to ease gathering the chain length measurement.

A separately chained hash table is simply an array of linked lists. Once
you index the array, it is just a matter of searching, inserting, or
removing in a linked list. Assuming an unordered chain, the search is
almost trivial...almost:

```c
struct jsw_node
{
    void *key;
    struct jsw_node *next;
};

struct hash_table
{
    struct jsw_node **table;
    size_t size;
};

int jsw_find(struct hash_table *table, void *key, size_t size)
{
    /* Get an index for the key */
    size_t h = hash(key, size) % table->size;

    if (table->table[h] != EMPTY)
    {
        /* Find the matching key in the chain, if any */
        struct jsw_node *it = table->table[h];

        while (it != NULL)
        {
            if (compare(key, it->key) == 0)
            {
                return 1;
            }

            it = it->next;
        }
    }

    return 0;
}
```

Insertion into a separately chained hash table can be even simpler than
the search if duplicates are allowed, as the chain does not need to be
searched for a matching key first. Simply create a header if necessary
and push a new node with the key and value onto the front. If duplicates
are not allowed, an extra search needs to be performed to see if the key
is already present in the chain:

```c
struct jsw_node
{
    void *key;
    struct jsw_node *next;
};

struct hash_table
{
    struct jsw_node **table;
    size_t size;
};

int jsw_insert(struct hash_table *table, void *key, size_t size, int allow_duplicates)
{
    struct jsw_node *it;

    /* Get an index for the key */
    size_t h = hash(key, size) % table->size;

    if (!allow_duplicates)
    {
        /* Search for any duplicate keys */
        it = table->table[h];

        while (it != NULL)
        {
            if (compare(key, it->key) == 0)
            {
                /* Stop the insertion if a duplicate is found */
                return 0;
            }

            it = it->next;
        }
    }

    /* Create a new node for the chain */
    it = malloc(sizeof *it);

    if (it == NULL)
    {
        return 0;
    }

    /* Attach the new node to the front of the chain */
    it->key = key;
    it->next = table->table[h];
    table->table[h] = it;

    return 1;
}
```

When removing from a separately chained hash table, a search is required
for the old key. Fortunately, this is the hardest part of the algorithm
and the key is gone, which is a welcome change from the other collision
resolution strategies. However, keep in mind that if duplicates are
allowed, your deletion code might need to remove all occurances of the
key rather than simply the first. For conciseness, I'll leave the
remove\_node function as an exercise. It's nothing more than a normal
linked list removal algorithm:

```c
struct jsw_node
{
    void *key;
    struct jsw_node *next;
};

struct hash_table
{
    struct jsw_node **table;
    size_t size;
};

int jsw_remove(struct hash_table *table, void *key, size_t size, int allow_duplicates)
{
    int item_found = 0;

    /* Get an index for the key */
    size_t h = hash(key, size) % table->size;

    if (table->table[h] == NULL)
    {
        /* The chain is empty, no more work needed */
        return 0;
    }

    /* Always try to remove the first occurrence */
    item_found = remove_node(&table->table[h], key);

    if (item_found && allow_duplicates)
    {
        /* Try to remove all subsequent occurrences */
        while (remove_node(&table->table[h], key))
            ;

        /* At least one was found if we got here */
        item_found = true;
    }

    return item_found;
}
```

Also keep in mind that this is a very basic algorithm and some fairly
simple optimizations can improve the performance. For example, rather
than searching from the beginning each time for removing duplicates,
remove\_node might be edited to give you a pointer to the node before
the one that was deleted, such that it can be re-entrant and the removal
can start again where it left off. Whether this is a practical change or
not depends heavily on the average length of the chains in your hash
table. If the average length is small, the extra code would likely be
more of a handicap than a benefit.

Separate chaining is probably the most popular collision resolution
strategy because it is so flexible. The concept is simple, and while the
implementation may be more complicated than other methods, it has
several advantages:

  - The table has no hard size limit.
  - Performance degrades gracefully with more collisions.
  - The table easily handles duplicate keys.
  - Deletion is simple and permanent.

Of course, there are disadvantages as well:

  - Rebuilding the table if it is resized is slightly harder.
  - Potentially more memory is used because of the links.
  - Potentially slower because links need to be dereferenced.

More often than not, the advantages outweigh the disadvantages. Because
the chains grow and handle collisions gracefully, the need to resize a
hash table is greatly diminished. With a good hash function, you
probably won't see any severe performance hits until the majority of the
chains reach a length where working with them becomes a bottleneck. So
lacking any application-specific requirements, a separately chained hash
table is a good default when you need a hash table.

#### Open addressing

While separate chaining is tempting, there are ways to resolve
collisions without resorting to any dynamically growing alternate data
structure. These solutions resolve collisions by placing the colliding
key into another index in a deterministic way. This process is then
repeated for searches, so that any key can be found even if the actual
index doesn't match the hashed index any longer. These methods are
collectively known as “open addressing”.

#### Linear probing

By far the simplest open addressing strategy is the linear probe. When a
collision is detected, the index is incremented (wrapping around to the
beginning if it reaches the end) until an empty bucket is found.
Typically, the increment is by 1, but other step sizes can be used as
well, though more care should be taken because only a step size of 1
guarantees that all buckets will be encountered without extra
precautions.

In linear probing, keys tend to cluster. That is, several parts of the
table may become full quickly while others remain completely empty.
Because linear probing expects a large number of empty buckets uniformly
interspersed among the used buckets, clusters will cause a large number
of buckets to be walked through before an empty bucket is found. This
slows down search significantly, and in turn slows down insertion and
removal as well. As the load factor increases, so does the effect of
clustering. The following is an example of primary clustering:

    Insert 426 at 8: |   |   |   |   |   |   |   |   |426|   |   |
    Insert 921 at 8: |   |   |   |   |   |   |   |   |416|921|   |
    Insert 714 at 0: |714|   |   |   |   |   |   |   |416|921|   |
    Insert 129 at 8: |714|   |   |   |   |   |   |   |416|921|129|
    Insert 514 at 8: |714|514|   |   |   |   |   |   |416|921|129|
    Insert 287 at 1: |714|514|287|   |   |   |   |   |416|921|129|

Notice how each insertion at index **8** takes longer and longer to find
a an empty bucket. This is primary clustering, and when the table
becomes more than half full, it becomes a severe problem, slowing down
the list exponentially. Primary clustering can be improved with a good
hash function, but the best way to deal with it is to change the step
size.

Searching with linear probing is quite literally, trivial:

```c
struct hash_table
{
    void **table;
    size_t size;
};

int jsw_find(struct hash_table *table, void *key, size_t size)
{
    /* Get the initial index for the key */
    size_t h = hash(key, size) % table->size;

    /* Potentially infinite loop if the table is full */
    while (table->table[h] != EMPTY)
    {
        if (compare(key, table->table[h]) == 0)
        {
            return 1;
        }

        /* Move forward by 1, wrap if necessary */
        if (++h == table->size)
        {
            h = 0;
        }
    }

    return 0;
}
```

The wrap around is performed by incrementing the index and resetting it
to 0 if it hits the table size. This is actually a naive implementation,
and if you'd like you can use modulo arithmetic to the same effect with
h = ( h + 1 ) % table-\>size. This may or may not be more efficient due
to the cost of a division as opposed to the cost of a branch. In general
you should refrain from worrying about such micro-optimizations and use
whichever is clearer.

Also notice how a test for the table being full is not made. This might
be done by saving **table-\>table\[h\]**, and comparing each successive
probe against it, or maintaining a counter of how many iterations have
been made and stopping when that counter hits the table size. Any
technique for finding out if you're at an index that you've already
visited will suffice.

However, this test should only be present as a stopgap against
exceptional failure. Open addressing schemes do not handle high load
factors well and you should avoid filling the table completely. In fact,
the table should half full at most for a generally safe open addressing
load factor. Some open addressing techniques allow more, but half full
is a good time to start worrying.

The step size is almost always **1** with linear probing, but it is
acceptable to use other step sizes as long as the step size is
relatively prime to the table size so that every index is eventually
visited. If this restriction isn't met, all of the indices may not be
visited, and the search may fail or loop infinitely even if the key
exists in the table.

Insertion into a hash table with linear probing is simply a variation of
searching, where the key is inserted into the first empty bucket. If the
table does not allow duplicates, an early return should be made. This
works because the probes will always end up going over a previously
entered duplicate before hitting an empty slot. However, that does
introduce a problem with deletion that we'll look at shortly:

```c
struct hash_table
{
    void **table;
    size_t size;
};

int jsw_insert(struct hash_table *table, void *key, size_t size, int allow_duplicates)
{
    /* Get the initial index for the key */
    size_t h = hash(key, size) % table->size;

    while (table->table[h] != EMPTY)
    {
        if (!allow_duplicates && compare(key, table->table[h]) == 0)
        {
            return 0;
        }

        if (++h == table->size)
        {
            h = 0;
        }
    }

    table[h] = key;

    return 1;
}
```

Removal in any open addressing strategy is where things get interesting.
A direct removal is not possible because future probes could rely on the
key being removed, and if an empty bucket is created where a full bucket
once was, searches will incorrectly fail. All in all, the obvious way to
remove from a hash table with open addressing will destroy the data
structure and cause it to work improperly. With linear probing, it is
possible to remove a key and then re-insert each of the keys in the same
cluster, but that solution is somewhat complicated.

A more general solution that works for all of the open addressing
schemes is to mark a bucket as deleted in such a way so that searches
will not fail. This is a rather hackish solution, but it works well in
practice:

```c
struct hash_table
{
    void **table;
    size_t size;
};

int jsw_remove(struct hash_table *table, void *key, size_t size, int allow_duplicates)
{
    int item_found = 0;

    /* Get the initial index for the key */
    size_t h = hash(key, size) % table->size;

    while (table[h] != EMPTY)
    {
        if (compare(key, table->table[h]) == 0)
        {
            item_found = 1;
            table[h] = DELETED;

            if (!allow_duplicates)
            {
                /* No need to keep searching the cluster */
                break;
            }
        }

        if (++h == table->size)
        {
            h = 0;
        }
    }

    return item_found;
}
```

The catch to this is that if duplicate items are allowed, you can't just
delete the first occurrence and be satisfied. You have to keep going
through the entire cluster and make sure that all occurrences are marked
as deleted. Only the current cluster needs to be searched though,
because once you hit an empty slot, it's guaranteed that no duplicates
could be probed beyond it.

Keep in mind that the insertion algorithm must also be modified to
insert into deleted buckets rather than ignoring them for this technique
to work properly. The change is quite easy. Note that because the
removal algorithm takes care not to leave any duplicates, the insertion
algorithm can safely assume that an EMPTY or DELETED slot marks a valid
insertion point. Inserting in a DELETED slot won't leave an accidental
duplicate further along the probe line.

```c
struct hash_table
{
    void **table;
    size_t size;
};

int jsw_insert(struct hash_table *table, void *key, size_t size, int allow_duplicates)
{
    /* Get the initial index for the key */
    size_t h = hash(key, size) % table->size;

    while (table->table[h] != EMPTY && table->table[h] != DELETED)
    {
        if (!allow_duplicates && compare(key, table->table[h]) == 0)
        {
            return 0;
        }

        if (++h == table->size)
        {
            h = 0;
        }
    }

    table[h] = key;

    return 1;
}
```

Occasionally these deleted items will begin to fill the table as they
take the place of real keys. In that case, the deleted items must be
removed and the entire table rebuilt from scratch. We will cover this
operation shortly.

#### Quadratic probing

In an attempt to alleviate the problem of primary clustering, a
non-constant step size can be used. In general, it has been found that a
quadratically growing step size works well. Simply start with a step
size of **1** and quadratically increase the step size until the desired
index is found. This strategy is called quadratic probing, and the
search algorithm is only slightly more complicated than linear probing,
and insertion and deletion are equally simple changes. Just add a step
and use it to increase the index:

```c
struct hash_table
{
    void **table;
    size_t size;
};

int jsw_find(struct hash_table *table, void *key, size_t size)
{
    size_t step;

    /* Get the initial index for the key */
    size_t h = hash(key, size) % table->size;

    for (step = 1; table->table[h] != EMPTY; step++)
    {
        if (compare(key, table->table[h]) == 0)
        {
            return 1;
        }

        /* Move forward by quadratically, wrap if necessary */
        h = (h + (step * step - step) / 2) % table->size;
    }

    return 0;
}
```

Quadratic probing alleviates primary clustering because probes are no
longer close together according to some small constant. Rather, the
probe sequence covers buckets that are far apart and because the step
size is not constant, primary clustering is no longer apparent. However,
because the probe sequence is always the same from each bucket, the
effect of secondary clustering can be seen. However, secondary
clustering is not nearly as big of a problem as primary clustering, and
the slowdown is hardly noticeable because quadratic probing only works
if the load factor is less than **.5** and the table size is prime. Both
of these requirements go a long way toward improving clustering anyway.

In general, quadratic probing is fast and avoids primary clustering, and
it is a marked improvement over linear probing, but quadratic probing is
restrictive in basic implementations and overcoming those restrictions
can be tricky. For example, basic quadratic probing is only guaranteed
to work if the table size is prime and the load factor is less than .5.
Beyond that you are on your own. While this is a nice step forward,
double hashing is generally a better method for open addressing because
it avoids primary clustering without the annoying restrictions.

#### Double hashing

Double hashing uses two independent hash functions. The first hash
function is used as usual, and the second hash function is used to
create a step size. Because the key itself determines the step size,
primary clustering is avoided. The algorithm is simple, but two rules
must be adhered to for double hashing to work correctly: First, the
second hash cannot ever return 0 or there will be an infinite loop.
Second, much like with linear probing, the table size must either be
prime, or a power of two with the second hash returning an odd number.
Beyond that, the implementation is similar to the other open addressing
methods discussed so far. So similar, in fact, that I'll show only the
search algorithm. The insertion and removal algorithms are identical to
linear probing with the step changes, just like quadratic probing:

```c
struct hash_table
{
    void **table;
    size_t size;
};

int jsw_find(struct hash_table *table, void *key, size_t size)
{
    /* Get the initial index for the key */
    size_t h = hash(key, size) % table->size;

    /* Get the step size */
    size_t step = hash2(key) % (table->size - 1) + 1;

    while (table->table[h] != EMPTY)
    {
        if (compare(key, table->table[h]) == 0)
        {
            return 1;
        }

        /* Move forward by quadratically, wrap if necessary */
        h = (h + step) % table->size;
    }

    return 0;
}
```

#### Coalesced chaining

*Note: This is a modification of my Wikipedia article on [Coalesced
Hashing](https://en.wikipedia.org/wiki/Coalesced_hashing).*

Coalesced chaining is a lesser known strategy that forms a hybrid of
separate chaining and open addressing. In a separately chained hash
table, items that hash to the same index are placed on a list at that
index. This can result in a great deal of wasted memory because the
table itself must be large enough to maintain a load factor that
performs well (typically twice the expected number of items), and extra
memory must be used for all but the first item in a chain (unless list
headers are used, in which case extra memory must be used for all items
in a chain).

Given a sequence of randomly generated three character long strings, the
following table might be generated with a table of size 10:

    (null)
    "clq"
    "qur"
    (null)
    (null)
    "dim"
    "aty" --> "qsu"
    "rhv"
    "qrj" --> "ofu" --> "gcl" -- > "ecd"
    (null)
    (null)

This strategy is effective and very easy to implement. However,
sometimes the extra memory use might be prohibitive, and the most common
alternative using open addressing has uncomfortable disadvantages when
it comes to primary clustering. Coalesced hashing uses a similar
technique as separate chaining, but instead of allocating new nodes for
the linked list, buckets in the table are used, just like open
addressing. The first empty bucket in the table is considered a
collision bucket. When a collision occurs anywhere in the table, the key
is placed in the collision bucket and a link is made between the
colliding index and the collision bucket. Then the next empty bucket is
searched for to give the next collision bucket.

Because the collision bucket moves in a predictable pattern unrelated to
how the keys are hashed, the effect of primary clustering is avoided,
and search times remain efficient. The lists are interleaved around one
another, so they are said to coalesce, hence the name, coalesced
chaining. The code to insert into a coalesced hash table is surprisingly
short for how convoluted the concept seems at first.

```c
struct jsw_node
{
    void *key;
    struct jsw_node *next;
};

struct hash_table
{
    struct jsw_node **table;
    size_t size;
    size_t cursor;
};

int jsw_insert(struct hash_table *table, void *key, size_t size)
{
    /* Get an index for the key */
    size_t h = hash(key, size) % table->size;

    if (table->table[h] == EMPTY)
    {
        /* The slot is empty, no more work needed */
        table->table[h] = key;
    }
    else
    {
        struct jsw_node *it;

        /* Find the next open slot */
        while (table->cursor < table->size && table->table[table->cursor] != EMPTY)
        {
            ++table->cursor;
        }

        if (table->cursor == table->size)
        {
            return 0;
        }

        table->table[table->cursor] = key;

        /* Find the end of the coalesced chain */
        for (it = table->table[h]; it->next != NULL; it = it->next)
            ;

        it->next = table->table[table->cursor];
    }

    return 1;
}
```

Search with coalesced chaining is identical to separate chaining, but
removal is not as simple. In fact, because coalesced chaining is
technically an open addressing strategy, a sentinel marking the bucket
as deleted must be used, just as with the other open addressing schemes.

#### Rehashing

Occasionally, because the operation is very slow, a table needs to be
resized, or deleted sentinel items need to be removed to make way for
real keys. Unfortunately, the only way to do this is to take every key
out of the table and rebuild it completely from scratch. Naturally, this
process is very slow compared to other operations, so it should be
performed very rarely. An example of rehashing can be found in the
[jsw\_hlib](../jsw_hlib) source code.

#### Performance

The performance properties of hash tables are well known. In my quest to
avoid complex mathematical calcuations in my tutorials, and thus cater
to the other 99% of programmers, I will summarize.

A hash table has O(1) performance in the best case. The majority of the
work in implementing a hash table is ensuring that this best case is
also the average case by choosing a good hash function that minimizes
collisions. However, it is more informative to say that the average
expected performance of a hash table is somewhere between O(1) and O(log
N) with a good hash function, and there is a strong bias toward O(1).

The worst case for a hash table is O(N), and there is no way to avoid
the worst case using a basic hash table. If guaranteed good performance
is required then a more suitable structure should be used, such as a
deterministic skip list or balanced binary search tree.

The biggest factor in the performance of a hash table, if the hash
function is good, is the load factor of the table. The load factor is
defined as the result of dividing the number of used buckets by the size
of the table, or M/N where M is the number of used buckets and N is the
table size. All collision resolution strategies perform equally well
when the load factor is **.5**, which is the optimum load factor most of
the time, and each begins to degrade as the load factor increases.

The general recommendation is to maintain a **.5** load factor when
possible, and never to go above a **.7** load factor. However, if memory
is scarce, separately chained hash tables degrade more slowly and more
gracefully than open addressing tables, and you can have higher load
factors. Open addressing schemes are limited by this guideline because
of the pronounced slowdown with larger load factors, and the
implementations of open addressing typically assume that the table is
largely empty.

However, separately chained schemes are not bound as tightly to the
guideline and a table size of the expected number of keys will not
perform as badly as it would with an open addressing strategy. The
search times will simply be a bit slower due to longer chains. For the
speed conscious, more empty buckets means shorter chains, and to ensure
good performance, the a larger table is desirable.

 

#### Comparison

Of these five collision resolution strategies, which one is best? Much
like everything in computer science, it depends. Typically the choice is
between separate chaining and double hashing because those two support
the most flexibility and the best performance properties. Linear probing
breaks down to easily because of primary clustering, quadratic probing
is too restrictive, and coalesced chaining is somewhat of the worst of
both worlds where the complexity of chains are coupled with the
disadvantages of open addressing. In the end a choice should be made on
a case by case basis.

 

#### Disadvantages

Hash tables are not a silver bullet. If the hash function is poor then
there will be excessive collisions and performance will quickly approach
O(N). On top of this, some hash functions are slower than others, and a
slow hash function could increase the constant for O(1) due to expensive
operations and the hash table could be slower than other data structures
that are theoretically slower.

Of the collision resolution strategies discussed, only separate chaining
can easily handle deletion of a key. The open addressing schemes do not
lend themselves well to deleting a key because other keys may rely on it
as part of a search probe sequence.

Separate chaining is also the only collision resolution strategy that
can easily handle duplicate keys. A few moments thought will show this
to be true because to search for duplicates in a chain, only the entire
chain must be searched, whereas with duplicates in an open addressed
table, the entire cluster must be searched for linear probing, and the
entire table for non-linear probing\!

Hash tables are based on arrays, which are notoriously difficult to
resize once they have been allocated. As such, a hash table is said to
have a constant size unless you want to implement the table as a dynamic
array and rehash all of the keys whenever the size changes. This is an
incredibly expensive operation if the table is even remotely large and
should be avoided as much as possible by increasing the size of the
table by a fairly large amount (such as half again the current table
size, or N \* 1.5). That way the reorganizations are minimal for
real-time applications and can be amortized into other operations for
other applications.

The keys in a table need to be rehashed when the size is changed because
each insertion and subsequent search depends on the table size to give
the correct result. If the table size changes, the indices for all of
the previously hashed keys are invalidated because future searches will
no longer work correctly.

Because a hash table is an unordered data structure, certain operations
are difficult and expensive. Range queries, selection, and sorted
traversals are possible only if the keys are copied into a sorted data
structure. There are hash table implementations that keep the keys in
order, but they are far from efficient.

A solution to this problem is a hybrid data structure called a hashlist.
The idea is to use a sorted data structure such as a binary search tree
or sorted linked list as the primary storage unit. Then the keys in the
primary structure are hashed and references to them are stored in the
hash table. This has excellent performance properties, but in practice
the hashlist can be more work than it is worth.

The keys in a hash table are ideally spread in a uniform, pseudorandom
pattern throughout the table. This is a good thing in theory, but in
practice, processors will create a cache of frequently accessed memory
under the assumption that adjacent memory is more likely to be accessed
next than non-adjacent memory. As such, unless a fix like the hashlist
is used, searching in a hash table may result in excessive cache misses
for medium to large tables because the jumping around performed by a
hash table does not fit the usage pattern for efficient caching. For
smaller tables that fit completely in a cache line, any performance
problems are unlikely to be noticeable.

 

#### Conclusion

Hash tables are a powerful and flexible data structure for unordered
searching and storage. There are subtle issues when resolving
collisions, but all in all the hash table is easy to implement and
provides near constant performance in the averages case.

-----

© 2018 - Eternally Confuzzled
