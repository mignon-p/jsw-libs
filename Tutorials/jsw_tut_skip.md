Skip Lists
==========

A binary search tree is a data structure designed for efficient search, insertion, and deletion in the presence of a large number of items. While these operations are easy to implement for a basic binary search tree, less trivial operations such as flexible non-recursive traversal are not as simple to create. Also, basic binary search trees have uncomfortably common worst case behavior that makes them impractical for many applications.

The initial solution to the problem of degenerate trees is to maintain a balancing invariant, whereby the tree is restructured to enforce a certain measure of balance when a new insertion or deletion causes the invariant to be broken. Unfortunately, the one benefit of the basic binary search tree, simplicity, is destroyed when a balanced tree is used. The algorithms for balanced binary search trees are very complicated, very fragile, and few programmers are willing to attempt an implementation.

Worse yet, balanced trees are very inflexible when it comes to optimization. The structure is well defined, efficient algorithms are known, and changes to those algorithms will typically result in subtle bugs. The basic rule of thumb with balanced trees is to use a known recursive approach if you do not need blazing speed, and a known non-recursive approach if you do. As such, balanced trees do not encourage experimentation.

An alternative to balanced trees is to use randomization to guarantee, with high probability, that a tree will not be so unbalanced as to lose its O(log N) performance properties. These randomized trees are much simpler that their balanced siblings and far safer than basic binary search trees, but non-recursive implementations are often not immediately obvious. If an absolute guarantee of good performance is not necessary, a recursive implementation and a slim chance of poor performance is acceptable, randomized trees are a good solution.

However, this still does not solve the problem of non-trivial operations. For example, an iterator based tree traversal is neither simple in concept, nor efficient in practice. One tries to implement a tree traversal such that clients can write code like this with a guarantee that each item is only visited once, thus ensuring an efficient traversal:

    reset(tree)

    while (item := next(tree)) != END do
        # Do something with item
    loop

Unfortunately, such a guarantee is not possible without caching items in a linear data structure or saving extranenous traversal information in the tree. The former is expensive when it comes to space, and the latter is complicated to implement and in the end, has little real use. The reset operation would be forced to start a search at the smallest item, thus incurring a logarithmic cost to find the smallest item:

    function reset: tree
    begin
        tree.walk := tree.root

        if tree.walk = null then
            return
        endif

        while tree.walk.left <> null do
            tree.walk := tree.walk.left
        loop
    end

This alone would not be much of a concern, but for each call to next, a stack must be maintained to keep track of the search path, basically turning an iterative inorder traversal into a step-by-step iterative inorder traversal, where the state is saved after each step so that it can be continued with the next call to the function:

    function next: tree
    begin
        if walk = null then
            return END
        endif

        if walk.right <> null then
            stack.push(walk)
            walk := walk.right

            while walk.left <> null do
                stack.push(walk)
                walk := walk.left
            loop
        else
            while true do
                last := walk
                walk := stack.pop()

                if last <> walk.right then
                    break
                endif
            loop
        endif

        return walk.item
    end

One might argue that this is an elegant and efficient algorithm, but it is not as efficient as we would like, and without extra help, this is the best that binary trees can do.

Fortunately, there is a data structure that is often simpler both in concept and in implementation. A skip list is a data structure that conceptually uses parallel sorted linked lists. For example, consider a basic sorted linked list:

    1---2---3---4---5---6---7---8---*

This is clearly a linear structure. Searching, insertion, and deletion will all take O(N) time. However, if we add a second list that skips over every other node, we can make searches faster by searching the second list and then moving down into the first list when a node that is larger than the search item is found:

    1-------3-------5-------7-------*
    1---2---3---4---5---6---7---8---*

A search for **8** would result in four nodes being traversed rather than seven. This is the basic idea behind a skip list, but a skip list goes even further and uses M sorted linked lists to achieve O(log N) performance. For example, by adding a third list that skips over every other node in the second list, we achieve a structure that looks strikingly like a balanced binary search tree:

    1---------------5---------------*
    1-------3-------5-------7-------*
    1---2---3---4---5---6---7---8---*

Indeed, a skip list is really just a simulation of a binary search tree using multiple linked lists running in parallel. Of course, trying to maintain a perfectly balanced binary search tree using this simulation is still expensive, and it is still complicated. However, by viewing a skip list as a single sorted linked list, where each node has a column of links to other nodes in the list, it is easy to see how randomly choosing a height for each new column could potentially produce the same good behavior as a balanced skip list.

The original design of the skip list was as a randomized data structure, much like randomized binary search trees. By eschewing a logarithmic bound guarantee in favor of a logarithmic bound with high probability, a skip list can enjoy the performance properties of a well balanced binary search tree (on average) while avoiding many disadvantages of tree structures.

For example, while an iterator based traversal in a binary search tree is a non-trivial problem, the same problem becomes a couple of one-liners when using a skip list, and the best case of traversal performance is achieved (each unique node is visited only once):

    function reset: skip
    begin
        walk := skip[0]
    end

    function next: skip
    begin
        return (walk := walk.next) <> null
    end

Notice that the above functions assume that a skip list is implemented as an array of linked lists, with index 0 being the lowest level. This is by no means the only way to implement skip lists, as we shall see shortly, but it is by far the most common.

#### Random height

Selecting a random height for each node is not as simple as just picking any old random number. An important aspect of a skip list is the maximum height, because if more time is spent moving down to the next level from a ridiculously high column compared to the rest, any gain from the skip list could be lost. So an upper limit to the column height is necessary. Fortunately, the height can be relatively small and still support a large list. A skip list with a maximum height of 16, for example, could hold approximately 2<sup>16</sup> nodes. Even a skip list of moderate height, when full, can quickly take up most of the fast memory of modern systems (at the time of writing).

So it is reasonable to say that the maximum height for a skip list will be somewhere between 16 and 24, for most real world applications. If there are a small number of items then a less sophisticated structure is more likely to be faster simply because it does less work for each operation. Anyway, with the maximum height in hand, the problem is still not as simple as returning a random number between 0 and MAX.

First, a random height generator should not return 0, because every valid node in a skip list must contain at least one link to the next node on the lowest level. A height of 0 is illogical. Second, most random number generators return a random number with uniform distribution, which means that every height would be equally probable, and this does not encourage the efficient data structure that we want.

What we need is a function that returns a weighted random number based on a probability equation. To get the binary tree-like structure in a skip list, each height must be about half as probable as the height below it, or in more precise terms, the random height should be chosen with a probability of 1/2. To find a number like this, one only has to perform a series of coin flips until the maximum height is reached, or a probability test fails:

    int rheight(int max)
    {
        int h = 1; /* Never return 0 */

        while (h < max && rand() < RAND_MAX / 2)
        {
            ++h;
        }

        return h;
    }

This is a simple solution, but it is not practical for the real world because it is too inefficient. Notice how **rand()** is called for every iteration of the loop. Because max is expected to be relatively small, we can expect that at most, max calls to **rand()** will be made. However, this can prove to be a significant bottleneck in the skip list algorithm if care is not taken.

Notice how we are always testing for “yes” or “no”, up to a count of max. So rather than continually call **rand()** and test whether it is bigger than half of **RAND\_MAX** or not, why not simply call **rand()** once and then use the bits of that random number to determine the height? This bit stream approach is more efficient because it only calls the expensive random number generator once. The solution is simple. Just create a random number, then walk through the bits of the number testing for a 0 or a 1. An implementation is longer, but straightforward:

    int rheight(int max)
    {
        static int bits = 0;
        int h, found = 0;

        for (h = 0; !found; h++)
        {
            if (bits == 0)
            {
                bits = rand();
            }

            found = bits % 2;
            bits = bits / 2;
        }

        if (h >= max)
        {
            h = max - 1;
        }

        return h;
    }

This algorithm is tuned for speed without sacrificing clarity. Notice how no test for h being greater than or equal to max is made until outside of the loop. This is because if the random number has sufficiently random bits, the loop will be short, and our probability requirements will be met. However, even this can be improved by forcing all of the bits in the number to be used, thus reducing the number of calls to the random number generator, and also by replacing the divisions with faster bitwise operations:

    int rheight(int max)
    {
        static int bits = 0;
        static int reset = 0;
        int h, found = 0;

        for (h = 0; !found; h++)
        {
            if (reset == 0)
            {
                bits = rand();
                reset = sizeof(int) * CHAR_BIT;
            }

            found = bits & 1;
            bits = bits >> 1;
            --reset;
        }

        if (h >= max)
        {
            h = max - 1;
        }

        return h;
    }

One important point to keep in mind with the bit stream solution is that it relies heavily on the bit length of the random number generator. On many systems, **rand()** returns a 16-bit random number, so the largest height available on such a system using **rand()** and the bit stream solution is 15. Therefore, it makes sense to use a random number generator that is known to have random bits and return at least a 32-bit value.

Also note that a probability of 1/2 is not the best, theoretically. A probability of 1/3 has seen better performance in real applications, and 1/e is the theoretical optimum. However, in my experience, there has been little difference between the three, and 1/2 is easier to optimize for the bit stream approach. Performance-wise, a skip list using 1/2 will have approximately half the links on each level that are on the level below it, and on average, only two nodes per level are visited during a search, so regardless of whether you choose 1/2, 1/3, 1/e, or something else near that range, a skip list will be O(log N) with high probability.

#### Searching

Searching in a skip list is only marginally more difficult than searching in a regular sorted linked list. However, because a skip list is a two dimensional data structure, it must be noted that there are several ways to implement one. The two most common implementations are the classic skip list and the linked skip list, which use a linked list of arrays of links, and a two dimensional network of nodes, respectively.

The classic skip list node and list structures will look something like this in a realistic implementation:

    struct jsw_node
    {
        int data;
        int height;
        struct jsw_node **next;
    };

    struct jsw_skip
    {
        struct jsw_node *head;
        int height;
    };

Notice how next is a pointer to a pointer so that the size of a node is relatively small while still allowing memory to be allocated to the column so that it can be used as an array of pointers to jsw\_node. Also notice that this, and most implementations, uses a header node that does not contain an item. This simplifies the algorithms for insertion and deletion. The header node should have the maximum height possible for the list, to avoid reallocation of memory.

However, for the purposes of this tutorial, a much simpler alternative will be used so as to avoid memory allocations and better illustrate the workings of the data structure:

    struct jsw_node
    {
        int data;
        int height;
        struct jsw_node *next[MAX];
    };

    struct jsw_skip
    {
        struct jsw_node *head;
        int height;
    };

The **height** field in **jsw\_skip** is for the current height of the list, which will be less than an upper limit, in this case defined as **MAX** with an unspecified value, which is used as the parameter for the **rheight** function. The **height** field changes if the height of a new node exceeds the current height of the list, which starts at 0 for an empty list and never exceeds **MAX - 1**.

A linked skip list node and list structure typically look like this, though the linked implementation is rare, and I have yet to see one in practice, we will cover the algorithms in detail for completeness:

    struct jsw_node
    {
        int data;
        struct jsw_node *next;
        struct jsw_node *down;
    };

    struct jsw_skip
    {
        struct jsw_node *head;
        struct jsw_node *bottom;
        int height;
    };

The **down** and **next** links do exactly what you would expect. The **next** link moves to the next node on the current level and the **down** link moves to the level below the node. What is not immediately obvious is the need for both a **head** and a **bottom** node. Because the linked implementation no longer uses an array of headers (though it could if you choose to do so), it makes sense to save the beginning of the lowest level for traversals rather than start at the top and move down to the bottom each time you want to traverse the lowest level.

Now, you might think that **MAX** and **height** are no longer needed, and while it is possible to implement a linked skip list without them, it is much easier with them. **MAX** can be variable in this implementation if you so choose, but for the purposes of this tutorial, it will not change, just like in the classic implementation. Modification of the algorithms to allow a dynamically sized maximum height is left as an exercise, though keep in mind that such a change is probably not ever needed if **MAX** is chosen carefully.

#### Classic search

The algorithm is simple. Start at the highest level, follow the next link until an item that is larger than the key is found, then move down to the next level and repeat until either the item is found, or until the spot where the item would be if it were in the list is found. That is the basis of all searching algorithms in a skip list, and searching is the basis of almost all other operations on a skip list. The implementation for search in a classic skip list is short and sweet:

    int jsw_find(struct jsw_skip *skip, int key)
    {
        struct jsw_node *it = skip->head;
        int i;

        for (i = skip->height - 1; i >= 0; i--)
        {
            while (it->next[i] != NULL && key > it->next[i]->data)
            {
                it = it->next[i];
            }
        }

        return it->next[0] ? it->next[0]->data : -1;
    }

Notice how the search finishes on the node before either the found item, or the place where the key would be. This can be useful for modularizing a skip list so that only one search algorithm is implemented as a function and all other algorithms that require a search can use the function. However, also note that there is potential for the next link to be a null pointer, which requires a bit of extra work to ensure that the correct type is returned and that a null pointer is never dereferenced. In this example I make the assumption that **-1** will never be a valid item in the skip list. However, a more realistic approach is to use a pointer to void for the item and simply return the next link in the hope that the calling function will take care to test for a null pointer.

#### Linked search

Searching in a linked implementation is arguably even simpler in concept than in a classic implementation because it involves straight pointer chasing rather than a combination of pointer chasing and array indexing:

    int jsw_find(struct jsw_skip *skip, int key)
    {
        struct jsw_node *it = skip->head;

        while (it->down != NULL)
        {
            while (it->next != NULL && key > it->next->data)
            {
                it = it->next;
            }

            it = it->down;
        }

        return it->next ? it->next->data : -1;
    }

#### Finger search

Just like in Binary search trees, there is a locality of reference issue where if certain groups of items are searched for regularly, the skip list search still starts at the top and performs a complete search. This is wasteful, and can easily be fixed using search fingers. A search finger is simply the search path of the last successful or unsuccessful search. Future searches begin by following the finger, rather than the full search path, in the hopes that there will be locality of reference and the new item to be searched for is close to the last search path. The simplest finger search by far is a dual search starting with the finger:

    int jsw_find(struct jsw_skip *skip, int key)
    {
        struct jsw_node *it = fix[skip->height - 1];
        int i = skip->height - 1;

        for (i = skip->height - 1; i >= 0; i--)
        {
            while (it->next[i] != NULL && key > it->next[i]->data)
            {
                it = it->next[i];
            }

            fix[i] = it;
        }

        if (key != it->next[0]->item)
        {
            it = skip->head;

            for (i = skip->height - 1; i >= 0; i--)
            {
                while (it->next[i] != NULL && key > it->next[i]->data)
                {
                    it = it->next[i];
                }

                fix[i] = it;
            }
        }

        return it->next[0] ? it->next[0]->data : -1;
    }

First the finger path is searched, and the finger array is updated as the search progresses. If the finger search succeeds then the algorithm returns successfully and the finger array reflects the most recent search. If the finger search fails, a complete search is performed, taking care to also update the finger array.

A fortunate side effect of traditional insertion and deletion is the need for an auxiliary array of size **MAX** for maintaining the search path. This is done so that the list can be correctly updated. The update array can also be used as a finger if it is global or a member of the skip list structure, because the last search will be saved in it unless explicitly cleared. A finger search can be either significantly faster, or significantly slower, depending on the locality of reference for a search, so if you use it, do so carefully.

#### Classic insertion

Insertion into a skip list is a simple extension of the search algorithm. The only difference is that during the search, the path is saved so that each level of the new column can be properly linked into the list. Aside from that, insertion is just a repetitive insertion into a sorted singly linked list. Of course, if the height of the new column is greater than the current height of the list, the height of the list must grow to accomodate it. Consider the insertion of 6 with height 4 into the following skip list:

    *-----------3-----------7-----------*
    *---1-------3---4-------7-------9---*
    *---1---2---3---4---5---7---8---9---*

First, a search is performed, where the node before where 6 would be is saved. Because the new node is taller than the list, the header is used as the previous node for each level higher than the current list level. Then the actual search begins and 3 is placed in the update array because 7 is greater than 6, so 6 would be the next node:

    fix[3] = head
    fix[2] = 3
    fix[1]
    fix[0]

    *===========3-----------7-----------*
    *---1-------3---4-------7-------9---*
    *---1---2---3---4---5---7---8---9---*

The search continues down at 3, then right to 4, and 4 is saved in the update array once again because 6 would go before 7 and 7 is directly after 4:

    fix[3] = head
    fix[2] = 3
    fix[1] = 4
    fix[0]

    *===========3-----------7-----------*
    *---1-------3===4-------7-------9---*
    *---1---2---3---4---5---7---8---9---*

The search moves down again because we are not at the bottom level, and right to 5. The search stops at 5 because 7 is after 5, 6 would fit in that gap, and the bottom of the list has been reached. At this point the update array is complete and the new node can be spliced into the list:

    fix[3] = head
    fix[2] = 3
    fix[1] = 4
    fix[0] = 5

    *===========3-----------7-----------*
    *---1-------3===<4-------7-------9---*
    *---1---2---3---4===5---7---8---9---*

Splicing the new node into the list is a simple matter of afixing 6's 0th next pointer to **fix[0]**'s next pointer, then **fix[0]**'s next pointer to 6. Repeat the process for all of 6's height, and the splice is complete:

    fix[3] = head
    fix[2] = 3
    fix[1] = 4
    fix[0] = 5

    *=======================6---------------*
    *-----------3===========6---7-----------*
    *---1-------3---4=======6---7-------9---*
    *---1---2---3---4---5===6---7---8---9---*

The code to perform this magic is short, simple, and uses the same search algorithm except with a minor modification for adding to the update array:

    int jsw_insert(struct jsw_skip *skip, int key)
    {
        struct jsw_node *it = skip->head;
        struct jsw_node *item = new_node(key);
        int i;

        for (i = skip->height - 1; i >= 0; i--)
        {
            while (it->next[i] != NULL && key > it->next[i]->data)
            {
                it = it->next[i];
            }

            fix[i] = it;
        }

        if (it->next[0] != NULL && it->next[0]->data == key)
        {
            return 0;
        }
        else
        {
            int h;

            if (item->height > skip->height)
            {
                item->height = ++skip->height;
                fix[item->height - 1] = skip->head;
            }

            h = item->height;

            while (--h >= 0)
            {
                item->next[h] = fix[h]->next[h];
                fix[h]->next[h] = item;
            }
        }

        return 1;
    }

Alternatively, you can avoid the auxiliary array by splicing the links as you go down the search path, but this approach assumes that the item is not already present in the list, or that duplicate keys are allowed. Duplicate items may be allowed, in which case they will exhibit stack-like behavior, but the typical use of a skip list will disallow duplicates. To disallow duplicates in this apprach, an extra step that searches the list for existence is needed, otherwise unwanted changes might be made to the structure before it is determined whether or not a duplicate is present:

    int jsw_insert(struct jsw_skip *skip, int key)
    {
        if (jsw_find(skip, key) == key)
        {
            return 0;
        }
        else
        {
            struct jsw_node *it = skip->head;
            struct jsw_node *item = new_node(key);
            int i;

            if (item->height > skip->height)
            {
                item->height = ++skip->height;
            }

            for (i = skip->height - 1; i >= 0; i--)
            {
                while (it->next[i] != NULL && key > it->next[i]->data)
                {
                    it = it->next[i];
                }

                if (i < item->height)
                {
                    item->next[i] = it->next[i];
                    it->next[i] = item;
                }
            }
        }

        return 1;
    }

One of the key points in insertion is raising the height of the list if the height of the new node is taller than the list. In these examples, the list only every grows by one level, but a more common implementation is to grow the list by the difference between the current height of the list and the height of the new node. This variation is preferred because everyone probably reads the pseudocode in skip list reports and not the text. ;-) The changes are simple:

    int jsw_insert(struct jsw_skip *skip, int key)
    {
        struct jsw_node *it = skip->head;
        struct jsw_node *item = new_node(key);
        int i;

        for (i = skip->height - 1; i >= 0; i--)
        {
            while (it->next[i] != NULL && key > it->next[i]->data)
            {
                it = it->next[i];
            }

            fix[i] = it;
        }

        if (it->next[0] != NULL && it->next[0]->data == key)
        {
            return 0;
        }
        else
        {
            int h;

            while (item->height > skip->height)
            {
                fix[skip->height++] = skip->head;
            }

            h = item->height;

            while (--h >= 0)
            {
                item->next[h] = fix[h]->next[h];
                fix[h]->next[h] = item;
            }
        }

        return 1;
    }

The top-down approach can also be modified for for this variation:

    int jsw_insert(struct jsw_skip *skip, int key)
    {
        if (jsw_find(skip, key) == key)
        {
            return 0;
        }
        else
        {
            struct jsw_node *it = skip->head;
            struct jsw_node *item = new_node(key);
            int i;

            while (item->height > skip->height)
            {
                ++skip->height;
            }

            for (i = skip->height - 1; i >= 0; i--)
            {
                while (it->next[i] != NULL && key > it->next[i]->data)
                {
                    it = it->next[i];
                }

                if (i < item->height)
                {
                    item->next[i] = it->next[i];
                    it->next[i] = item;
                }
            }
        }

        return 1;
    }

Which of these approaches (forced growth by one or unbounded growth) is better remains to be seen, though that leads me to believe that either works just as well as the other. However, in my experience, the former results in a faster algorithm, and the difference in structure is negligable, so my preference is to limit list growth to a single level. The difference between top-down and bottom-up insertion is also difficult to determine. The tradeoff of speed for space is tempting, and the top down algorithm certainly has the benefit of clarity. The choice, as usual, must be made on a case by case basis.

#### Linked insertion

Insertion into a linked skip list is a direct translation of the description into some pointer chases. As with classic insertion, the links can be added in a top-down manner as the search progresses, or by saving the previous links in an array and splicing new links in after the search. However, there is little point in using an auxiliary array as one of the primary benefits of a linked skip list is the lack of need for arrays. As you already know, the top-down insertion requires that a prior search for existence be made if the skip list does not allow duplicate keys:

    int jsw_insert(struct jsw_skip *skip, int key)
    {
        if (jsw_find(skip, key) == key)
        {
            return 0;
        }
        else
        {
            struct jsw_node *it, *save = NULL;
            int h = rheight(MAX);
            int i;

            if (h > skip->height)
            {
                skip->head = new_node(-1, NULL, skip->head);
                ++skip->height;
            }

            it = skip->head;

            for (i = skip->height; it != NULL; i--)
            {
                while (it->next != NULL && key > it->next->data)
                {
                    it = it->next;
                }

                if (i <= h)
                {
                    if (it->next == NULL || key != it->next->data)
                    {
                        it->next = new_node(key, it->next, NULL);
                    }

                    if (save != NULL)
                    {
                        save->down = it->next;
                    }

                    save = it->next;
                }

                it = it->down;
            }
        }

        return 1;
    }

This example shows a primary disadvantage of the linked approach. Rather than simply creating an array of pointers, each column must be constructed from multiple unique nodes. Not only is this a waste of space, it can be an issue if extra data aside from the key is changed for a column. Either all but the lowest level must be ignored, and thus result in an inaccurate list, or extra work must be done to change every node in the column. It also results in more calls to a memory manager, which is expensive. However, a free list of nodes can improve this cost drastically if deletions are common.

A linked skip list has only three real advantages over the classic skip list. First, a linked skip list is easier to visualize because it does not attempt to form a hybrid of arrays and sorted linked lists. That concept seems to be a stumbling block among beginning programmers and experienced programmers who are comfortable with trees. As such, I have found the linked skip list to be easier to implement correctly the first time whereas the classic skip list requires a keen eye for subtle indexing errors.

The second advantage that a linked skip list has over a classic skip list is that it is easier to convert a randomized skip list to a balanced skip list efficiently and without bending over backward. We shall see why this is so shortly. Finally, the linked skip list can more easily grow its maximum height as a side effect of simply adding a new head node rather than growing a dynamic array.

Aside from those two points, the classic skip list dominates. It is faster, takes less space in memory, requires fewer comparisons per search, requires fewer calls to the memory manager per insertion or deletion (though a free list alleviates the problem in a linked solution), does not require extra copies of the key, uses less expensive array indexing for one direction, has slightly better cache performance due to the contiguous memory of arrays, and the average and worst case time complexity is exactly the same. Concerning the growth issue, because the maximum height will be relatively small for huge skip lists, the cost of a few extra pointers in the head array of a classic skip list is minimal, so the benefit of the linked skip list is also minimal.

All in all, while a linked skip list has a few advantages and seems to be a superior first encounter for newcomers who are first learning the data structure, the classic skip list has much better properties for the real world. So my recommendation is to understand both, but unless you are teaching skip lists to someone, stick to the classic algorithms.

#### Traversal

Traversing a skip list is incredibly simple. For the classic skip list, it breaks down to traversing a linked list, starting at the lowest level. There is no magic to traversal, but there are a few considerations when implementing a skip list. First, the question of whether the list should be singly or doubly linked arises. A doubly linked skip list on the lowest level can provide two-way traversals with only a small amount of extra implementation effort. A doubly linked skip list on all levels also encourages search optimizations based on interpolation. If it could be more efficient to start a search at the back and move forward then the speed of a search could be improved, and a doubly linked skip list allows that.

Another traversal consideration is traversal of the other levels, or even a two dimensional traversal. The possibilities are endless, and I encourage you to be creative in how you can traverse a skip list, because you might find a use for it that you never saw before.

#### Classic traversal

The classic skip list promotes easy traversal because you the 0th index of the head array can act as a starting point with no work at all:

    void jsw_traverse(struct jsw_skip *skip, void (*action)(struct jsw_node*))
    {
        struct jsw_node *it = skip->head->next[0];

        while (it != NULL)
        {
            action(it);
            it = it->next[0];
        }
    }

#### Linked traversal

A linked traversal is harder to design because the skip list is meant to be searched efficiently from the top, not traversed from the bottom. In a naive linked skip list, the traversal would end up starting at the top, then finding the bottom, then performing a traversal in an L-shaped pattern:

    void jsw_traverse(struct jsw_skip *skip, void (*action)(struct jsw_node*))
    {
        struct jsw_node *it = skip->head;

        while (it->down != NULL)
        {
            it = it->down;
        }

        while (it != NULL)
        {
            it = it->next;
            action(it);
        }
    }

The linked skip list in this tutorial uses a bottom pointer to keep track of the head of the lowest level so as to avoid that first step. Now the traversal looks more like the classic traversal and is also more efficient because it does not need to find the bottom first:

    void jsw_traverse(struct jsw_skip *skip, void (*action)(struct jsw_node*))
    {
        struct jsw_node *it = skip->bottom->next;

        while (it != NULL)
        {
            action(it);
            it = it->next;
        }
    }

#### Classic deletion

Removing an item from a skip list is the inverse of inserting one. As usual, a search is performed and the update array is used to save the path. Then a simple linked list removal is performed for each level. The traditional code for a classic skip list is:

    int jsw_remove(struct jsw_skip *skip, int key)
    {
        struct jsw_node *it = skip->head;
        struct jsw_node *save;
        int i;

        for (i = skip->height - 1; i >= 0; i--)
        {
            while (it->next[i] != NULL && key > it->next[i]->data)
            {
                it = it->next[i];
            }

            fix[i] = it;
        }

        if (it->next[0] == NULL || it->next[0]->data != key)
        {
            return 0;
        }
        else
        {
            save = fix[0]->next[0];

            for (i = 0; i < skip->height; i++)
            {
                if (fix[i]->next[i] != NULL)
                {
                    fix[i]->next[i] = fix[i]->next[i]->next[i];
                }
            }

            while (skip->height > 0)
            {
                if (skip->head->next[skip->height - 1] != NULL)
                {
                    break;
                }

                skip->head->next[--skip->height] = NULL;
            }
        }

        free(save);

        return 1;
    }

Just as with insertion, deletion can remove the item as the search moves down the list rather than by building an array for fixing links. This top-down structure change is actually a more elegant solution in the absence of fingers because there is no need for an update array, and with a little extra thought, there is no real need for an extra existence search beforehand as some part of the search will be performed anyway. Of course, the code is easier to understand with the preliminary search:

    int jsw_remove(struct jsw_skip *skip, int key)
    {
        if (jsw_find(skip, key) != key)
        {
            return 0;
        }
        else
        {
            struct jsw_node *it = skip->head;
            struct jsw_node *save;
            int i;

            for (i = skip->height - 1; i >= 0; i--)
            {
                while (it->next[i] != NULL && key > it->next[i]->data)
                {
                    it = it->next[i];
                }

                if (i == 0)
                {
                    save = it->next[0];
                }

                if (it->next[i] != NULL)
                {
                    it->next[i] = it->next[i]->next[i];
                }

                if (skip->head->next[i] == NULL)
                {
                    --skip->height;
                }
            }

            free(save);
        }

        return 1;
    }

#### Linked deletion

The code for linked deletion is, like insertion, almost identical to classic deletion. I hesitated to provide code for it to help keep the tutorial shorter, but for completeness, here is the code for linked deletion using a top-down approach that the linked structure encourages:

    int jsw_remove(struct jsw_skip *skip, int key)
    {
        if (jsw_find(skip, key) != key)
        {
            return 0;
        }
        else
        {
            struct jsw_node *it = skip->head;
            struct jsw_node *save = NULL;

            while (it != NULL)
            {
                while (it->next != NULL && key > it->next->data)
                {
                    it = it->next;
                }

                if (it->next != NULL && key == it->next->data)
                {
                    save = it->next;
                    it->next = save->next;
                    free(save);
                }

                it = it->down;
            }
        }

        return 1;
    }

#### Balancing

Forcing balance to a skip list such that every search is guaranteed to be O(log N) is, like with binary search trees, a non-trivial problem. The problem can be simplified slightly by treating the number of skipped over items as a gap, and ensuring that no more than a certain number are ever skipped. This is the approach taken by most balanced skip list algorithms, and they follow the same principle as red black binary search trees, where the data structure is a simulation of a B-tree of order 4.

Also note that because a skip list is a list based simulation of a tree structure, the other height balanced tree algorithms can be modified with minimal effort to work with skip lists. However, detailed coverage of these algorithms is beyond the scope of this tutorial.

A balanced skip list, as defined by Ian Munroe, Thomas Papadakis, and Robert Sedgewick, ensures that the largest allowable gap is 3, and any structural changes to the list will ensure that this invariant is maintained. I highly recommend the paper “Deterministic skip lists” for a their detailed report on how to balance a skip list. A conceptual skip list created by this scheme is:

    Insert 0:
    *---0---*

    Insert 1:
    *---0---1---*

    Insert 2:
    *---0---1---2---*

    Insert 3:
    *-------1-----------*
    *---0---1---2---3---*

    Insert 4:
    *-------1---------------*
    *---0---1---2---3---4---*

    Insert 5:
    *-------1-------3-----------*
    *---0---1---2---3---4---5---*

    Insert 6:
    *-------1-------3---------------*
    *---0---1---2---3---4---5---6---*

    Insert 7:
    *-------1-------3-------5-----------*
    *---0---1---2---3---4---5---6---7---*

    Insert 8:
    *-------1-------3-------5---------------*
    *---0---1---2---3---4---5---6---7---8---*

    Insert 9:
    *---------------3---------------------------*
    *-------1-------3-------5-------7-----------*
    *---0---1---2---3---4---5---6---7---8---9---*

Notice that when a gap would be increased to 4, one of the columns in that gap is raised by one so that the invariant is not violated. However, because raising one column may violate the invariant for the next level up, a predictive algorithm is used, where the search moves in a top-down manner and raises any gaps of length 3 to ensure that any columns that are raised on a lower level do not break the balance invariant.

Now, the actual implementation is more efficient and easier to implement if each key “falls back” in the actual structure. Conceptually, the structure is the same as above, but the list structure would actually look like this:

    *
    3---------------*
    1-------3-------5-------7-------*
    0---1---2---3---4---5---6---7---8---9---*

The only difference is that instead of a column of header nodes, a single head at the top points down to the tallest column. Then the tallest column points down to the next tallest, and so on. This eases the transition of heights when an item is added or removed.

#### Balanced insertion

The code to implement this type of insertion is surprisingly simple, but it is littered with special cases if we continue to use **NULL** terminated linked skip lists. A much better solution is to define sentinels that have a value which is greater than any item in the list, and point to themselves. This approach ensures that pointer chases will remain within the data structure, and the large number of tests for a null pointer can be avoided. Here are the updated types:

    /* Max key for signed integers */
    #define MAX_KEY INT_MAX

    struct jsw_node
    {
        int data;
        struct jsw_node *n;
        struct jsw_node *d;
    };

    struct jsw_skip
    {
        struct jsw_node *head;
        struct jsw_node *bottom;
        struct jsw_node *tail;
    };

The **MAX\_KEY** value can be modified to work with any type as long as there is a representation for that type that is not allowed in the list. For example, using ASCII string keys it is a relatively safe assumption that the character represented by 127 is not going to show up in any keys, so **MAX\_KEY** could be defined as the following because a lexicographical comparison stops on the first differing character and uses it as the result:

    /* Max key for strings */
    #define MAX_KEY "\x7f"

The jsw\_node structure remains mostly unchanged, but because long chains of pointer chases will be used, the names next and down were shortened to **r** for “right” and **d** for “down”. The **jsw\_skip** structure has the most changes. The **head** link is still there, but there are two more links for the sentinel below the lowest level and the sentinel to mark the end of each level, named **bottom** and **tail**, respectively. More initialization effort is required for this approach as well, because the sentinels need to be initialized. So we will use a **new\_skip** function to allocate a skip list, initialize it, and then return it. The **new\_node** function is a trivial exercise, but in this tutorial I will assume it cannot fail:

    struct jsw_skip *new_skip()
    {
        struct jsw_skip *skip = malloc(sizeof *skip);

        if (skip == NULL)
        {
            return NULL;
        }

        skip->bottom = new_node(MAX_KEY, NULL, NULL);
        skip->tail = new_node(MAX_KEY, NULL, NULL);
        skip->head = new_node(MAX_KEY, skip->tail, skip->bottom);

        skip->bottom->r = skip->bottom;
        skip->bottom->d = skip->bottom;
        skip->tail->r = skip->tail;

        return skip;
    }

With a fresh new skip list created, we can proceed to perform balanced insertions. Amazingly enough, the code to do this is clean and simple, primarily because of the sentinels protecting against the need for special case tests:

    int jsw_insert(struct jsw_skip *skip, int key)
    {
        struct jsw_node *it;

        /* Set search params for insertion */
        skip->bottom->data = key;

        for (it = skip->head; it != skip->bottom; it = it->d)
        {
            while (key > it->data)
            {
                it = it->r;
            }

            /* Raise column */
            if (it->data > it->d->r->r->data)
            {
                it->r = new_node(it->data, it->r, it->d->r->r);
                it->data = it->d->r->data;
            }
            else if (it->d == skip->bottom)
            {
                return 0;
            }
        }

        /* Grow list if necessary */
        if (skip->head->r != skip->tail)
        {
            skip->head = new_node(MAX_KEY, skip->tail, skip->head);
        }

        return 1;
    }

The first thing of note is that the search key is assigned to the bottom sentinel. This is done to ease searching by ensuring that all searches will stop at the sentinel rather than loop infinitely, because the sentinel points to itself. Everything else is obvious in this incredibly elegant algorithm. Try tracing the execution of this function with the insertion example given above.

Because the structure of this skip list is slightly different from the linked skip list described previously, a column of header nodes is no longer used, you must keep in mind that a search should return the current item rather than the next item.

#### Balanced deletion

Deletion in a balanced skip list is, to put it mildly, a pain in the ass. There are several annoying cases, much like with red black trees, that result in a long, complicated, and ugly algorithm. Deletion is conceptually very simple, being the direct inverse of insertion, but the implementation proves to be exceptionally difficult, and it rivals deletion in a balanced binary search tree.

I have chosen, for the purposes of this tutorial, to break the algorithm up into several functions, with a global structure instance containing variables for walking through the skip list and making updates. Hopefully this will avoid the initial overwhelming impact that the algorithm has when one first looks at it. The global structure is simply a structure that has an iterator member, just like the it that we have been using, a save member for generic odd-jobs, and variables for saving the previous and next gaps, as well useful key values:

    struct j_walker
    {
        struct jsw_node *it;   /* Current item */
        struct jsw_node *prev; /* Previous gap */
        struct jsw_node *next; /* Next gap */

        struct jsw_node *save; /* Temporary holder */
        int lastb;             /* Previous key bottom */
        int lastu;             /* Previous key up */
    };

    static struct j_walker w; /* Variables for deletion */

The **jsw\_remove** function relies on other functions to handle the balancing and removal of the lowest level node, but a final second pass to fully clean up the structure is needed. This may seem a little odd at first, but it greatly simplifies the balancing step. Aside from maintaining the extra save points in the **j\_walker** struct, the code is simple and should be familiar by now:

    int jsw_remove(struct jsw_skip *skip, int key)
    {
        /* Set search params for deletion */
        skip->bottom->data = key;
        w.lastu = skip->head->data;
        w.it = skip->head->d;

        for (; w.it != skip->bottom; w.it = w.next)
        {
            while (key > w.it->data)
            {
                w.prev = w.it;
                w.it = w.it->r;
            }

            if (rebalance(skip) == 0)
            {
                return 0;
            }
        }

        /* Fix columns taller than 1 */
        w.it = skip->head->d;

        for (; w.it != skip->bottom; w.it = w.it->d)
        {
            while (key > w.it->data)
            {
                w.it = w.it->r;
            }

            if (key == w.it->data)
            {
                w.it->data = w.lastb;
            }
        }

        /* Shrink if necessary */
        if (skip->head->d->r == skip->tail)
        {
            w.save = skip->head;
            skip->head = w.save->d;
            free(w.save);
        }

        return 1;
    }

Now we wander into the confusing world of the **rebalance** function. There are four cases for a top-down deletion that mirrors the borrow and merge technique used by B-trees. Simply look at the gaps and make sure that they are greater than or equal to 1, and also less than 4. The code to handle a gap is simple once you understand the concept:

    static void fix_gap(struct jsw_skip *skip)
    {
        w.save = w.it->r;

        if (w.save->data == w.save->d->r->data || w.next == skip->bottom)
        {
            /* Lower next column */
            w.it->r = w.save->r;
            w.it->data = w.save->data;
            free(w.save);
        }
        else
        {
            /* Raise first of next gap, lower next column */
            w.it->data = w.save->d->data;
            w.save->d = w.save->d->r;
        }
    }

Unfortunately, this does not work for the last gap, where the previous gap needs to be adjusted as well. Fortunately, while the code to handle the last gap is a special case, it is relatively short:

    static void last_gap(struct jsw_skip *skip)
    {
        if (w.prev->data <= w.prev->d->r->data)
        {
            /* Lower prev column */
            if (w.next == skip->bottom)
            {
                w.lastb = w.prev->data;
            }

            w.prev->r = w.it->r;
            w.prev->data = w.it->data;
            free(w.it);
            w.it = w.prev;
        }
        else
        {
            /* Raise last of prev gap, lower prev column */
            if (w.prev->data == w.prev->d->r->r->data)
            {
                w.save = w.prev->d->r;
            }
            else
            {
                w.save = w.prev->d->r->r;
            }

            w.prev->data = w.save->data;
            w.it->d = w.save->r;
        }
    }

The last piece of the puzzle is a rebalance function that puts together **fix\_gap** and **last\_gap**, as well as handles the piddling pointer management and determines if the key is even in the skip list:

    static int rebalance(struct jsw_skip *skip)
    {
        w.next = w.it->d;

        if (w.it->data == w.next->r->data)
        {
            if (w.it->data != w.lastu)
            {
                fix_gap(skip);
            }
            else
            {
                last_gap(skip);
            }
        }
        else if (w.next == skip->bottom)
        {
            return 0;
        }

        w.lastu = w.it->data;

        return 1;
    }

Now, this process is very complicated compared to all of the other skip list algorithms. A balanced skip list is much faster than a randomized skip list, but the complexity of deletion usually results in the balanced skip list not being used. However, a cheap alternative is to take a trick from hash tables, where each node contains a flag saying whether it contains a deleted key or not. Then deletion becomes nothing more than a search that sets the flag as necessary:

    int jsw_remove(struct jsw_skip *skip, int key)
    {
        struct jsw_node *it = skip->head;
        int rc = 0;

        for (; it != skip->bottom; it = it->d)
        {
            while (key > it->data)
            {
                it = it->r;
            }

            if (key == it->data)
            {
                it->deleted = 1;
                rc = 1;
            }
        }

        return rc;
    }

Of course, this only delays the problem, but if deletions are considerably less frequent than insertions, this approach is an excellent alternative, and depending on the application, it is possible to simply set the flag for an item and then forget about it. But a more common implementation will modify the other algorithms to restore the flag if the key is inserted again, so as to avoid hairy issues with duplicates.

#### Performance

The skip list is a data structure that has been hyped to be much more than it really is. Yes, skip lists are flexible and powerful, just like any multidimensional data structure. Yes, skip lists offer logarithmic performance for searching, insertion, and deletion. No, skip lists are no more an alternative to balanced binary search trees than randomized binary search trees are. No, skip lists are not always as efficient as balanced binary search trees, and no, skip lists are not necessarily easier to implement in light of a realistic perspective, they just have a different class of problems that need to be dealt with.

#### Competition

In his reports on the skip list, Bill Pugh made it very clear that the intention of a skip list is as an alternative to a balanced binary search tree. To a point, this is true, as the skip list competes directly with randomized binary search trees (and related data structures such as treaps) and hash tables as an alternative to the more difficult balanced trees at the cost of an absolute performance guarantee.

Unfortunately, many people seem to have misunderstood the intention of the skip list and believe it to be a complete replacement for the balanced trees, which it is most certainly not. They also seem to be under the impression that skip lists are more efficient than all but the most heavily optimized balanced trees, which is also not necessarily true.

In reality, this is what a skip list is and does:

Skip lists are logarithmic with high probability, each level will typically have half of the nodes that the level below it has, and on average, a search will visit at most, two nodes for each level.

Due to a simpler fundamental design, skip lists are easier to optimize than binary search trees and because both linked lists and arrays are simple and familiar data structures, skip lists encourage experimentation. The simpler fundamental design also makes skip lists easier to understand.

Because skip lists are not designed only with fast searching in mind, non-trivial operations such as an iterator based traversal are much easier to visualize and implement than with binary search trees. Also, balancing schemes are easier to visualize and implement using skip lists because complicated and confusing rotations are not needed, only simple array or linked list operations that are well understood.

The description of a skip list does not suggest a recursive implementation. In fact, unlike binary search trees, skip lists are not recursive by nature and a recursive solution is typically more difficult to understand and optimize than a more conventional solution. To date, the only attempt at a recursive skip list was made by Robert Sedgewick in his algorithm books.

A classic skip list (as described by Bill Pugh) is easier to implement in theory, but due to the hybrid nature of the data structure, it is generally more difficult to implement in practice. Of course, even the most complicated skip list is still easier to implement than a balanced binary search tree. A classic skip list is probabilistic in nature, and thus competes directly with randomized binary search trees rather than balanced binary search trees.

A classic skip list, and all skip lists in general, require more comparisons than any variation of binary search trees. Skip lists also require more memory than binary search trees due to the need for multiple pointers, multiple nodes for each item, or multiple copies of the data. Common implementations also require an extra auxiliary array for making correct structural changes.

Due to the extra comparisons, skip lists are not always faster than binary search trees on average. Even a modest balanced binary search tree can outperform an optimized skip list in every case, given realistic input. A skip list will generally have roughly comparable performance to well balanced binary search trees, but to say that they are faster is a dangerous overgeneralization.

Skip lists by definition have a bounded height. This is not a hard rule, and it is possible to implement a skip list with a dynamically growing height, but there is a common misconception that the bounded height is a disadvantage. In reality, even a small height can contain a large number of elements, as each new level roughly doubles the potential number of items. So while a height of 15 can only hold just over 65,000 items, a modest increase to 19 can hold over 1 million items.

#### Conclusion

Skip lists are powerful, flexible, and interesting data structures. It is painfully obvious that the reality is not as wonderful as the hype, but with a realistic perspective, skip lists are more than just a viable alternative to certain tree based data structures, they are a data structure in their own right and can be used in a broader range of applications than their tree based cousins because they are based on a foundation that can be used effectively for more than just searching.

This tutorial is far from complete, but I have tried to be as thorough as possible. I encourage you to play with the code, experiment with different optimizations and ways of implementing the same structure, and search the web for other reports and papers that cover the few areas that I glossed over.

------------------------------------------------------------------------

© 2018 - Eternally Confuzzled
