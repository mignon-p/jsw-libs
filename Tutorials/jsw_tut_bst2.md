Binary Search Trees II
======================

In Binary Search Trees I you learned that binary search trees are a powerful data structure for storage and retrieval of data. Binary search trees are conveniently simple for simple operations, and more complicated operations are made possible, though not always simple, especially when rotations or other structural modifications are involved. I also hinted that the basic binary search tree algorithms have exceptionally bad performance in certain situations. There are three worst cases corresponding to data being input in ascending sorted order:

      A
       \
        B
         \
          C
           \
            D
             \
              E

Data being inserted in descending sorted order:

              E
             /
            D
           /
          C
         /
        B
       /
      A

Or data being inserted in alternating, outside-in order:

      A
       \
        E
       /
      B
       \
        D
       /
      C

None of these make for particularly efficient binary search trees because they are all structurally equivalent to linked lists. The result of all of this is that the good O(log N) performance of binary search trees degenerates to O(N). Because binary search trees are best used for large data sets, this can result in severe performance problems. Unfortunately, the classic methods for maintaining an optimal tree are frighteningly complicated. A simple solution is to recognize the performance of the average case of a binary search tree.

Performance of a basic binary search tree is O(log N) on average. The average case is when the data being inserted is sufficiently random to create what is called a randomized binary search tree, where the each new item has a 1/N probability of being placed at the root in any subtree:

The most obvious way to create a randomized binary search tree is to permute the input randomly. However, most times we don't have access to all of the input at any given time. In fact, usually we will be inserting items one at a time into the tree from a stream or object factory. This makes it very difficult to permute the data before inserting it into the tree.

A randomized binary search tree can be created if every item in the data set has an equal probability of being the root item. This suggests the use of the root insertion routines devised in the last tutorial. At every point in the insertion, make a random selection of root insertion or basic insertion. The end result will be a randomized binary search tree. The algorithm is sound, an surprisingly easy to write. For this implementation of a randomized binary search tree insertion, we will be using the **jsw\_node** and **jsw\_tree** structures defined in the previous tutorial:

    struct jsw_node
    {
        struct jsw_node *link[2];
        int data;
    };

    struct jsw_tree
    {
        struct jsw_node *root;
        size_t size;
    };

Consider, if you will, a simple recursive insertion, much like the pair of functions shown in Binary Search Trees I:

    struct jsw_node *jsw_insert_r(struct jsw_node *root, struct jsw_node *item, int key)
    {
        if (root == NULL)
        {
            root = item;
        }
        else
        {
            int dir = root->data < key;

            root->link[dir] = jsw_insert_r(root->link[dir], item, key);
        }

        return root;
    }

    void jsw_insert(struct jsw_tree *tree, struct jsw_node *item, int key)
    {
        tree->root = jsw_insert_r(tree->root, item, key);
        ++tree->size;
    }

Assuming a sorted sequence of keys, a degenerate tree is sure to be the result of running this algorithm. However, two simple changes will reduce the probability of that degenerate tree to a vanishingly small number. The first modification is the addition of the size argument to **jsw\_insert\_r**, which will be given the initial value of **tree-\>size** in **jsw\_insert**. For each recursive call to **jsw\_insert\_r**, half of **sz** would be passed to give a rough estimate of the number of items in the subtree.

The second modification is to perform an insertion at the root, with the current subtree as the root to insert at. This root insertion would be made only with a probability of 1/sz. All in all, assuming the existence of **jsw\_root\_insert**, the changes to **jsw\_insert\_r** and **jsw\_insert** look like this:

    struct jsw_node *jsw_insert_r(struct jsw_node *root, struct jsw_node *item, int key, int sz)
    {
        if (root == NULL)
        {
            root = item;
        }
        else if (rand() % (sz + 1) == sz) /* Probability 1/sz */
        {
            root = jsw_root_insert(root, item, key);
        }
        else
        {
            int dir = root->data < key;

            root->link[dir] = jsw_insert_r(root->link[dir], item, key, sz / 2);
        }

        return root;
    }

    void jsw_insert(struct jsw_tree *tree, struct jsw_node *item, int key)
    {
        tree->root = jsw_insert_r(tree->root, item, key, tree->size);
        ++tree->size;
    }

This use of random numbers is likely to be inferior, and it is used only for simplicity. For a thorough discussion on the merits of various random number implementations and uses, see the Random Numbers tutorial. For now, using the standard library **rand** function with modulus will serve our purposes.

Assuming the existence of **jsw\_root\_insert** is all well and good when brainstorming, but a concrete implementation would be helpful in real code. Without going into too much detail, here's one example of how to do it:

    struct jsw_node *jsw_root_insert(struct jsw_node *root, struct jsw_node *item, int key)
    {
        if (root == NULL)
        {
            root = item;
        }
        else
        {
            struct jsw_node *save;
            int dir = root->data < key;

            root->link[dir] = jsw_root_insert(root->link[dir], item, key);

            /* Rotate on root */
            save = root->link[dir];
            root->link[dir] = save->link[!dir];
            save->link[!dir] = root;
            root = save;
        }

        return root;
    }

These functions cover no new ground. Anyone who has read Binary Search Trees I should have no trouble with them.

An alternative to inserting as normal and then lifting the new item to a selected root with probability is splicing the subtree with probability. This is the method used by the “official” randomized binary search tree data structure as described by Martinez and Roura. The top level insertion algorithms are not much changed and familiar:

    struct jsw_node *jsw_insert_r(struct jsw_node *root, struct jsw_node *item, int key)
    {
        int n = (root == NULL) ? 0 : root->size;

        if (rand() % (n + 1) == n)
        {
            return jsw_root_insert(root, item);
        }
        else
        {
            int dir = root->data < key;

            root->link[dir] = jsw_insert_r(root->link[dir], item, key);
            ++root->size;
        }

        return root;
    }

    void jsw_insert(struct jsw_tree *tree, struct jsw_node *item, int key)
    {
        tree->root = jsw_insert_r(tree->root, item, key);
    }

Instead of waiting until **root** is **NULL** to perform the insertion, the probability test is the sole factor in determining a location to insert. Note that when **root** is a terminal node, **n** will be 0 and the probability test will succeed, thus simulating the common leaf insertion. This ensures that the edge case of inserting at a terminal node is satisfied.

The splicing operation will be done in **jsw\_root\_insert** with the aid of a helper function called **jsw\_split**. **jsw\_root\_insert** does not perform any searching; instead, it simply makes room for the new item and splices it into the tree at the chosen root. This can be a tricky operation because it must be guaranteed that the tree will remain a valid binary search tree. Therefore, **jsw\_split** must be recursive as well, so that it can clean up any subtrees of the new root. Naturally, **jsw\_split** does not have to be recursive, but this is a case where recursion makes life easier:

    void jsw_split(struct jsw_node *root, struct jsw_node *item, struct jsw_node **s, struct jsw_node **g)
    {
        if (root == NULL)
        {
            *s = *g = NULL;
        }
        else if (item->data < root->data)
        {
            *g = root;
            jsw_split(root->link[0], item, s, &((*g)->link[0]));
        }
        else
        {
            *s = root;
            jsw_split(root->link[1], item, &((*s)->link[1]), g);
        }
    }

    struct jsw_node *jsw_root_insert(struct jsw_node *root, struct jsw_node *item)
    {
        struct jsw_node *s, *g;

        jsw_split(root, item, &s, &g);

        root = item;
        root->link[0] = s;
        root->link[1] = g;
        root->size = 1;

        root->size += s == NULL ? 0 : s->size;
        root->size += g == NULL ? 0 : g->size;

        return root;
    }

This implementation of **jsw\_root\_insert** is very simple in that it hands off most of the work to **jsw\_split**. **jsw\_root\_insert** simply takes the results of **jsw\_split** and assigns them to **root**, then ensures that the sizes are properly adjusted to match the structural changes to the subtree. **jsw\_split** is somewhat tricky in how it recursively maintains a valid search tree structure. Consider the following tree:

       b
      / \
     a   e
        / \
       d   g

Let's say we want to insert **c** into the tree. **jsw\_split** needs to make room for **c** without breaking the binary search tree invariant. This is done by separating the two subtrees of the new node without breaking the invariant. Here is a sample execution of splitting the subtrees to fit **c** in. The most important part to understanding this algorithm is recognizing that each recursive call uses a child of either **s** or **g**. (**!** represents the current node, **?** is an indeterminate pointer, and **~** is a null pointer):

       b! <-----(s)
      / \
     a   e
        / \
       d   g


       b <------(s)
      / \
     a   e! <---(g)
        / \
       d   g


       b <------(s)
      / \
     a   e <----(g)
        / \
       d!  g
       ^
       |
       +--------(g->link[0])


       b <------(s)
      / \
     a   e <----(g)
        / \
       d   g
      /^
     ~!|
       |
       +--------(g->link[0])


       b <------(s)
      / \
     a   e <----(g)
        / \
       d   g
      /^
     ~!|
     ^ |
     | +--------(g->link[0])
     |
     +----------(g->link[0]->link[0])

After **jsw\_split** has divided up the tree, the pieces look something like the following diagram. Now, **jsw\_root\_insert** only has to use **s** and **g** to restructure the tree. Follow along yourself using the code to see how the end result is correct:

      (s)    (g)
       b      e      c
      / \    / \    / \
     a   ~  d   g  ~   ~


            c
          /   \
         b     e
        / \   / \
       a   ~ d   g

All of this trickery is, of course, meant to maintain the binary search tree invariant that every item is greater than the values in its left subtree and less the values in its right subtree. Without it, we would be forced to use leaf insertion and rotations to avoid the difficult problem of correctly splicing in a binary search tree.

Note that **jsw\_insert** allows duplicate keys with this implementation. There are several ways of disallowing duplicates, ranging from the simple but somewhat inefficient to the complicated but very fast. With splicing, the easiest solution by far is to call **jsw\_find** before ever performing the insertion:

    int jsw_insert(struct jsw_tree *tree, struct jsw_node *item, int key)
    {
        if (jsw_find(tree, key))
        {
            return 0;
        }

        tree->root = jsw_insert_r(tree->root, item, key);

        return 1;
    }

Removal from this style of randomized binary search tree is “simply” the inverse of splitting called, creatively, join. The code for **jsw\_remove** is a comfortable algorithm at this point:

    struct jsw_node *jsw_remove_r(struct jsw_node *root, struct jsw_node **old_item, int key)
    {
        struct jsw_node *save;

        if (root != NULL)
        {
            if (key == root->data)
            {
                save = jsw_join(root->link[0], root->link[1]);
                *old_item = root;
                root = save;
            }
            else
            {
                int dir = root->data < key;

                root->link[dir] = jsw_remove_r(root->link[dir], old_item, key);
                --root->size;
            }
        }

        return root;
    }

    int jsw_remove(struct jsw_tree *tree, struct jsw_node **old_item, int key)
    {
        if (!jsw_find(tree, key))
        {
            return 0;
        }

        tree->root = jsw_remove_r(tree->root, old_item, key);

        return 1;
    }

What **jsw\_join** does is remove **root** from the structure and reconnect the broken links into a valid binary search tree using probability such that the randomness of the tree can be maintained. The code is short, but may not be immediately obvious:

    struct jsw_node *jsw_join(struct jsw_node *left, struct jsw_node *right)
    {
        int ln = left == NULL ? 0 : left->size;
        int rn = right == NULL ? 0 : right->size;

        if (ln + rn == 0)
        {
            return NULL;
        }
        else if (rand() % (ln + rn) < ln)
        {
            left->link[1] = jsw_join(left->link[1], right);
            left->size += rn;
            return left;
        }
        else
        {
            right->link[0] = jsw_join(left, right->link[0]);
            right->size += ln;
            return right;
        }
    }

To map the execution of **jsw\_join**, we will use a similar tree as with **jsw\_split**, except this time removing **e** instead of adding **c** at **e**:

       b
      / \
     a   g
        / \
       d   h
        \
         e

Given the left and right links of **g** (**d** and **h**), **jsw\_join** first checks to see if **g** was a terminal node by testing the sum of its subtree's sizes. If **g** was a terminal node, we could simply replace it with **NULL** and terminate. However, in this case **g** has two children, so more work is required. Using the same probability as with insertion (1/sz), **jsw\_join** calls itself recursively, looking for a valid replacement based on the probability test. Assuming the right path is chosen, the resulting tree looks like this:

       b
      / \
     a   d
          \
           e
            \
             h

Had the left path been chosen, the final structure would be:

       b
      / \
     a   h
        /
       d
        \
         e

Because tracing this execution is a trivial exercise, and tracing recursive algorithms is good practice, I leave it to you to figure out how the algorithm gets to the final structure.

#### Treaps

The aptly named treap is a data structure that maintains both the invariant for a binary search tree and the invariant for a heap. When the priority for the heap is a randomly generated number, the result is a surprisingly well balanced randomized binary search tree. A treap must conform to two hard rules:

Binary search tree invariant (for **u** with a left child **v** and a right child **w**):

    v->data < u->data < w->data

Heap invariant:

    w->priority < v->priority < u->priority

By assigning priorities randomly or by hashing the data and then maintaining both the binary search tree condition and the heap condition, we have an excellent chance of getting a well balanced binary search tree. The worst case will still be O(n), as with all randomized binary search trees, but the chances of seeing that case are rather slim. In cases where guaranteed best case isn't necessary, a treap will perform adequately and has the benefit of being easy to implement.

#### Insertion

Insertion into a treap is straightforward. With a new item and a random priority, simply perform a normal binary search tree insertion and then walk back up the tree performing rotations until the heap condition is met. This is one of the two relatively simple options for inserting a new item into a binary search tree other than at a leaf. By using rotations we can guarantee that the binary tree invariant will remain valid. However, a few changes will need to be made to the original **jsw\_node**:

    struct jsw_node
    {
        struct jsw_node *link[2];
        int priority;
        int data;
    };

    struct jsw_node NullItem = { &NullItem, &NullItem, RAND_MAX, 0 };
    struct jsw_node *Null = &NullItem;

    struct jsw_tree
    {
        struct jsw_node *root;
    };

Notice the added priority member. Of immediate notice is the fact that I have defined two global variables of **jsw\_node**, an instance initialized to boundary values and a pointer initialized to the address of the instance. Our treap will be using **Null** to mark a terminal node rather than a null pointer as has been used thusfar. This may seem like an unnecessary change, but it greatly simplifies the removal algorithm we will be using.

The recursive algorithm to implement **jsw\_insert** as a treap is simplicity itself:

    struct jsw_node *jsw_insert_r(struct jsw_node *root, struct jsw_node *item, int key)
    {
        if (root == Null)
        {
            root = item;
        }
        else
        {
            int dir = root->data < key;

            root->link[dir] = jsw_insert_r(root->link[dir], item, key);

            if (root->priority < root->link[dir]->priority)
            {
                struct jsw_node *save = root->link[dir];

                root->link[dir] = save->link[!dir];
                save->link[!dir] = root;
                root = save;
            }
        }

        return root;
    }

    void jsw_insert(struct jsw_tree *tree, struct jsw_node *item, int key)
    {
        tree->root = jsw_insert_r(tree->root, item, key);
    }

This function is painfully simple. The only potentially confusing part is the use of **dir** to determine a right or left rotation, though once the idiom is understood, it actually results in shorter and simpler code. You can avoid this complexity by using two cases instead if you so choose:

    if (key < root->data)
    {
        root->link[0] = jsw_insert_r(root->link[0], item, key);

        if (root->priority < root->link[0]->priority)
        {
            struct jsw_node *save = root->link[0];

            root->link[0] = save->link[1];
            save->link[1] = root;
            root = save;
        }
    }
    else
    {
        root->link[1] = jsw_insert_r(root->link[1], item, key);

        if (root->priority < root->link[1]->priority)
        {
            struct jsw_node *save = root->link[1];

            root->link[1] = save->link[0];
            save->link[0] = root;
            root = save;
        }
    }

Of course, that defeats the purpose of using the link array in **jsw\_node**, though it does conform to the traditionally accepted design of binary search trees found in books and classrooms.

Using the lessons of Binary Search Trees I, the astute reader may discover that the treap insertion algorithm bears a striking similarity to basic root insertion. This leads the way to a non-recursive algorithm with no trouble at all:

    int jsw_insert(struct jsw_tree *tree, struct jsw_node *item, int key)
    {
        struct jsw_node *walk;
        struct jsw_node *up[50];
        int dir;
        int top = 0;

        /* Handle empty tree case */
        if (tree->root == Null)
        {
            tree->root = item;
            return 1;
        }

        /* Search for an empty link */
        walk = tree->root;

        for (;;)
        {
            if (walk->data == key)
            {
                return 0;
            }

            dir = walk->data < key;

            if (walk->link[dir] == Null)
            {
                break;
            }

            up[top++] = walk;
            walk = walk->link[dir];
        }

        /* Insert the new item */
        walk->link[dir] = item;

        /* Walk back up and rotate */
        while (item != tree->root)
        {
            if (walk->priority > item->priority)
            {
                break;
            }

            dir = item == walk->link[1];
            walk->link[dir] = item->link[!dir];
            item->link[!dir] = walk;

            /* Notify the rest of the tree of changes */
            if (top > 0 && up[top - 1] != Null)
            {
                dir = (walk == up[top - 1]->link[1]);
                up[top - 1]->link[dir] = item;
            }

            /* Reseat root if needed */
            if (tree->root == walk)
            {
                tree->root = item;
            }

            /* Move up and start over */
            walk = up[--top];
        }

        return 1;
    }

It should be obvious that this is a cut and paste from Binary Search Trees I with a few minor modifications to maintain the heap invariant. What could be easier than stealing existing code and mangling it to suit your needs?

#### Removal

Removal of a node is not as important as insertion, and many texts omit the code for it. While I can sympathize, I prefer to give at least a little working code whenever it would be of benefit. This feeling stems from trying in vain to weed out useful code snippets for removal algorithms when learning data structures myself. The algorithm for removing from a treap is exactly the opposite of insertion, as surprising as that may be. Find the item and perform rotations (walking the item down the tree) until it is a leaf, then remove it. This operation is typically called “pushdown”, once again proving that computer scientists have very little imagination. The algorithm is short:

    struct jsw_node *jsw_remove_r(struct jsw_node *root, struct jsw_node **old_item, int key)
    {
        if (root != Null)
        {
            struct jsw_node *save;

            if (key == root->data)
            {
                int dir = root->link[0]->priority > root->link[1]->priority;

                save = root->link[dir];
                root->link[dir] = save->link[!dir];
                save->link[!dir] = root;
                root = save;

                if (root != Null)
                {
                    root = jsw_remove_r(root, old_item, key);
                }
                else
                {
                    *old_item = root->link[1];
                    root->link[1] = Null;
                }
            }
            else
            {
                int dir = root->data < key;

                root->link[dir] = jsw_remove_r(root->link[dir], old_item, key);
            }
        }

        return root;
    }

    void jsw_remove(struct jsw_tree *tree, struct jsw_node **old_item, int key)
    {
        tree->root = jsw_remove_r(tree->root, old_item, key);
    }

The search is simple: If **root** is not a terminal node, move left or right accordingly. Otherwise perform the appropriate rotation and continue to recurse as long as **root** is not a terminal node. If **root** is a terminal node, then a simple replacement with **Null** will be sufficient to remove it from the tree. After removing the item, the algorithm must then walk back up the search path and repair any unraveled links.

By studying this code, you can see why I chose to use a sentinel instead of a null pointer to mark terminal nodes. If I had used a null pointer, the following section of code Would surely break because root is dereferenced in the case where it is an invalid item.

    if (root != Null)
    {
        root = jsw_remove_r(root, old_item, key);
    }
    else
    {
        *old_item = root->link[1];
        root->link[1] = Null;
    }

#### Conclusion

Binary search trees are a good start, but they have a nasty worst case that happens to be uncomfortably common. By using a randomized algorithm to restructure the tree as items are inserted, we can avoid those worst cases most of the time. While I chose to describe only three variations of randomized binary search trees, there are many others. A similarity to the quicksort algorithm should not go unnoticed either. The basic quicksort algorithm is like the basic binary search tree algorithms: fast, but with a common worst case that strongly affects performance. With the benefit of randomization, both quicksort and binary search trees can minimize the probability of encountering a worst case situation, and thus, become far more useful to the average programmer without much more effort.

------------------------------------------------------------------------

© 2018 - Eternally Confuzzled
