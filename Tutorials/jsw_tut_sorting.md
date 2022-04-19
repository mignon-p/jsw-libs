# Sorting



I'm confident that you already know what sorting is. Not only is it a
concept that saturates computer science, it's also pretty intuitive.
Basically you take things that are out of order, and put them into
order. Examples of sorting are a phone book being ordered by name, or a
coin machine that puts the same type of coins into those neat little
packages so that the bank will give you paper money in exchange. :-) The
idea behind sorting is simple. It makes life easier if things are in
order, not the least important is finding things. It'd be a nightmare to
try and find somebody's phone number if the phone book wasn't ordered by
name.

Beyond the practical uses for sorting, the algorithms are fundamental to
computer science, and fluency in the techniques used by sorting
algorithms provides insight into the craft of programming in many
unexpected areas. Sorting also gives one a certain measure of confidence
in designing and analyzing algorithms because most of the techniques
used appear in the context of sorting as well.

Finally, it has been said that computers have spent more time sorting
data than anything else, so it makes sense to have a strong background
in the concept and theory of sorting, as well as a firm understanding of
the different sorting methods and their relative strengths and
weaknesses. Since sorting efficiently is surprisingly complicated,
programmers should try to avoid adding to the time wasted by using a
poor algorithm for the job.

Many generic sorting libraries already exist and are ready to be used,
such as C's qsort, or C++'s std::sort or std::partial\_sort. However,
these general algorithms are not always the best option. Yes, they
should be first on your list of possible solutions because you're
basically given a tested and debugged algorithm that's easy to use.
That's far superior to writing your own algorithm, testing it, and
debugging it before it can be used with any confidence.

However, there are two distinct problems with the standard library
sorting algorithms. First, they're designed to work for as many cases as
possible, and may not be practical or efficient for your needs. For
example, if qsort is too slow, how to do you fix the problem? You can
find another library, but what are the chances that it's any better than
qsort? What if you need a sorting algorithm that's tailored to your
data? The chances of finding a library that meets your needs are slim
indeed unless your needs are very common.

Second, libraries are generally designed as black boxes, so users of the
functions are have no idea what algorithm is actually being used. This
isn't as much of a problem in the standard C++ library, where the
sorting algorithms have well defined performance attributes that must be
met, but many people are surprised to learn that qsort may actually be
implemented as anything as long as it eventually sorts the array
correctly. The name suggests the quicksort algorithm, but in reality it
could be bubble sort, or worse, a very poor implementation of quicksort
that works for many cases but runs very very slow in common worst cases.

As such, it's beneficial to understand what algorithms are available,
what their advantages and disadvantages are, and how to implement your
own algorithm that's custom tailored to your data if the need arises.
And you can brag that you know half a dozen sorting algorithms and sneer
at your friends who can barely write bubble sort off the top of their
heads. Bragging rights are a pretty darned good reason to learn
something in my opinion. ;-)

#### Sorting Considerations

To sort or not to sort. That's the question, isn't it? Sorting an
unordered array or list is time consuming, even if you do it
efficiently, so it makes sense to avoid sorting when you can. That
usually amounts to using a better data structure such as a hash table
(where sorting probably isn't necessary), or a binary search tree (where
items are sorted by default). Sometimes you don't even need to sort\! It
could be just as fast, or faster, to just brute force your way through
things. It really isn't noticeably slower to use a sequential search on
an array of about 50 items than it is to sort the same array and then
use a binary search on it. To use a crude analogy, it doesn't take a
wrecking ball to destroy a sand castle.

Okay, let's pretend that we want to sort. What about stability?
Stability, you say? What's stability? No, it's not a part of my
psychological diagnosis that's immediately preceeded by “lack of”.
Stability is what happens when two items compare as equal when sorting.
There are two kinds of sorting algorithms concerning stability, stable
and unstable (obviously\!). The only difference is how they treat
duplicate items.

In a stable algorithm, two items that compare as equal will retain their
original relation to each other as in the original unsorted array or
list. This makes absolutely no difference if the items being compared
consist only of, say, integral values. When 2 compares equal to 2, it
really doesn't matter which 2 is placed before the other in the final
sorted sequence. However, consider the more common situation where items
consist of more than one value, and only one of those values is used as
the item for sorting:

```
 |(5:4)|(6:6)|(3:1)|(5:5)|(1:0)|(3:2)|(4:3)|(6:7)|
```

When sorting on the first half of the item, a stable sort has to give a
result that retains the original order of the second half of the item.
In other words, the second values have to be in sorted ascending order
after the sort is finished, or the algorithm isn't stable:

```
 |(1:0)|(3:1)|(3:2)|(4:3)|(5:4)|(5:5)|(6:6)|(6:7)|
```

An unstable sort has no such restriction. Of the algorithms that we'll
cover in this tutorial, the following are naturally stable, but the
others can be made stable at the cost of using more space or taking more
time. For example, selection sort is not usually stable, but can easily
be made so by inserting the new item (shifting all items after its
location to make a hole, like insertion sort) instead of simply swapping
items:

  - Selection sort is stable if you shift instead of swap.
  - Insertion sort is stable.
  - Bubble sort is stable.
  - Counting sort is stable.
  - Merge sort is stable if the merge algorithm is stable.
  - LSD radix sort must be stable to work correctly.
  - MSD radix sort is stable if the underlying sort is stable.

What about performance? This is a tricky area because sorting algorithms
are all across the board. The performance complexities you'll see
primarily are O(N<sup>2</sup>), O(Nlog N), and O(N). It should be
relatively obvious that the upper bound for any sorting algorithm is
infinite as long as it manages to sort the items eventually. Naturally,
this suggests a realistic upper bound of O(N\!), because sorting can be
thought of as picking a sorted permutation of the items. However, the
canonical awful algorithm, called bogosort, is probably the worst
possible sorting algorithm. Bogosort is equivalent to throwing a deck of
cards in the air, then testing to see if they're sorted when you pick
them up at random. Bogosort could potentially never end due to the its
random nature.

Lower bounds aren't as obvious. The lowest possible bound for most
sorting algorithms is Ω(Nlog N). This is because most sorting algorithms
use item comparisons to determine the relative order of items. Any
algorithm that sorts by comparison will have a minimum lower bound of
Ω(Nlog N) because a comparison tree is used to select a permutation
that's sorted. A comparison tree for the three numbers 1,2, and 3 can be
easily constructed:

```
                             1 < 2

               1 < 3                       1 < 3

       2 < 3           3,1,2       2,1,3           2 < 3

  1,2,3     1,3,2                            2,3,1     3,2,1
```

Notice how every item is compared with every other item, and that each
path results in a valid permutation of the three items. The height of
the tree determines the lower bound of the sorting algorithm. Because
there must be as many leaves as there are permutations for the algorithm
to be correct, the smallest possible height of the comparison tree is
logn\!, which is equivalent to Ω(Nlog N).

So are we doomed to Ω(Nlog N) as the best possible performance for
sorting algorithms? Well, yes, if you use item comparisons to determine
relative order. Therefore, the most general of sorting algorithms will
be limited by this lower bound. However, it's possible to meet the safe
lower bound of O(N) for sorting. We'll look at some of these algorithms
later when we look at the counting and radix sorts.

It's easy to see that O(N) is a safe lower bound on general sorting
algorithms for the obvious reasons; you can't sort what you haven't
seen. Well, technically, you could rely on the law of quantum
indeterminacy whereby not observing any of the items, you place them in
a state of superposition and it's both sorted and unsorted, and the
sorting process would be O(1). But as of yet, nobody has suggested a
practical use for such a sorting algorithm, or even whether it would
work correctly in anything but a theoretical sense. :-)

#### Preliminaries

Before we start, there are a few helper functions that I'll be using in
much of the code. Rather than keep repeating myself, I'll supply it all
here. The first is a swap function that takes pointers to two items and
swaps the items:

```c
    void jsw_swap(int *a, int *b)
    {
        int c = *a;
        *a = *b;
        *b = c;
    }
```

Next, it's useful to be able to quickly determine if an array is already
sorted or not for testing, and we'll use it as an optimization feature
later, so here is a function that returns 1 if the array is sorted and 0
if it isn't:

```c
    int is_sorted(int a[], int n)
    {
        int i;

        for (i = 0; i < n - 1; i++)
        {
            if (a[i] > a[i + 1])
            {
                return 0;
            }
        }

        return 1;
    }
```

Finally, some of the algorithms work with linked lists. To save you the
trouble of figuring out what I mean, here's the node and list structure
that I built the lists with:

```c
    struct jsw_node
    {
        int data;
        struct jsw_node *next;
    };

    struct jsw_list
    {
        struct jsw_node *head;
        int size;
    };
```

#### Selection Sort

One of the two most obvious ways to sort data is to find the largest (or
smallest) item in the collection, and then move it to the back. This
straight selection process is repeated for all other items in the list
except for the last of them, then again for all items except for the
last two, and so on until all items are ignored. When all items are
ignored, they are all in their sorted position, and the algorithm is
complete. An example of this simple process follows:

```
 |41|67|34|00|69|24|78|58|62|64<
 |41|67|34|00|69|24|64|58|62<78|
 |41|67|34|00|62|24|64|58<69|78|

 |41|58|34|00|62|24|64<67|69|78|
 |41|58|34|00|62|24<64|67|69|78|
 |41|58|34|00|24<62|64|67|69|78|
 |41|24|34|00<58|62|64|67|69|78|
 |00|24|34<41|58|62|64|67|69|78|

 |00|24<34|41|58|62|64|67|69|78|
 |00<24|34|41|58|62|64|67|69|78|
 <00|24|34|41|58|62|64|67|69|78|
```

The code to do this is as simple as finding the largest item in the
array. A few notes should be made though. If the largest item is the
current end of the array, no swap needs to be done, so we use a
conditional test to avoid a function call and the operations of swapping
an item with itself. This algorithm looks at every item in the array to
find the largest. It has to, because no other order has been imposed on
the array to make selection easier, so it's slow:

```c
    void jsw_selection(int a[], int n)
    {
        while (--n > 0)
        {
            int i, max = n;

            for (i = 0; i < n; i++)
            {
                if (a[i] > a[max])
                {
                    max = i;
                }
            }

            if (max != n)
            {
                jsw_swap(&a[n], &a[max]);
            }
        }
    }
```

There are a few important little factoids about straight selection sort
that you need to remember. First, all of the items have to be present
before the sort can begin. That means that selection sort isn't a viable
solution for sorting items as they come though an input stream or a
random number generator. The array has to be completely filled before
you can sort it. Second, straight selection sort is slow because the
process of selection is slow. Since straight selection is an O(N)
operation, and the algorithm does it approximately N times, straight
selection sort is a quadratic sorting algorithm. Quadratic sorts are the
slowest practical algorithms you'll see in the real world. Finally,
selection sort is very efficient in terms of data movement. If
comparisons are significantly cheaper than swaps, selection sort
performs well.

As presented above, the straight selection sort is not stable. That is,
if the items have more data attached to them that aren't a part of the
comparison, the relative order of duplicate items is unspecified by the
algorithm. To make the sort stable such that duplicate items retain
their relative order, we can select the last of any duplicates and shift
instead of swap Of course, this removes the primary benefit of selection
sort, lack of data movement, because we're moving items around in the
shift:

```c
    void jsw_selection(int a[], int n)
    {
        while (--n > 0)
        {
            int i, max = n;

            for (i = 0; i < n; i++)
            {
                if (a[i] >= a[max])
                {
                    max = i;
                }
            }

            if (max != n)
            {
                int save = a[max];

                for (i = max; i < n; i++)
                {
                    a[i] = a[i + 1];
                }

                a[n] = save;
            }
        }
    }
```

Let's look at how that works with a quick trace by adding some extra
data to each item and throwing in a few duplicates. Notice how the
secondary numbers are in the same order after the sort as before the
sort. For example, 69,4 is in front of 69,5 both before and after the
algorithm touches them:

```
 |41,0|41,1|34,2|00,3|69,4|69,5|78,6|78,7|62,8|64,9<
 |41,0|41,1|34,2|00,3|69,4|69,5|78,6|62,8|64,9<78,7|
 |41,0|41,1|34,2|00,3|69,4|69,5|62,8|64,9<78,6|78,7|

 |41,0|41,1|34,2|00,3|69,4|62,8|64,9<69,5|78,6|78,7|
 |41,0|41,1|34,2|00,3|62,8|64,9<69,4|69,5|78,6|78,7|
 |41,0|41,1|34,2|00,3|62,8<64,9|69,4|69,5|78,6|78,7|
 |41,0|41,1|34,2|00,3<62,8|64,9|69,4|69,5|78,6|78,7|
 |41,0|34,2|00,3<41,1|62,8|64,9|69,4|69,5|78,6|78,7|

 |34,2|00,3<41,0|41,1|62,8|64,9|69,4|69,5|78,6|78,7|
 |00,3<34,2|41,0|41,1|62,8|64,9|69,4|69,5|78,6|78,7|
 <00,3|34,2|41,0|41,1|62,8|64,9|69,4|69,5|78,6|78,7|
```

#### Heap Sort

The straight selection sort is probably the simplest sorting algorithm,
but that simplicity is really the only benefit. Selection sort has
quadratic time complexity, which is the worst complexity of the
practical sorting algorithms that you'll see in the real world. There
are a handful of quadratic sorting algorithms in common use, and while
selection sort isn't the worst of them, you're really comparing the
lowest common denominators with each other. Maybe we should work smarter
instead of trying to pick the best of the worst. ;-)

The very name of selection sort suggests a way to improve the algorithm.
Since the biggest problem with selection sort is the linear time it
takes to select the largest item, we can find the most improvement by
solving the selection problem more efficiently. In real code, we would
do that with a priority queue, where items are ordered so that
extracting the largest item is quick and easy. The code to do that if we
assume an existing priority queue library is simplicity itself:

```c
    void jsw_selection(int a[], int n)
    {
        pqueue pq;
        int i;

        for (i = 0; i < n; i++)
        {
            pq_insert(pq, a[i]);
        }

        while (!pq_empty(pq))
        {
            a[--n] = pq_max(pq);
        }
    }
```

Okay, but what if that nifty little library doesn't already exist? Well,
we need to do it manually. The best known priority queue implementation
is implemented with a max heap. The heap data structure has two very
simple rules. It's a complete binary tree where the children of a node
cannot be larger (or smaller if you want a min heap) than their parent.
If you have a valid max heap then the largest item is the root of the
tree. This structure can be easily converted into an array because the
tree is complete, so there are no gaps:

```
 |962|500|724|467|464|334|478|358|41|169|

 is equivalent to

                           962

                    /              \

                500                   724

           /           \            /     \

       467               464    334         478

     /     \           /

 358         41    169
```

By converting an array into a heap structure, we can then easily extract
the largest item from index 0. After doing that, the heap needs to be
fixed so that it's still a valid heap again, and the process is
repeated. This method of sorting is called heap sort, and it's one of
the faster sorting algorithms with a worst case that has the same time
complexity as the average case\! By working smarter with the selection
problem, we've turned one of the worst sorting algorithms into one of
the best. Not bad, eh? :-)

Cool, so how do we do all of that junk? Well, naturally we need to
figure out how to force an array to have the heap property. Basically,
we can say that for each index **i**, the child nodes are at **i \* 2 +
1** and **i \* 2 + 2**. In the above tree, we can see that 467 is a
child of 500. In the array, 500 is at index 1 and **1 \* 2 + 1** is
index 3, which is where 467 is in the array. Try this with any of the
indices and you'll see that this property holds for all of them. Also
notice that the nodes with children are roughly the first half of the
array and the nodes without children are the last half. So to build a
heap, all we need to do is force the heap property from the lowest nodes
to the root. Let's look at an example:

```
 |1|67|34|0|69|24|78|58|62|64|


                         1

                 /              \

              67                  34

         /          \           /    \

       0              69     24        78

     /   \          /

 58        62    64
```

To build a heap out of this array (I'll also include a tree
representation to make things more obvious), we need to work with
subtrees first. We start with 69 and its one child. Since 64 is smaller
than 69, the heap property holds. Then we move to 0 and its children. 62
is the largest of the three, so we swap 62 and 0. Note that the relative
order of the children is irrelevant:

```
 |1|67|34|62|69|24|78|58|0|64|


                         1

                 /              \

              67                  34

         /          \           /    \

       62             69     24        78

     /    \         /

 58         0    64
```

Now we move up a level and all the way to the right. In the array we're
just decrementing the parent index though. At 34, we test the 3 nodes
and find that 78 is the largest. So we swap 78 and 34 to restore the
heap property:

```
 |1|67|78|62|69|24|34|58|0|64|


                         1

                 /              \

              67                  78

         /          \           /    \

       62             69     24        34

     /    \         /

 58         0    64
```

Now we go left to 67 and its two children. Since 69 is the greatest of
the children and is greater than 67, we swap 67 and 69:

```
 |1|69|78|62|67|24|34|58|0|64|


                         1

                 /              \

              69                  78

         /          \           /    \

       62             67     24        34

     /    \         /

 58         0    64
```

Finally, at the root of the tree, testing 1 and its children we find
that 78 is greater than both 1 and 69, so we make 78 the new root of the
tree. Uh oh, this isn't a valid heap though. 78 is greater than 1, but 1
isn't greater than either 24 or 34, so we've got a problem. It appears
that we can't just do local changes to a parent and its two children, we
need to push the parent down as long as it violates the heap property.
We can do this because we build a heap from the bottom up, so the lower
subtrees are always valid heaps unless a change higher up the tree
violates it. Let's do the final step by fixing 78's right subtree:

```
 |1|69|78|62|67|24|34|58|0|64|


                        78

                 /             \

              69                 34

         /          \          /    \

       62             67    24        1

     /    \         /

 58         0    64
```

Now we have a valid heap\! Let's write a **jsw\_do\_heap** function that
walks down the tree, making sure that the heap property is met. This is
a fairly obvious method. The outer loop of **jsw\_do\_heap** handles
cases where multiple parents need to be fixed down the tree. The first
step inside the loop finds the largest child, the second step swaps the
largest child and the parent if the child is larger. Notice how the **i
\* 2 + 1** trick is used to find the first child. **jsw\_heapsort** is
just a translation of the priority queue pseudocode:

```c
    void jsw_do_heap(int a[], int i, int n)
    {
        int k = i * 2 + 1;

        while (k < n)
        {
            if (k + 1 < n && a[k] < a[k + 1])
            {
                ++k;
            }

            if (a[i] < a[k])
            {
                jsw_swap(&a[i], &a[k]);
            }

            k = ++i * 2 + 1;
        }
    }

    void jsw_heapsort(int a[], int n)
    {
        int i = n / 2;

        while (i-- > 0)
        {
            jsw_do_heap(a, i, n);
        }

        while (--n > 0)
        {
            jsw_swap(&a[0], &a[n]);
            jsw_do_heap(a, 0, n);
        }
    }
```

This is a surprisingly simple algorithm for how good heap sort is
supposed to be, but it's not very fast, and we can do much better with
only minor changes to **jsw\_do\_heap**. The current algorithm walks
down every node in the tree until the end of the array. That means we're
doing a lot of unnecessary work if only one push down is required in an
array of 1,000,000 items. We also don't need to call jsw\_swap in the
loop. We can simply save the first root that we're pushing down, move
children into its place, and after the loop is finished, put that saved
node into its final resting place:

```c
    void jsw_do_heap(int a[], int i, int n)
    {
        int k = i * 2 + 1;
        int save = a[i];

        while (k < n)
        {
            if (k + 1 < n && a[k] < a[k + 1])
            {
                ++k;
            }

            if (save >= a[k])
            {
                break;
            }

            a[i] = a[k];
            k = ++i * 2 + 1;
        }

        a[i] = save;
    }
```

That's much faster than our last attempt, but because we're only
incrementing **i** by 1 each time, we're basically fixing the entire
heap when we only need to fix the subtree that we swapped with, not both
subtrees. So by making the simple change of setting **i** to **k**
instead of incrementing it, we improve the speed of the algorithm by
several orders of magnitude. **jsw\_heapsort** doesn't change throughout
all of these improvements, and we come to the final (and fastest)
incarnation of heapsort for this tutorial:

```c
    void jsw_do_heap(int a[], int i, int n)
    {
        int k = i * 2 + 1;
        int save = a[i];

        while (k < n)
        {
            if (k + 1 < n && a[k] < a[k + 1])
            {
                ++k;
            }

            if (save >= a[k])
            {
                break;
            }

            a[i] = a[k];
            i = k;
            k = i * 2 + 1;
        }

        a[i] = save;
    }
```

Okay, I'm a liar. :-) We'll look at another alternative before moving
on. A lot of people are surprised to learn that heap sort can be
implemented any number of ways. The heap property is a very flexible
rule when it comes to how you want to enforce it, so there can be a lot
of variation in how heap sort is implemented. The best way to find
alternative implementations is to think about how heaps work and look
for patterns. One pattern in particular that we will look at is the
relationship between each index and its child indices. If you think
about the simple calculation for finding a child, you'll come up with
the following pattern:

```
 |41|67|34|0|69|24|78|58|62|64|
 |41|67|34|
    |67|  |0|69|
       |34|    |24|78|
          |0|        |58|62|
            |69|           |64|
```

This shows the parents and their direct children. You can verify that
the right children go with the right parents by using the **i \* 2 + 1**
trick. 41 is index 0, 0 \* 2 + 1 is 1 and 0 \* 2 + 2 is 2. Okay, that's
an easy one, how about the last parent and child? 69 is index 4, 4 \* 2
\+ 1 is 9, which is the index for 64. So we know that the pattern matches
what we were doing by finding k with i \* 2 + 1. But since the parent
index increments by 1 each time and the index of the first child
increments by 2, we can work directly with the pattern by using two
indices (once again, **jsw\_heapsort** remains unchanged):

```c
    #define jmax(p,q) (*(p) > *(q) ? (p) : (q))
    #define maxof3(p,q,r) jmax((p), jmax ((q), (r)))

    void jsw_do_heap(int a[], int i, int n)
    {
        int k;

        for (k = i + 1; k < n; i++, k += 2)
        {
            if (k == n - 1)
            {
                if (a[k] > a[i])
                {
                    jsw_swap(&a[i], &a[k]);
                }
            }
            else
            {
                jsw_swap(&a[i], maxof3(&a[i], &a[k], &a[k + 1]));
            }
        }
    }
```

But this is really two steps backward since it's even slower than our
first attempt at heapsort, and implementing the same optimizations that
we used before would make the code much more complicated, so this is
really a substandard solution even though it's intellectually
interesting to play around with alternatives. :-) I like to think that
if you understand the variations we've looked at, you'll have a good
feel for heap sort.

One problem that heap sort has (though you won't see it here) is that
the traditional algorithm assumes 1-based array indices, while C uses
0-based arrays. That's proven to be a very frustrating problem because
getting a 1-based algorithm to work with 0-based arrays can be tricky.
As such, you'll probably see different tricks that avoid the issue, like
ignoring the 0th index, or shifting the array so that the 0th index
actually refers to one element in front of the array. The former is
awkward and the latter is undefined by C. Other attempts are just wrong
in subtle ways, and while they may seem to work most of the time,
they'll break, usually at the worst possible time. ;-) No worries here
though, all of my implementations are written specifically with 0-based
arrays in mind. You're welcome.

#### Insertion Sort

The other most obvious way to sort a collection of items is by
insertion. Insertion takes each new item, then locates the current
ordered position in the collection and inserts the item there. The
common example is sorting a bridge hand, but I don't play bridge and I
don't know anyone that does, so an execution trace on our list of
numbers as an example would fare better:

```
  1)  |41|
  2)  |41|67|
  3)  |34|41|67|
  4)  |00|34|41|67|
  5)  |00|34|41|67|69|
  6)  |00|24|34|41|67|69|
  7)  |00|24|34|41|67|69|78|
  8)  |00|24|34|41|58|67|69|78|
  9)  |00|24|34|41|58|62|67|69|78|
 10)  |00|24|34|41|58|62|64|67|69|78|
```

Each new item is moved into the correct position as it arrives. Unlike
sorting by selection, this suggests that sorting by insertion is well
suited to sorting an unknown number of items, and that the items don't
need to be present before sorting can begin. So acquiring items through
a stream or a generator is perfectly acceptable when sorting by
insertion.

However, that very sequential nature suggests that sorting by insertion
has potential performance issues when used with arrays. Insertion into
the middle of an array requires that a hole be created by shifting all
items after the hole to the right. For a large array, that can be
incredibly slow and expensive. On the other hand, sorting by insertion
into a linked list can be far more efficient in theory because the
shifting step is not required.

Note that sorting by insertion can be done in-place without using any
extra memory. Using insertion sort on an existing array of items would
result in the following execution:

```
  1)  |41>67|34|00|69|24|78|58|62|64|
  2)  |41|67>34|00|69|24|78|58|62|64|
  3)  |34|41|67>00|69|24|78|58|62|64|
  4)  |00|34|41|67>69|24|78|58|62|64|
  5)  |00|34|41|67|69>24|78|58|62|64|
  6)  |00|24|34|41|67|69>78|58|62|64|
  7)  |00|24|34|41|67|69|78>58|62|64|
  8)  |00|24|34|41|58|67|69|78>62|64|
  9)  |00|24|34|41|58|62|67|69|78>64|
 10)  |00|24|34|41|58|62|64|67|69|78|
```

The code to do this is pretty simple. All we need to do is pick each
item in the array, then make a hole where it needs to go and insert it.
In fact, insertion sort is considered to be one of the simplest sorting
algorithms both in concept and in implementation:

```c
    void jsw_insertion(int a[], int n)
    {
        int i;

        for (i = 1; i < n; i++)
        {
            int j, save = a[i];

            for (j = i; j >= 1 && a[j - 1] > save; j--)
            {
                a[j] = a[j - 1];
            }

            a[j] = save;
        }
    }
```

Unfortunately, being one of the simplest sorting algorithms also means
that insertion sort is also one of the least efficient. It's in the
class of sorting algorithms that have quadratic time complexity, which
means that it doesn't scale well to large arrays. However, insertion
sort has one really awesome feature that makes it useful as a part of
more sophisticated algorithms: insertion sort is blazing fast on arrays
that are sorted or almost sorted. This makes it ideal for finishing up a
partial quicksort, which we'll see shortly.

Insertion sort on a linked list is almost as simple as insertion sort on
an array. With linked lists we may not be able to go backward, but
that's hardly a stumbling block since we can easily save the sorted
items in a temporary list by extracting the nodes from the original
list, inserting them into the temporary list, then pointing the original
list pointer back to the temporary list before returning. The temporary
list only has the overhead of an extra variable, so it's space
efficient:

```c
    void jsw_insertion(struct jsw_list *list)
    {
        struct jsw_node result = { 0 };
        struct jsw_node *i, *step, *next;

        for (step = list->head; step != NULL; step = next)
        {
            next = step->next;

            for (i = &result; i->next != NULL; i = i->next)
            {
                if (i->next->data > step->data)
                {
                    break;
                }
            }

            step->next = i->next;
            i->next = step;
        }

        list->head = result.next;
    }
```

#### Shell Sort

While straight insertion is particularly efficient on already sorted or
almost sorted arrays or lists, it's still a quadratic sort because the
overall result only sorts the items by one position at a time. Now,
while this may sound like a given, it's possible to force the algorithm
to move items over wider ranges by dividing the array into multiple
interleaved groups. For example, dividing the insertion sort example
into two groups of five, then sorting each corresponding value in those
groups, then combining the groups and sorting the one group results in
the following execution:

```
  1)  |41|67|34|00|69|   |24|78|58|62|64|
  2)  |24|67|34|00|69|   |41|78|58|62|64|
  3)  |24|67|34|00|69|   |41|78|58|62|64|
  4)  |24|67|34|00|69|   |41|78|58|62|64|
  5)  |24|67|34|00|64|   |41|78|58|62|69|
  6)  |00|24|67|34|64|41|78|58|62|69|
  7)  |00|24|34|64|67|41|78|58|62|69|
  8)  |00|24|34|41|64|67|78|58|62|69|
  9)  |00|24|34|41|58|64|67|78|62|69|
 10)  |00|24|34|41|58|62|64|67|78|69|
 11)  |00|24|34|41|58|62|64|67|69|78|
```

Hmm, not much better, is it? How about we divide the array into four
groups instead of two? That might be better:

```
  1)  |41|67|   |34|00|69|   |24|78|   |58|62|64|
  2)  |24|67|   |34|00|69|   |41|78|   |58|62|64|
  3)  |24|00|   |34|62|69|   |41|67|   |58|78|64|
  4)  |24|00|   |34|62|64|   |41|67|   |58|78|69|
  5)  |00|24|34|62|64|41|67|58|78|69|
  6)  |00|24|34|41|62|64|67|58|78|69|
  7)  |00|24|34|41|58|62|64|67|78|69|
  8)  |00|24|34|41|58|62|64|67|69|78|
```

Sorting interleaved groups is the idea behind the diminishing increment
sort, better known by the popular name Shell sort. Donald Shell
capitalized on the fact that sorting by insertion is very fast for small
arrays that are almost sorted. He then introduced the concept of
breaking the array into increasing groups of smaller numbers (usually
called the h-increment) of items. As long as the groups eventually
shrink down to N groups of 1, the final step is a straight insertion and
the algorithm terminates successfully:

```c
    void jsw_shellsort(int a[], int n)
    {
        int i, h = 1;

        while (h <= n / 9)
        {
            h = 3 * h + 1;
        }

        while (h > 0)
        {
            for (i = h; i < n; i++)
            {
                int j, save = a[i];

                for (j = i; j >= h && a[j - h] > save; j -= h)
                {
                    a[j] = a[j - h];
                }

                a[j] = save;
            }

            h /= 3;
        }
    }
```

The only difference between this code and the insertion sort is that
we've added an extra loop around the insertion sort that handles the
step increments and replaced references to 1 with h. By moving items
that are separated by a large number of items, they get closer to their
final resting place faster. The examples above don't do the algorithm
justice, but until quicksort was developed, Shell sort was the fastest
sorting algorithm available.

A careless choice of values for the h-increments, such as powers of two,
could result in strikingly bad performance, while a seemingly arbitrary
sequence can result in very good performance. To date, nobody has
determined what the very best sequence is. Two sequences known to be
good are the square of the Fibonacci sequence (1,4,9,25,64,169,441,...),
and a sequence recommended by Donald Knuth that's fast and easy to
calculate (1,4,13,40,121,364,1093,3280,9841...). While there are better
sequences, these two are not substantially worse, and they have the
benefit of being easy to create with minimal cost. The function above
uses Knuth's sequence.

The Shell sort is simple to understand, easy to implement, and has good
performance for small to medium arrays, but its time complexity is
difficult to analyze and depends heavily on the sequence of h-increments
chosen. In general, Shell sort should be the first sort used because
it's easier to implement than the higher order sorting algorithms (with
the possible exception of heap sort) and has good performance. If Shell
sort turns out to be too slow, then some of the faster algorithms can be
considered.

#### Bubble Sort

Sorting by exchanging is just that. Pairs of items that are out of order
are exchanged repeatedly until no more pairs exist. When no more pairs
exist that are out of order, the array must be completely sorted.
Sorting by exchanging is particularly dangerous because the efficient
solutions aren't nearly as obvious as the hideously inefficient ones.
This is true for all sorting categories, but sorting by exchanging has
the great honor of boasting the worst sorting algorithm that you might
see in the real world (if you're unlucky). Here is a look at the most
obvious algorithm:

```
  1)  |41|67|34|00|69|24|78|58|62|64|
  2)  |41>34|67|00|69|24|78|58|62|64|
  3)  |41|34>00|67|69|24|78|58|62|64|
  4)  |41|34|00|67>24|69|78|58|62|64|
  5)  |41|34|00|67|24|69>58|78|62|64|
  6)  |41|34|00|67|24|69|58>62|78|64|
  7)  |41|34|00|67|24|69|58|62>64|78|
  8)  >34|41|00|67|24|69|58|62|64|78|
  9)  |34>00|41|67|24|69|58|62|64|78|
 10)  |34|00|41>24|67|69|58|62|64|78|
 11)  |34|00|41|24|67>58|69|62|64|78|
 12)  |34|00|41|24|67|58>62|69|64|78|
 13)  |34|00|41|24|67|58|62>64|69|78|
 14)  >00|34|41|24|67|58|62|64|69|78|
 15)  |00|34>24|41|67|58|62|64|69|78|
 16)  |00|34|24|41>58|67|62|64|69|78|
 17)  |00|34|24|41|58>62|67|64|69|78|
 18)  |00|34|24|41|58|62>64|67|69|78|
 19)  |00>24|34|41|58|62|64|67|69|78|
```

As you can clearly see, this particular exchange sort is exceptionally
inefficient, even when compared to straight selection and insertion,
which have similar time complexity properties, even when the execution
left out any of the steps that didn't result in a swap. Hey, I don't
want to spend half my life typing out those executions, so I made it
shorter. In the process I made the algorithm look better than it is, and
it **STILL** looks bad. ;-)

Unfortunately, because the algorithm is simple and easy to understand
this so-called “bubble sort” is commonly taught to beginning programmers
as the first sorting algorithm, and often as a first algorithm entirely.
I think that's a shame, because bubble sort only has practical use in
very specific and unlikely circumstances, and for everything else it
just sucks too much. For comparison and completeness, I'll provide an
implementation of the bubble sort as it's usually taught:

```c
    void jsw_bubblesort(int a[], int n)
    {
        int i;

        for (i = 0; i < n; i++)
        {
            int j;

            for (j = n - 1; j > 0; j--)
            {
                if (a[j] < a[j - 1])
                {
                    jsw_swap(&a[j], &a[j - 1]);
                }
            }
        }
    }
```

Well, you have to admit that it's simple. Of course, once you optimize
it a bit, that simplicity starts to go away. A flag for testing whether
a swap has been made is critical for bubble sort, because otherwise it
would make several unnecessary passes over the collection before
terminating. While strictly optional, a test for early completion is
considered a mandatory optimization to the bubble sort algorithm. No
further optimizations are necessary as they won't improve the
performance significantly, and simpler algorithms, such as insertion
sort, will almost always outperform it. Here's the “optimized” bubble
sort:

```c
    void jsw_bubblesort(int a[], int n)
    {
        int i;

        for (i = 0; i < n; i++)
        {
            int j, done = 1;

            for (j = n - 1; j > 0; j--)
            {
                if (a[j] < a[j - 1])
                {
                    jsw_swap(&a[j], &a[j - 1]);
                    done = 0;
                }
            }

            if (done)
            {
                break;
            }
        }
    }
```

Bubble sort is a quadratic algorithm, but it's frighteningly inefficient
even if you only compare it with other quadratic algorithms. Words can't
describe my loathing for this algorithm, and I've gotten into very
heated debates about whether it should ever be used or not, even as an
introductory algorithm. My recommendation: forget bubble sort exists.
You'll be better off for it. :-)

#### Quicksort

Bubble sort is unmercifully slow. It's the worst possible sorting
algorithm that you'll see in the real world, and even then only because
the author of the code probably doesn't know any better. Fortunately, we
can improve bubble sort in much the same way that we improved the
straight selection sort. But since we don't have a neat and tidy concept
to improve, along with a way to improve it (ie. the selection problem),
we need another way to improve the performance of exchanging.

The good news is that we can use recursion to split the array at a
median value at each step by moving the median value to its sorted
position, then recursively sort each subarray the same way in a very
good algorithm called quicksort. By dividing the problem at the median
each time, we can guarantee that this part of the algorithm is O(log N).
Now it's just a simple matter of figuring out a good way to sort the
subarrays that doesn't take longer than O(N) and we'll have an optimal
comparison sort.

Okay, maybe it's not that simple. :-) We have two immediate problems.
First we need a way to partition the array. Partitioning is forcing a
median value to its sorted position while making sure that all items
less than the median come before it and all items greater than or equal
to the median come after it. Then we need a way to find the median value
without taking too much time. To really find the median requires at
least an O(N) algorithm on top of the partitioning which will at best be
O(N). Now, O(N) + O(N) is still O(N), but the algorithm will run more
slowly than if we only had one O(N) part, and we want this to be as fast
as possible.

Rather than waste cycles trying to find the true median, we'll just
guess. Our first attempt at guessing is the simplest. We just take the
first item in the subarray and pretend that it's the median value. :-)
This could be a serious problem if the first item is the smallest item
in the array, but we'll look at that a bit later. One problem at a time.
Assuming a partition function, we can easily write a recursive function
to partition, then recursively do the same thing on each subarray as
long as the subarray has more than one item:

```c
    void jsw_quicksort_r(int a[], int first, int last)
    {
        if (first < last)
        {
            int pivot = jsw_partition(a, first, last);

            jsw_quicksort_r(a, first, pivot - 1);
            jsw_quicksort_r(a, pivot + 1, last);
        }
    }

    void jsw_quicksort(int a[], int n)
    {
        jsw_quicksort_r(a, 0, n - 1);
    }
```

Let's look at a sample execution to see how this function could possibly
work. It's not obvious at first unless you're familiar with recursion
and can see how partitioning forces an item to its sorted position
recursively results in a sorted file. In this example, we take the first
item in each subarray and use it as the median and ignore cases where
the subarray is too small to partition:

```
 |41|67|34|00|69|24|78|58|62|64|
 |41|34|00|24|58|62|64|69|67|78|
 |41|34|00|24|58|62|
 |41|34|00|24|58|
 |00|24|41|34|
 |00|24|34|41|58|62|64|
                      |69|67|78|
 |00|24|34|41|58|62|64|67|69|78|
```

The base case for recursion is when the indices **first** and **last**
cross. So we stop the recursion if we get to a subarray with only a
single item. **jsw\_quicksort** passes an inclusive array to
**jsw\_quicksort\_r**, which basically means that both **a\[first\]**
and **a\[last\]** are valid items. This is different from most of our
other algorithms (such as heap sort) where **last** is the first index
after the last element of the array. This is for the benefit of
**jsw\_partition**, which is more easily written if both **first** and
**last** refer to valid indices. The pivot variable refers to the index
of the median value so there's no need to include it in the recursive
calls as it's already sorted after partitioning. Here's the code that
performs the partition magic:

```c
    int jsw_partition(int a[], int first, int last)
    {
        int pivot = first;

        while (first < last)
        {
            if (a[first] <= a[last])
            {
                jsw_swap(&a[pivot++], &a[first]);
            }

            ++first;
        }

        jsw_swap(&a[pivot], &a[last]);

        return pivot;
    }
```

Partitioning an array is surprisingly difficult to get right. The
algorithm above is a relatively safe, simple, and efficient one that
gets the job done in O(N) time. The idea is to walk across the array,
swapping the pivot with items that are less than the pivot as we go, and
resetting the pivot to the next item when we swap. When we get to the
end of the array, the current pivot is the median value that's in its
sorted position. Note that the median isn't necessarily the pivot that
we chose at the beginning of the partition.

That's all there is to quicksort\! Once you have a partition algorithm
that works, writing the code to do a quicksort is trivial, and then you
have the fastest non-hybrid sorting algorithm at your disposal. But
we're not done yet because we can improve the algorithm. There are a few
problems that need to be fixed and optimizations that can make quicksort
even faster.

The first problem is the biggest. If the pivot value chosen in
**jsw\_partition** is always the smallest item in the array, such as
when the array is already sorted, no swaps will be made and the median
will be the first item in the array. This results in a vastly unbalanced
split where the left half has one item and the right half has the rest
of the array. This is a degenerate case where quicksort really isn't
that quick, with a quadratic time complexity.

There are two widely accepted ways to minimize the chances of hitting a
degenerate case. First, we can use a random number generator to pick
which value in the array to use as a pivot. Then we guarantee that
nothing untoward can happen by swapping that value with the first item
in the array because the partition algorithm assumes that pivot starts
at first. Otherwise we would end up walking off the end of the array,
and that's never a good thing. ;-) The changes are minimal:

```c
    int jsw_partition(int a[], int first, int last)
    {
        int pivot = first + rand() % (last - first + 1);

        jsw_swap(&a[pivot], &a[first]);
        pivot = first;

        while (first < last)
        {
            if (a[first] <= a[last])
            {
                jsw_swap(&a[pivot++], &a[first]);
            }

            ++first;
        }

        jsw_swap(&a[pivot], &a[last]);

        return pivot;
    }
```

Notice that the code to find a random index within the range is somewhat
complicated. We need to take into account that **first** is not always
zero, so we use **last - first + 1** to find the number of items in the
subarray (we add 1 because **last** is the last valid item, not one
beyond the last valid item), then use that as the modulus for rand
(ignoring the issues with rand). Finally, because the resulting number
is ranged from zero to the number of elements, we add **first** to fit
it within the range of \[**first**,**last**\] as a valid index.

The second, and preferred, option is to take the median of three items
picked at random from the array (though the first, last, and middle are
most common), and use that as the pivot. The process is the same, except
instead of picking a random index and swapping its item with
**a\[first\]**, we pick the median of **a\[first\]**, **a\[mid\]**, and
**a\[last\]**, then swap that median with **a\[first\]**:

```c
    int jsw_partition(int a[], int first, int last)
    {
        int pivot, mid = (first + last) / 2;

        /* Largest of two */
        pivot = a[first] > a[mid] ? first : mid;

        /* Smallest of remaining */
        if (a[pivot] > a[last])
        {
            pivot = last;
        }

        jsw_swap(&a[pivot], &a[first]);
        pivot = first;

        while (first < last)
        {
            if (a[first] <= a[last])
            {
                jsw_swap(&a[pivot++], &a[first]);
            }

            ++first;
        }

        jsw_swap(&a[pivot], &a[last]);

        return pivot;
    }
```

The next improvement is an optimization that doesn't touch
**jsw\_partition** (yay\!). Since it's really a waste to recurse on
small subarrays, we can set a threshold for when to stop calling
**jsw\_quicksort\_r** recursively. As it is, we're recursing until
**first** and **last** cross, which is equivalent to a threshold of 0.
But if we set a cutoff of, say, 15 or 20, we can mostly sort a large
array, then hand it off to insertion sort, which is much faster for
small or nearly sorted arrays. This suggests two possible variations.
First, we could call insertion sort inside **jsw\_quicksort\_r**, if we
get to the cutoff. Robert Sedgewick studied this thoroughly and
discovered that it's slightly more efficient to call insertion sort
after quicksort finishes, which brings us to this (where **THRESHOLD**
is a macro with whatever cutoff value you choose):

```c
    void jsw_quicksort_r(int a[], int first, int last)
    {
        if (last - first > THRESHOLD)
        {
            int pivot = jsw_partition(a, first, last);

            jsw_quicksort_r(a, first, pivot - 1);
            jsw_quicksort_r(a, pivot + 1, last);
        }
    }

    void jsw_quicksort(int a[], int n)
    {
        jsw_quicksort_r(a, 0, n - 1);
        jsw_insertion(a, n);
    }
```

The next optimization that we'll look at handles the problem of already
sorted subarrays. If the subarrays are already sorted or have a large
number of duplicates, quicksort will take its sweet time going through
them all. There's no point in using a sophisticated sort on an already
sorted subarray, and we can make quicksort consistently fast by testing
if the subarray is already sorted and doing nothing if it is. The change
is made to **jsw\_quicksort\_r**, and we can use the **is\_sorted**
function given earlier:

```c
    void jsw_quicksort_r(int a[], int first, int last)
    {
        if (last - first > THRESHOLD)
        {
            int pivot;

            if (is_sorted(a, first, last))
            {
                return;
            }

            pivot = jsw_partition(a, first, last);
            jsw_quicksort_r(a, first, pivot - 1);
            jsw_quicksort_r(a, pivot + 1, last);
        }
    }
```

You might be thinking that calling a linear function might have
performance issues, but in practice it actually makes quicksort faster.
But that's still not the best implementation\! It's fast, yes. It's
definitely a very good quicksort, and a good compiler will give you
efficient machine code for it. But a not so good compiler probably won't
take care of the tail recursion in **jsw\_quicksort\_r**. Removing
recursion is the last optimization for quicksort, and I'll leave it to
you because I'm evil. \>:)

As an aside, the worst case of quicksort is still O(N<sup>2</sup>) even
though we've shrunk the possibility of encountering it to virtually
nothing. However, by putting together several sorting algorithms into a
hybrid “super-algorithm”, we can have our cake and eat it too. Here are
the common players:

Heap sort: O(Nlog N) in the average and worst cases, but still slower
than quicksort due to more comparisons.

Quicksort: O(Nlog N) in the average case, but O(N<sup>2</sup>) in the
worst case. Fast as greased lightning if the worst case isn't
encountered though.

Insertion sort: O(N<sup>2</sup>) in the average case, but O(N) if the
array is already sorted or almost sorted.

Here's what we're going to do. We'll use quicksort until it's obvious
that we're nearing a degenerate case. We can test for that by figuring
out the depth of recursion that qualifies as O(log N). When we get to
that depth, we're starting to leave the boundaries of quicksort's
average case. Once we hit that cutoff, we then switch to heap sort
because it's guaranteed to be O(Nlog N). By doing this, we never leave a
complexity of O(Nlog N). We can also implement all of the optimizations
for quicksort such as the threshold and insertion sort at the end, as
well as already sorted subarrays. The resulting algorithm is called
introsort, short for introspective sort because it's smart enough to
change how it works when things start to get hairy:

```c
    void jsw_introsort_r(int a[], int first, int last, int depth)
    {
        if (last - first > THRESHOLD)
        {
            if (depth == 0)
            {
                jsw_heapsort(&a[first], last - first + 1);
            }
            else
            {
                int pivot;

                if (is_sorted(a, first, last))
                {
                    return;
                }

                pivot = jsw_partition(a, first, last);
                jsw_introsort_r(a, first, pivot - 1, depth - 1);
                jsw_introsort_r(a, pivot + 1, last, depth - 1);
            }
        }
    }

    void jsw_introsort(int a[], int n)
    {
        jsw_introsort_r(a, 0, n - 1, (int)(2 * log(n)));
        jsw_insertion(a, n);
    }
```

Heap sort and quicksort remain unchanged, as does insertion sort, but we
need to be careful to only call heapsort on the correct subarray, which
means passing an array slice. Introsort only puts them together into an
algorithm that guarantees good performance in all cases. But we can
improve even this beautiful algorithm by paring it down to trim the fat.
Since there's a logarithmic cutoff on the depth of the recursion, we can
remove the left side recursion in favor of a loop. This change helps to
negate any overhead that introsort might have so that it's that much
closer to a plain quicksort in the average case:

```c
    void jsw_introsort_r(int a[], int first, int last, int depth)
    {
        while (last - first > THRESHOLD)
        {
            if (depth == 0)
            {
                jsw_heapsort(&a[first], last - first + 1);
            }
            else
            {
                int pivot;

                if (is_sorted(a, first, last))
                {
                    return;
                }

                pivot = jsw_partition(a, first, last);
                jsw_introsort_r(a, pivot + 1, last, depth - 1);
                last = pivot - 1;
            }
        }
    }
```

Note that heap sort isn't a requirement. You can use any sorting
algorithm that guarantees at least O(Nlog N) performance, such as merge
sort or radix sort. However, heap sort has an added benefit that it
sorts in-place while merge sort and radix sort require extra memory to
temporarily store subarrays. The same goes for insertion sort. Any sort
that's most efficient with almost sorted arrays will work just as well.

#### Merge Sort

One of the first sorting methods suggested was by John Von Neumann. The
concept is simple: split an array into two parts, sort those two parts,
then merge them together. This is generally done recursively, where each
subarray is then separated into two smaller subarrays, those subarrays
are separated into four smaller subarrays, and so on until you have N
subarrays of 1 item. At that point, you merge all of the subarrays
together and the array is sorted. The basic code for this process is
similar to quicksort:

```c
    void jsw_mergesort_r(int a[], int first, int last)
    {
        if (first < last - 1)
        {
            int mid = (first + last) / 2;

            jsw_mergesort_r(a, first, mid);
            jsw_mergesort_r(a, mid, last);
            jsw_merge(a, first, mid, last);
        }
    }

    void jsw_mergesort(int a[], int n)
    {
        jsw_mergesort_r(a, 0, n);
    }
```

It seems as if merge sort is the inverse of quicksort. Where quicksort
first partitions and then recurses, merge sort recurses and then merges,
which is a reasonable observation. As with quicksort, if the merge
operation is assumed to work properly, the algorithm is easy to
visualize, even if it's difficult to describe. Also, while the recursive
scheme works for arrays in fast memory, merge sort's claim to fame is
that it can easily be modified to handle sequential data such as from a
stream or a generator. As such, merge sort is excellent for sorting
large linked lists, and it's often the algorithm of choice for external
sorting of files that are too large to fit in fast memory all at one
time.

It's a good idea, as with quicksort's partition, to choose the simplest
merge algorithm available. The process of merging is one of the
deceptively simple algorithms in computer science. Given two arrays,
compare each item with the other as they come and assign the smallest to
the final array. When one of the sequences is exhausted, copy the other
in bulk to the final array. Here is one algorithm, called straight
merging, that's easy to understand and works well in many cases:

```c
    void jsw_merge(int a[], int first, int mid, int last)
    {
        int i = first, j = mid, k = 0;
        int *save = malloc((last - first) * sizeof *save);

        while (i < mid && j < last)
        {
            if (a[i] <= a[j])
            {
                save[k++] = a[i++];
            }
            else
            {
                save[k++] = a[j++];
            }
        }

        while (i < mid)
        {
            save[k++] = a[i++];
        }

        while (j < last)
        {
            save[k++] = a[j++];
        }

        for (i = 0; i < (last - first); i++)
        {
            a[first + i] = save[i];
        }

        free(save);
    }
```

The merge simply works with two subarrays: a left subarray from first to
mid, and a right subarray from mid to last. As long as there are items
in both subarrays, we copy the smallest of them to the temporary buffer
and move only the index for the buffer and the subarray forward. After
one of the subarrays is exhausted, we copy the remaining subarray to the
buffer, then copy the buffer to the original array and terminate (but
not before freeing the memory we allocated). Here's an execution trace
of the entire sorting algorithm (leaving out the base cases because they
do nothing):

```
 |41|67|34|00|69|24|78|58|62|64|
 |41|67|34|00|69|
 |41|67|
       |34|00|69|
          |00|69|
       |00|34|69|
 |00|34|41|67|69|
                |24|78|58|62|64|
                |24|78|
                      |58|62|64|
                         |62|64|
                      |58|62|64|
                |24|58|62|64|78|
 |00|24|34|41|58|62|64|67|79|78|
```

It should be immediately obvious that for the merge algorithm to work,
extra space equal to the size of the array is required. If we didn't use
a temporary buffer, we might copy over the current item in one of the
subarrays and lose it forever. That's not conducive to a correct sorting
algorithm, so we live with the extra memory for a buffer.

The extra space requirement can be removed so that the merge runs
in-place, but the resulting algorithm is both complicated and slower, so
extra space is a necessary evil for an efficient merge sort using
arrays. This is why the merge sort is well suited to linked lists; an
item can simply be spliced in and out of a list representing the merged
halves without using any extra space, aside from a pointer or two.

The running time of a merge sort is largely insensitive to the order of
the items, so it has guaranteed performance properties. This is in
contrast to, for example, quicksort, where certain degenerate orders can
cause the running time of the algorithm to drop from O(Nlog N) to
O(N<sup>2</sup>). On the other hand, because guaranteed performance is a
double edged sword, a straight merge sort can't take advantage of any
inherent order already in the array to improve its running time.
Insertion sort is very fast, approaching and sometimes reaching O(N), if
the array is almost sorted, and other algorithms can take advantage of
the distribution of items to run very quickly.

Of course, like quicksort we can add a test to see if the subarray is
already sorted and avoid recursing on it. This counts as an
introspective algorithm that takes advantage of existing order in the
subarray. It also could be educational to try the threshold trick we got
from quicksort on merge sort, where we stop at a certain cutoff to make
the array almost sorted and finish the algorithm with an insertion sort.
Remember that tricks from one sorting algorithm can sometimes be reused
in other algorithms.

You might notice that while I said that merge sort is ideal for data
that can only be accessed sequentially, the merge sort algorithm above
actually relies heavily on random access for its speed and requires that
all of the items be present before the algorithm can be run so that it
can find the middle item. The same algorithm with a linked list would be
much slower because finding the middle of a list if you are only given
the head is somewhat expensive. The best solution is to walk two links
down the list, one moving twice as fast. When the faster link reaches
the end of the list, the slower link is at the middle. The code to split
a list in this way and then sort it by merging is relatively simple. The
split is easy to write, the merge is far easier to write and far
shorter, and the sort really doesn't change that much:

```c
    struct jsw_node *jsw_split(struct jsw_node *a)
    {
        struct jsw_node *b = a->next;

        while (b != NULL && b->next != NULL)
        {
            a = a->next;
            b = b->next->next;
        }

        b = a->next;
        a->next = NULL;

        return b;
    }

    struct jsw_node *jsw_merge(struct jsw_node *a, struct jsw_node *b)
    {
        struct jsw_node result, *it = &result;
        struct jsw_node **half;

        while (a != NULL && b != NULL)
        {
            half = (a->data < b->data) ? &a : &b;
            it->next = *half;
            it = *half;
            *half = (*half)->next;
        }

        it->next = (a == NULL) ? b : a;

        return result.next;
    }

    struct jsw_node *jsw_mergesort_r(struct jsw_node *a)
    {
        if (a != NULL && a->next != NULL)
        {
            struct jsw_node *b = jsw_split(a);

            a = jsw_mergesort_r(a);
            b = jsw_mergesort_r(b);

            return jsw_merge(a, b);
        }

        return a;
    }

    void jsw_mergesort(struct jsw_list *list)
    {
        list->head = jsw_mergesort_r(list->head);
    }
```

However, as I've already said several times, **jsw\_split** slows down
the algorithm by a substantial amount, so this algorithm isn't very
practical. A much better option is to take items sequentially from the
beginning, do some magic, and then merge the magical result into a
sorted array or list. This is much better suited to a sequential
delivery method such as a stream or generator, and it improves the
performance of the sort on linked lists considerably.

Because we are accepting items as they exist in the collection, we can
guarantee that the algorithm is stable. We can also take advantage of
the current order of the collection, unlike with straight merge sorting.
Such an algorithm is called a “natural” merge, because it uses the
natural order of the collection to improve its performance. The worst
case of a natural merge sort is O(Nlog N), but the best case is O(N).
This is an improvement over guaranteed O(Nlog N) performance in all
cases because if the array is already sorted or almost sorted, the
performance is as good as running insertion sort from the beginning.

The algorithms for a natural merge sort can vary wildly, but a simple
algorithm that performs well uses two queues to act as each half of the
merge. The first item goes into queue A, and for each new item, if the
item is smaller than the top of the current queue, the algorithm
switches queues. Because we switch queues only when the order of the
sequence changes, the number of items in each queue won't be balanced.
In fact, all of the items in the array could be in one queue, in which
case the array is completely sorted. Here's the code to do a natural
merge sort with arrays and queues:

```c
    void jsw_merge(int a[], int *q[2], int top[2])
    {
        int i[2] = { 0 }, k = 0;
        int dir = q[1][0] < q[0][0];

        a[k++] = q[dir][i[dir]++];

        while (i[dir] < top[dir])
        {
            dir = q[1][i[1]] < q[0][i[0]];

            /* Special case: Use larger item */
            if (q[dir][i[dir]] < a[k - 1] && a[k - 1] < q[!dir][i[!dir]])
            {
                dir = !dir;
            }

            a[k++] = q[dir][i[dir]++];
        }

        while (i[0] < top[0])
        {
            a[k++] = q[0][i[0]++];
        }

        while (i[1] < top[1])
        {
            a[k++] = q[1][i[1]++];
        }
    }

    void jsw_mergesort(int a[], int n)
    {
        int *q[2], top[2];
        int half, i;

        if (n < 2)
        {
            return;
        }

        q[0] = malloc(n * sizeof *q[0]);
        q[1] = malloc(n * sizeof *q[1]);

        for (;;)
        {
            top[0] = top[1] = 0;
            q[0][top[0]++] = a[0];
            half = 0;

            for (i = 1; i < n; i++)
            {
                if (a[i] < q[half][top[half] - 1])
                {
                    half = !half;
                }

                q[half][top[half]++] = a[i];
            }

            if (top[0] == 0 || top[1] == 0)
            {
                break;
            }

            jsw_merge(a, q, top);
        }

        half = top[0] == 0;

        for (i = 0; i < n; i++)
        {
            a[i] = q[half][i];
        }

        free(q[0]);
        free(q[1]);
    }
```

Notice that **jsw\_merge** no longer needs to allocate memory because
that memory is allocated in **jsw\_mergesort** to the queues, and then
the queues are passed to **jsw\_merge**. This makes the merge simpler
and faster because we're not constantly calling an expensive memory
manager. A simple boolean flag is used to switch between queues. This
works because we only have two queues, so the only valid indices are 0
and 1. That coincides perfectly with the result of a boolean test.
However, we need to be careful to reset the queues on each iteration of
the outer loop (by setting the tops to 0 and giving the first queue the
first item in the array).

Let's look at an example, because this algorithm could easily be
confusing. :-) The following execution depicts the worst case for a
natural merge sort (where the algorithm has O(Nlog N) performance). The
worst case is an already sorted array in descending order, which is
basically the opposite of what we want (in this tutorial we're sorting
in ascending order). Pay careful attention to how the two queues work
together to sort the array:

```
          Result              Queue A             Queue B
 1)  |8|7|6|5|4|3|2|1|   | | | | | | | | |   | | | | | | | | |
 2)  |7|5|3|1|8|6|4|2|   |8|6|4|2| | | | |   |7|5|3|1| | | | |
 3)  |5|1|7|3|6|2|8|4|   |7|3|6|2| | | | |   |5|1|8|4| | | | |
 4)  |1|5|3|6|4|7|2|8|   |5|3|6|4| | | | |   |1|7|2|8| | | | |
 5)  |1|3|5|4|6|2|7|8|   |1|5|4|7| | | | |   |3|6|2|8| | | | |
 6)  |1|3|4|5|2|6|7|8|   |1|3|5|2|7|8| | |   |4|6| | | | | | |
 7)  |1|2|3|4|5|6|7|8|   |1|3|4|5| | | | |   |2|6|7|8| | | | |
 8)  |1|2|3|4|5|6|7|8|   |1|2|3|4|5|6|7|8|   | | | | | | | | |
```

It's possible to modify the algorithm to eliminate the need for two
queues, but the result is dreadfully complicated and only serves to
confuse most of the people that look at it. :-) The only problem with
using two queues implemented as arrays is that both have to be at least
as big as the array itself, so you are looking at an extra space cost of
2N instead of N. However, using a dynamic queue can decrease that cost
while causing the algorithm to potentially run slower.

The extra extra space for a natural merge sort on arrays isn't such a
bad thing because you shouldn't use a natural merge sort on arrays in
the first place. Even though a natural merge sort on an array is
theoretically better than a straight merge sort, in practice it's much
slower due to excessive copying of data in and out of the queues.
However, the natural merge sort is ideal for linked lists because by
splicing nodes, you avoid the problem of copying data all over the
place. Here's a tuned natural merge sort for linked lists:

```c
    /* Remove a node from a list and push it onto a tail pointer */
    #define split_push(head,tail,item) do { \
      struct jsw_node *save = item->next; \
      item->next = NULL; \
      if (tail == NULL) \
        head = tail = item; \
        else { \
        tail->next = item; \
        tail = tail->next; \
        } \
      item = save; \
    }while (0)

    struct jsw_node *jsw_merge(struct jsw_node *a, struct jsw_node *q[2])
    {
        int dir = q[0]->data > q[1]->data;
        struct jsw_node *save = a = q[dir];

        /* Intended assignment */
        while (q[dir] = q[dir]->next)
        {
            dir = q[0]->data > q[1]->data;

            if (q[dir]->data < a->data  && a->data < q[!dir]->data)
            {
                dir = !dir;
            }

            a->next = q[dir];
            a = a->next;
        }

        dir = q[0] == NULL;

        while (q[dir] != NULL)
        {
            a->next = q[dir];
            q[dir] = q[dir]->next;
            a = a->next;
        }

        return save;
    }

    void jsw_mergesort(struct jsw_list *list)
    {
        struct jsw_node *q[2] = { 0 };
        struct jsw_node *a = list->head;

        for (;;)
        {
            struct jsw_node *end[2] = { 0 };
            int half = 0;

            split_push(q[0], end[0], a);

            while (a != NULL)
            {
                if (a->data < end[half]->data)
                {
                    half = !half;
                }

                split_push(q[half], end[half], a);
            }

            if (q[0] == NULL || q[1] == NULL)
            {
                break;
            }

            a = jsw_merge(a, q);
        }

        list->head = q[q[0] == NULL];
    }
```

Not only is the code simpler (assuming you're familiar with linked
lists), it's also much faster than the array version. The extra space
cost has virtually been eliminated because the queues are nothing more
than pointers to nodes that already exist, so no extra memory needs to
be allocated. **jsw\_merge** is just a translation of the natural merge
algorithm used for arrays to linked lists, and **split\_push** is
nothing more than a specialized way to extract a node from the list and
push it onto a queue.

The performance properties of merge sort are very good. It has
guaranteed O(Nlog N) performance, like heap sort, and when a natural
merge sort is used it can reach O(N) performance, but the performance of
merge sort comes at the price of extra space equivalent to the number of
items being sorted. This cost is eliminated when sorting linked lists,
which is why merge sort is a common list sorting algorithm. When the
merge algorithm is chosen carefully, mergesort is one of the few stable
sorting algorithms. Most other high speed sorting algorithms that can
handle large arrays or lists require extra work and extra space to make
them stable, so in light of the stability question, mergesort fares very
well. Another huge benefit is that when written carefully, merge sort
does not require that all items be present. It can sort an unknown
number of items coming in from a stream or generator, which is a very
useful property.

#### Radix Sort

Up to this point, we've looked mostly at algorithms that sort by
comparison. That is, we figure out the relative order of two items by
testing which is less than the other and place them accordingly. This
section will cover a different method, where we use the distribution of
items and place them in buckets, then put the buckets together to form a
final sorted array. The first algorithm that uses distribution is called
counting sort. The concept is to map each item to an array, where the
item's value is the array index. Then you count how many items are
smaller than that item, each time incrementing the index. Then you use
the count as a location index to move the item to its final location.

Here's the process on an array of 10 items ranged from \[0,10) using a
1-based index algorithm described in Donald Knuth's “The Art of Computer
Programming”, volume 3:

```
 |1|7|4|0|9|4|8|8|2|4|

 Get counts:

 |0|1|1|1|0|3|0|0|1|2|1|

 Count smaller items:

 |0|1|2|3|3|6|6|6|7|9|1|

 Use counts as indices:

 |0|1|2|4|4|4|7|8|8|9|
```

It's a nifty algorithm, but when I first read about it, I wasn't able to
completely understand what was going on until I walked through it on
paper, so I won't try to explain in detail. I'll leave you with the
basic concept, the code, and let you do the same thing (probably because
I'm evil). There's a lot to be said for working out algorithms on paper
though. :-) Here's the code that does a counting sort. Be sure to figure
it out before moving on, because this will be a fundamental part of the
coming radix sorts. Be happy that it's in C, I had to translate it from
MIX...

```c
    void jsw_countsort(int a[], int m, int n)
    {
        int *aux = malloc(n * sizeof *aux);
        int *count = calloc(m + 1, sizeof *count);
        int i;

        for (i = 0; i < n; i++)
        {
            ++count[a[i] + 1];
        }

        for (i = 1; i < m; i++)
        {
            count[i] += count[i - 1];
        }

        for (i = 0; i < n; i++)
        {
            aux[count[a[i]]++] = a[i];
        }

        for (i = 0; i < n; i++)
        {
            a[i] = aux[i];
        }

        free(count);
        free(aux);
    }
```

As we discovered previously in the tutorial, the lower bound for sorting
algorithms that use comparisons to find the relative order of items is
O(Nlog N). However, it's possible to improve that to O(N) by eschewing
comparisons and using distribution as the counting sort did. Counting
sort is a linear algorithm because it makes a set number of linear
passes through the array, regardless of pre-existing order.

However, the biggest problem with counting sort is that it requires
extra memory equivalent to the range of items being sorted. So if you
have 10 items in the range of \[0,1,000,000), you need to allocate an
extra array to hold the counts for one million items. That's a
ridiculous requirement, and it makes counting sort far less practical
than other sorting algorithms. If only we could figure out a way to use
counting sort but minimize the extra space we need to allocate for the
counts.

This idea is exactly what a class of algorithms called radix sort do.
Instead of using the full range of each item, they use the range of a
single digit, for some definition of “digit”, and then use a
distribution sort on each digit either from the most significant digit
to the least significant digit or vice versa. The two algorithms called
radix sort are LSD (for “least significant digit”, you druggies\!), and
MSD (for most significant digit). The LSD algorithm is what was used to
sort punch cards back in the early days of computing.

Before we get into the code, let's look at an example execution where we
start at the least significant digit of several 3 digit numbers and use
counting sort until we get to the most significant digit:

```
   1     2     3     4
 -----------------------
 |341| |100| |100| |100|
 |267| |341| |224| |178|
 |534| |762| |534| |224|
 |100| |534| |341| |267|
 |369| |224| |958| |341|
 |224| |564| |762| |369|
 |178| |267| |564| |534|
 |958| |178| |267| |567|
 |762| |958| |369| |762|
 |564| |369| |178| |958|
```

There are several things to remember about LSD radix sort. Each digit
needs to be sorted with a stable algorithm. The most common choice is a
counting sort because counting sort is an algorithm that can reach O(N)
performance. If you have short keys then LSD radix sort can also be
O(N), whereas if we used another stable sort like a straight shift
select, each digit would be sorted with an O(N<sup>2</sup>) sort, and
the entire algorithm would become quadratic. Next, each key has to be
the same length. That's why most radix sorts will use the number of
bytes as the radix, so 4 byte integers would be sorted from the least
significant byte to the most significant byte, where each byte is a
digit. The performance of any radix sort is dependent on the length of
the items.

With this in mind, LSD radix sort is relatively easy to implement. You
simply modify a counting sort to use digits, then call the counting sort
on each digit of the array. The auxiliary array that counting sort uses
can be allocated in the function that calls the counting sort, so we
avoid multiple calls to the memory manager. The nice thing about using
counting sort for a radix sort is that the count array can be small. In
our case, we only need to count all of the possible bytes, so the count
array can be as small as **( 1U \<\< CHAR\_BIT ) + 1**, which is exactly
what we do. :-)

```c
    #define RANGE ((1U << CHAR_BIT) + 1)
    #define digit(x,i) ((x) >> ((i) * CHAR_BIT) & UCHAR_MAX)

    void jsw_radix_pass(int a[], int aux[], int n, int radix)
    {
        int i;
        int count[RANGE] = { 0 };

        for (i = 0; i < n; i++)
        {
            ++count[digit(a[i], radix) + 1];
        }

        for (i = 1; i < RANGE; i++)
        {
            count[i] += count[i - 1];
        }

        for (i = 0; i < n; i++)
        {
            aux[count[digit(a[i], radix)]++] = a[i];
        }

        for (i = 0; i < n; i++)
        {
            a[i] = aux[i];
        }
    }

    void jsw_radixsort(int a[], int n)
    {
        int i;
        int *aux = malloc(n * sizeof *aux);

        for (i = 0; i < sizeof *a; i++)
        {
            jsw_radix_pass(a, aux, n, i);
        }

        free(aux);
    }
```

The biggest problem with LSD radix sort is that it starts at the digits
that make the least difference. If we could start with the most
significant digits, the first pass would go a long way toward sorting
the entire range, and each pass after that would simply handle the
details. The idea of MSD radix sorting is to divide all of the digits
with an equal value into their own bucket, then do the same thing with
all of the buckets until the array is sorted. Naturally, this suggests a
recursive algorithm, but it also means that we can now sort variable
length items and we don't have to touch all of the digits to get a
sorted array. This makes MSD radix sort considerably faster and more
useful.

```
 Original:

 |369|267|564|100|341|224|178|958|762|334|

 First Pass:

   1     2     3     4     5     6
 -----------------------------------
 |100| |267| |369| |564| |762| |958|
 |178| |224| |341|
             |334|

 Second Pass:

   1     2     3     4     5     6     7     8     9    10
 -----------------------------------------------------------
 |100| |178| |224| |267| |334| |341| |369| |564| |762| |958|

 Sorted:

 |100|178|224|267|334|341|369|564|762|958|
```

Notice that because all of the second digits are unique, no further
sorting is needed beyond the second pass. Each of the buckets are
separated into their own buckets containing items that have duplicate
digits. So in the first pass, every item that started with 3 was put
into a bucket, every item that started with 2 was put in a bucket, and
so on. This is a good example of MSD radix sort being more efficient
than LSD radix sort, which would have to sort all digits before the
array would be sorted.

The bad news is that MSD radix sort is going to be recursive in this
tutorial. It looks surprisingly similar to quicksort though, which is a
good thing because we can use the threshold trick to stop before
spending too much time on small buckets and finish the almost sorted
array with insertion sort. **jsw\_radix\_pass** is pretty much the same
thing, but for variety and to show the variable length item strength of
MSD radix sort, we'll implement this one to sort strings instead of
integers:

```c
    /* Extract ith MSD byte */
    #define digit(x,i) ((x)[i])
    #define bin(x) (first + count[x])

    void jsw_radix_pass(char *a[], char *aux[], int count[], int first, int last, int radix)
    {
        int i;

        for (i = first; i < last; i++)
        {
            ++count[digit(a[i], radix) + 1];
        }

        for (i = 1; i < UCHAR_MAX; i++)
        {
            count[i] += count[i - 1];
        }

        for (i = first; i < last; i++)
        {
            aux[count[digit(a[i], radix)]++] = a[i];
        }

        for (i = first; i < last; i++)
        {
            a[i] = aux[i - first];
        }
    }

    void jsw_radixsort_r(char *a[], char *aux[], int first, int last, int radix)
    {
        if (radix <= UCHAR_MAX && last - first > THRESHOLD)
        {
            int count[UCHAR_MAX + 1] = { 0 };
            int i;

            jsw_radix_pass(a, aux, count, first, last, radix);

            for (i = 0; i < UCHAR_MAX - 1; i++)
            {
                int left = bin(i);
                int right = bin(i + 1) - 1;

                jsw_radixsort_r(a, aux, left, right, radix + 1);
            }
        }
    }

    void jsw_radixsort(char *a[], int n)
    {
        char **aux = malloc((n + 1) * sizeof *aux);

        jsw_radixsort_r(a, aux, 0, n, 0);
        jsw_insertion(a, n);

        free(aux);
    }
```

MSD radix sort doesn't require a stable sort as the workhorse algorithm,
even though we still used counting sort because it's fast. So MSD radix
sort is considerably more flexible than LSD radix sort. One thing to be
careful of is that in both the LSD and MSD algorithms, I've implemented
0 as “start here”, so LSD sees digits in 5,4,3,2,1,0 order while MSD
sees digits in 0,1,2,3,4,5 order. In other words, be careful when mixing
and matching because these algorithms can be very fragile.

#### Conclusion

The art of sorting is unbelievably diverse, and one single source can't
possibly teach you everything there is to know about it. I regret that
this tutorial can't be more comprehensive, but it was already too long
when I felt that I included the bare minimum of information. I hope that
this article was helpful, and I encourage you to read Donald Knuth's
“The Art of Computer Programming” volume 3, which is considered the
definitive text on sorting. However, keep in mind that it is somewhat
out of date and that more recent discoveries on sorting will require
more intensive searching to uncover...ironically enough. :-)

-----

© 2018 - Eternally Confuzzled
