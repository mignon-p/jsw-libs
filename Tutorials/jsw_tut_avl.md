# AVL Trees

  

New programmers who are introduced to binary search trees quickly learn
that if items are inserted in certain orders, the performance of the
tree degenerates into that of a glorified linked list. Many brain cells
have been devoted to the task of finding efficient ways to avoid these
worst cases. Many exceedingly clever solutions have been developed, but
only a handful have made it into public knowledge and even fewer into
common use. Of those solutions, height balanced trees are the most
common, and the AVL tree is probably the oldest of the height balanced
trees.

This tutorial will cover, often in painful detail, the concept behind
AVL trees and implementation issues with a practical eye. If you are a
graduate student looking for lemmas and proofs, or detailed mathematical
forumlae proving the performance claims made, this tutorial is not for
you. I am neither qualified nor compelled to present such things because
AVL trees have been around for a very long time and their performance
has been exhaustively studied. I see no point in restating what others
have written better than I could. For the graduate students or anyone
else interested, an excellent analysis of AVL trees is made in Donald
Knuth's “The Art of Computer Programming”, volume 3.

If you are a struggling professional, or an amateur looking to broaden
your skills, look no further. This tutorial is written for you, the
average programmer who wants to learn what these mysterious “balanced
trees” are without having to work through the impenetrable code of a
research paper or textbook. Even if you don't care about the theory or
the implementation and just want code to solve a problem, this tutorial
provides several working versions of insertion and deletion in AVL
trees. The code is all public domain, so do with it whatever you want as
my gift to you.

I talk about the impenetrable code of papers and textbooks, but there
are many who would call my own approach impenetrable as well. I don't
use conventional solutions, so to minimize the shock of my
implementations, let's cover some of the key ideas behind why my code is
the way it is. Most noticeable is my use of an array of two links rather
than two separate links in the node structure:

    struct jsw_node
    {
        int data;
        struct jsw_node *link[2];
    };

The whole point of this is to avoid symmetric cases. Balanced tree
algorithms can be very verbose because code needs to be duplicated for
both the left and right subtree cases. Rather than suffer these
symmetric cases, I prefer to merge them into a single case through the
use of an easily computed array index. While a classic recursive binary
search tree insertion would look like this:

    struct jsw_node
    {
        int data;
        struct jsw_node *left;
        struct jsw_node *right;
    };
    
    struct jsw_node *make_node(int data)
    {
        struct jsw_node *rn = malloc(sizeof *rn);
    
        if (rn != NULL)
        {
            rn->data = data;
            rn->left = rn->right = NULL;
        }
    
        return rn;
    }
    
    struct jsw_node *jsw_insert(struct jsw_node *tree, int data)
    {
        if (tree == NULL)
        {
            tree = make_node(data);
        }
        else if (data < tree->data)
        {
            tree->left = jsw_insert(tree->left, data);
        }
        else
        {
            tree->right = jsw_insert(tree->right, data);
        }
    
        return tree;
    }

The same function with my idiom would merge the left and right cases
into one through the use of a direction index:

    struct jsw_node
    {
        int data;
        struct jsw_node *link[2];
    };
    
    struct jsw_node *make_node(int data)
    {
        struct jsw_node *rn = malloc(sizeof *rn);
    
        if (rn != NULL)
        {
            rn->data = data;
            rn->link[0] = rn->link[1] = NULL;
        }
    
        return rn;
    }
    
    struct jsw_node *jsw_insert(struct jsw_node *tree, int data)
    {
        if (tree == NULL)
        {
            tree = make_node(data);
        }
        else
        {
            int dir = tree->data < data;
    
            tree->link[dir] = jsw_insert(tree->link[dir], data);
        }
    
        return tree;
    }

Now, at this level there don't seem to be any savings, and if you're
unfamiliar with the idiom, it might take a moment to verify the
correctness of the algorithm. However, consider a balanced tree
algorithm that does more than simply walk down the tree in each case:

    struct jsw_node *jsw_insert(struct jsw_node *tree, int data)
    {
        if (tree == NULL)
        {
            tree = make_node(data);
        }
        else if (data < tree->data)
        {
            tree->left = jsw_insert(tree->left, data);
            /* Fifty lines of left balancing code */
        }
        else
        {
            tree->right = jsw_insert(tree->right, data);
            /* Fifty lines of right balancing code */
        }
    
        return tree;
    }

That's 50 lines of extra code if you believe the comments, which is very
hard for even a talented programmer to keep in his or her head all at
once. In this case, my merging idiom is far more beneficial because it
eliminates half of that code:

    struct jsw_node *jsw_insert(struct jsw_node *tree, int data)
    {
        if (tree == NULL)
        {
            tree = make_node(data);
        }
        else
        {
            int dir = tree->data < data;
    
            tree->link[dir] = jsw_insert(tree->link[dir], data);
            /* Fifty lines of dir balancing code */
        }
    
        return tree;
    }

Shorter functions are often easier to understand, and more importantly,
easier to check for correctness. In a traditional balanced tree
implementation, the author has to be very careful to make the left and
right cases perfectly symmetric. If this is not done correctly, the code
may work fine except in uncommon situations where it would fail
mysteriously. By merging the cases together, this repetition is avoided
and more effort can be devoted to making sure that a single case is
correct, because if the single case is correct, the symmetric case is
also correct. This is a basic tenet of good programming practice: avoid
code repetition. Once this idiom is understood, everything will fall
into place.

How does the comparison for calculating a direction index work though?
Put simply, if you want to move down the right link, ensure that the
comparison results in 1, otherwise the comparison will result in 0 and
you will move down the left link. In the classic algorithm, the
comparison “**data \< tree-\>data**” moves you down the left link, but
if **data** is less than **tree-\>data**, the result is 1 and that's the
wrong direction. So you reverse the test by saying “**tree-\>data \<
data**”. If this test is true then **data** is greater than or equal to
**tree-\>data**, and you want to move down the right link, which will
happen because the result of the test is 1. Alternatively, you could use
“**data \>= tree-\>data**” with the same results. Consider a case where
**data** is 5 and **tree-\>data** is 11:

    5 < 11  == 1  /* Wrong! We want to go left */
    11 < 5  == 0  /* Correct! We want to go left */
    5 >= 11 == 0  /* Correct! */

Of course, this assumes that if duplicate items are allowed, they are
placed in the right subtree, which is by far the most common method if
duplicates are allowed in a binary search tree implementation. Now let's
consider the opposite case, just for completeness. If **data** is 27 and
**tree-\>data** is 19, the comparisons look like this:

    27 < 19  == 0  /* Wrong! We want to go right */
    19 < 27  == 1  /* Correct! 1 is right */
    27 >= 19 == 1  /* Correct! */

#### Concept

Basic binary search trees can degenerate into linear data structures in
three cases. First, the data could be inserted in ascending or
descending sorted order. This results in trees with nothing but left or
right links. The third case is far less common, but if items are
inserted in alternating order from the outside in, the same degenerate
case arises because there is only one choice at each node and the effect
is a linear data structure:

``` 
 0                                  3          0

   \                              /              \

     1                          2                  3

       \          or          /          or      /

         2                  1                  1

           \              /                      \

             3          0                          2
```

This is basically an inefficient linked list, because the binary search
tree algorithms expect each node to have two paths rather than one,
whereas linked list algorithms are written with a linear structure in
mind. The result is a linear data structure that has expensive
algorithms. What we want in a binary search tree is a broad and flat
structure, where each node has two links and any path is logarithmic to
the number of nodes in the tree:

``` 
           3

       /       \

     1           5

   /   \       /   \

 0       2   4       6
```

Unfortunately, forcing such perfect balance is very expensive because it
requires a global restructuring that visits almost every node in the
tree and guarantees balance at each subtree. Alternatives for global
balancing use an auxiliary array, which isn't much better. So we have to
settle for less than perfect by only making local changes that meet a
well chosen invariant. However, with the AVL invariant, these local
changes result in a structure that is still very close to perfect.

A binary search tree that meets the AVL invariant is balanced if the
height of a node's subtrees differ by no more than 1. In the following
diagram, the first two trees are AVL trees but the third is not because
the left subtree of 5 has a height of 2 while the right subtree is a
null link and has a height of 0:

``` 
           3                        5                  5

       /       \                 /     \             /

     1           5             3         7         3

   /   \       /   \             \                   \

 0       2   4       6             4                   4
```

To maintain this invariant, certain structural changes must be made to
force subtrees into balance. Of course, these structural changes need to
be carefully considered because the AVL invariant is not the only rule
that cannot be violated. AVL trees inherit the rule that all items in a
node's left subtree must be lower in value and all items in the right
subtree must be greater or equal in value. So to maintain the AVL
invariant, we can't just splice nodes to our heart's delight and expect
a valid binary search tree. Rotations must be used to ensure that both
the binary search tree and AVL invariants are not violated.

It's easy to see that the AVL invariant can only be violated in two
cases when the symmetric cases are merged. First, if a subtree is too
long in the same direction, a single rotation can bring the structure
back into balance without breaking the binary search tree invariant. In
the next diagram, a left rotation at 3 is made to bring the subtree into
balance:

``` 
 3                     4

   \                 /   \

     4       -->   3       5

       \

         5
```

The code to perform a single rotation is simple. Care must be taken to
both rotate in the right direction and to correctly transfer the
children of the affected nodes. In this case, 4's left child must be
reassigned to 3's right child because 4's left child will be 3 and 4's
right child is free. This function uses **dir** as the direction of the
rotation, so a left rotation (such as above) will find **dir** as 0 when
the function is called. In the above diagram, **root** would be 3, and
**save** would be 4. The function returns the new root for reassignment
into the tree.

    struct jsw_node *jsw_single(struct jsw_node *root, int dir)
    {
        struct jsw_node *save = root->link[!dir];
    
        root->link[!dir] = save->link[dir];
        save->link[dir] = root;
    
        return save;
    }

The second case that would violate the AVL invariant is if a subtree is
too long in the opposite direction, where two rotations are required to
return the structure to balance. The first rotation is at the subtree,
then the second is at the root. Notice how the binary search tree
invariant is maintained throughout the entire operation, including the
intermediate step of the first rotation:

``` 
 3             3                     4

   \             \                 /   \

     5   -->       4       -->   3       5

   /                 \

 4                     5
```

The code for a double rotation is only slightly longer than a single
rotation and simply combines two rotations in opposite directions.
Another option that demonstrates this is to call **jsw\_single** twice
in the body of **jsw\_double** instead of doing the double rotation
manually. However, to better show how a double rotation works, I've
hardcoded the operations for now:

    struct jsw_node *jsw_double(struct jsw_node *root, int dir)
    {
        struct jsw_node *save = root->link[!dir]->link[dir];
    
        root->link[!dir]->link[dir] = save->link[!dir];
        save->link[!dir] = root->link[!dir];
        root->link[!dir] = save;
    
        save = root->link[!dir];
        root->link[!dir] = save->link[dir];
        save->link[dir] = root;
    
        return save;
    }

This is all well and good, but how does one determine when a subtree is
out of balance in one of these ways? By storing extra balance
information in each node. For AVL trees, the minimum extra space
required is two bits. However, in C and C++ it's hard to get a portable
and easy to use implementation that uses this minimum, so a variety of
other methods are used in practice. The first method is the one that I
will be using in this tutorial; a simple integer:

    struct jsw_node
    {
        int data;
        int balance;
        struct jsw_node *link[2];
    };
    
    struct jsw_tree
    {
        struct jsw_node *root;
    };

Variations of even this solution exist, with varying success and
reasons. Most commonly, a signed or unsigned char is used with space
efficiency in mind, other times a signed or unsigned short with similar
reasoning. However, for certain implementations (of which one will be
explored in this tutorial) a signed type is easier to understand because
the balance information consists of the values -1, 0, and +1 for a
taller left subtree, two subtrees of equal height, and a taller right
subtree, respectively. These balance factors represent the difference in
height between two subtrees. While an unsigned type will work under this
scheme, it would confuse too many people for my tastes. Another method
seen occasionally is a two bit bitfield.

The most important decision in setting up these balance factors is
whether they should be bounded or unbounded. An AVL tree with bounded
balance factors guarantees that the values will only be within a
specified range. By using only two bits, the range is -1, 0, and +1,
which is a little too restrictive for a simple implementation. Our
bounded range will be -2, -1, 0, +1, and +2, with -2 and +2 being
temporary values that signify a need to rebalance. Any range could be
used, but balance factors in this range can be updated easily with
minimal effort. Here are the trees given previously with bounded balance
factors:

``` 
               3,0                         5,-1                 5,-2

           /         \                   /      \             /

       1,0             5,0          3,-1          7,0    3,+1

     /     \         /     \             \                    \

 0,0         2,0  4,0        6,0           4,0                  4,0
```

Another option is an unbounded balance factor. While this sounds like a
limitation, in our case it really isn't. While one option is to use the
number of nodes in a subtree as the balance factor, this could easily
overflow a small integer. In this tutorial, we will be using an
unbounded balance factor of the longest path. Under such a scheme the
root of the tree will tell you how tall the tree is, which is useful
information in and of itself. Unlike the number of nodes in a subtree,
the longest path will not overflow an integer in the forseeable future,
so there is no hidden limitation. Here are the same three trees with
unbounded balance factors. Notice that the balance of each node is the
longest path in its subtrees:

``` 
               3,2                        5,2                5,2

           /         \                  /     \            /

       1,1             5,1          3,1         7,0    3,1

     /     \         /     \            \                  \

 0,0         2,0  4,0        6,0          4,0                4,0
```

#### Bounded Insertion

The biggest problem with bounded insertion is that the code to correctly
update balance factors after a rotation is somewhat intricate. All in
all it's not difficult, but you have to be careful not to break the
range or give the wrong balance to nodes. Fortunately, it all falls into
a few simple cases and the code to implement those cases is relatively
short and easy to follow. There are four cases for insertion into an AVL
tree, one case requiring a single rotation and three cases requiring a
double rotation. Put simply, if a single rotation is needed, the tree
becomes more balanced and both the old parent and new parent are given a
value of 0. The three cases for deletion depend on the initial balance
of the lowest node that will become the new parent. The following
diagram shows these four cases (note that I'm using the right path for
this example, so the direction index would be 1. Also notice that an
asterisk stands for balance factors that don't matter during
intermediate rotations):

    Single case:
    
    
         x,+2                         y,0
    
       /      \                     /     \
    
     A          y,+1     ->     x,0         C
    
              /      \        /     \
    
            B          C    A         B
    
    
    Double case 1 (z is balanced):
    
    
         x,+2                   x,*                               z,0
    
       /      \               /     \                          /       \
    
     A          y,-1        A         z,*                  x,0           y,0
    
              /      \   ->         /     \         ->   /     \       /     \
    
          z,0          D          B         y,*        A         B   C         D
    
        /     \                           /     \
    
      B         C                       C         D
    
    
    Double case 2 (z's right subtree is taller):
    
    
         x,+2                   x,*                               z,0
    
       /      \               /     \                          /       \
    
     A          y,-1        A         z,*                  x,-1          y,0
    
              /      \   ->         /     \         ->   /      \      /     \
    
          z,+1         D          B         y,*        A          B  C         D
    
        /      \                          /     \
    
      B          C                      C         D
    
    
    Double case 3 (z's left subtree is taller):
    
    
         x,+2                   x,*                               z,0
    
       /      \               /     \                          /       \
    
     A          y,-1        A         z,*                  x,0          y,+1
    
              /      \   ->         /     \         ->   /     \      /      \
    
          z,-1         D          B         y,*        A         B  C          D
    
        /      \                          /     \
    
      B          C                      C         D

The code to implement this is simple, and can be modularized into a
function. The best part is that the function can be reused for deletion
as well because deletion has three similar cases for double rotations.
There is a trick to this code, by the way. Instead of using -1 and +1
directly, I use the value of the direction index to determine which
would be best suited for the current direction, thus removing an
unnecessary symmetric case. This balance information varies with
insertion and deletion, just like the direction index, so they are both
arguments to the function. Follow the code closely and walk through each
of the cases above to see how the diagrams translate into code:

    void jsw_adjust_balance(struct jsw_node *root, int dir, int bal)
    {
        struct jsw_node *n = root->link[dir];
        struct jsw_node *nn = n->link[!dir];
    
        if (nn->balance == 0)
        {
            root->balance = n->balance = 0;
        }
        else if (nn->balance == bal)
        {
            root->balance = -bal;
            n->balance = 0;
        }
        else /* nn->balance == -bal */
        {
            root->balance = 0;
            n->balance = bal;
        }
    
        nn->balance = 0;
    }

Of course, just knowing how to change the balance factors for a double
rotation isn't enough. Another helper function that handles rebalancing
for insertion must be used that actually performs the rotations and also
fixes the balance factors for a single rotation. Remember that there are
only two rotation cases, and **jsw\_adjust\_balance** fixes the balance
factors for the double rotation case. So the code for
**jsw\_insert\_balance** is short and sweet. This is the function that
determines the value of **bal** and calls the rotation functions as
necessary. Once again, follow through this code and see how it compares
to each diagram above:

    struct jsw_node *jsw_insert_balance(struct jsw_node *root, int dir)
    {
        struct jsw_node *n = root->link[dir];
        int bal = dir == 0 ? -1 : +1;
    
        if (n->balance == bal)
        {
            root->balance = n->balance = 0;
            root = jsw_single(root, !dir);
        }
        else /* n->balance == -bal */
        {
            jsw_adjust_balance(root, dir, bal);
            root = jsw_double(root, !dir);
        }
    
        return root;
    }

These two functions are all that's needed to rebalance a violation of
the AVL invariant during insertion. Now all we need to do is come up
with the framework to insert a new node and update the balance factors.
The good news is that from the parent of the newly inserted node,
updating a balance factor is a simple matter of incremending the balance
factor if we went down the right subtree or decremending the balance
factor if we went down the left subtree. Beyond that it's just a basic
insertion. However, during AVL insertion, only one call of
**jsw\_insert\_balance** will ever be needed, and changing the balance
factors above that part of the tree will break the data structure. So we
need a way to stop updating after a rebalance has been performed. In an
iterative version, this is a simple matter of breaking from a loop or
returning from a function, but in the following recursive version a
status flag is needed:

    struct jsw_node *jsw_insert_r(struct jsw_node *root, int data, int *done)
    {
        if (root == NULL)
        {
            root = make_node(data);
        }
        else
        {
            int dir = root->data < data;
    
            root->link[dir] = jsw_insert_r(root->link[dir], data, done);
    
            if (!*done)
            {
                /* Update balance factors */
                root->balance += dir == 0 ? -1 : +1;
    
                /* Rebalance as necessary and terminate */
                if (root->balance == 0)
                {
                    *done = 1;
                }
                else if (abs(root->balance) > 1)
                {
                    root = jsw_insert_balance(root, dir);
                    *done = 1;
                }
            }
        }
    
        return root;
    }
    
    int jsw_insert(struct jsw_tree *tree, int data)
    {
        int done = 0;
    
        tree->root = jsw_insert_r(tree->root, data, &done);
    
        return 1;
    }

There are only three cases for updating balance factors, since there are
only three values in the range. If the balance factor was 0, then it is
incremented or decremented and the updating continues up the tree. If
the balance factor was -1 or +1 and becomes 0, the tree has become more
balanced and no more updating or rebalancing is required. If the balance
factor was -1 or +1 and becomes -2 or +2, rebalancing is required and
**jsw\_insert\_balance** is called. After **jsw\_insert\_balance**, the
tree is once again balanced and no more updating is performed. The code
is simple, but I still recommend drawing out several tree insertions
while following the algorithm closely to understand what is going on, or
at least to get bragging rights for the bug report if my algorithm is
wrong. ;-) Let's walk through a degenerate case to see that the AVL tree
results in a well balanced tree. The degenerate case is an ascending
sequence of integers, 0 through 9. Keep in mind that newly inserted
nodes have subtrees of equal height (ie. two empty subtrees), so the
balance factor of a node returned by **make\_node** is 0 to show that it
is balanced:

    Insert 0:
    
       0,0
    
    Insert 1:
    
       0,1
    
           \
    
             1,0
    
    Insert 2:
    
           1,0
    
         /     \
    
     0,0         2,0
    
    Insert 3:
    
           1,1
    
         /     \
    
     0,0         2,1
    
                     \
    
                       3,0
    
    Insert 4:
    
           1,1
    
         /     \
    
     0,0         3,0
    
               /     \
    
           2,0         4,0
    
    Insert 5:
    
                 3,0
    
               /     \
    
           1,0         4,1
    
         /     \           \
    
     0,0         2,0         5,0
    
    Insert 6:
    
                     3,0
    
               /            \
    
           1,0                5,0
    
         /     \            /     \
    
     0,0         2,0    4,0         6,0
    
    Insert 7:
    
                     3,1
    
               /            \
    
           1,0                5,1
    
         /     \            /     \
    
     0,0         2,0    4,0         6,1
    
                                        \
    
                                          7,0
    
    Insert 8:
    
                     3,1
    
               /            \
    
           1,0                5,1
    
         /     \            /     \
    
     0,0         2,0    4,0         7,0
    
                                  /     \
    
                              6,0         8,0
    
    Insert 9:
    
                       3,1
    
               /                \
    
           1,0                    7,0
    
         /     \                /     \
    
     0,0         2,0        5,0         8,1
    
                          /     \           \
    
                      4,0         6,0         9,0

Moving onward and upward, a lot of people flinch at recursive
implementations because recursion has a bad reputation of being slow and
limited by an unknown “stack” size. As such, it makes sense to consider
the non-recursive implementation of an AVL tree. There are two, the
first of which is a direct conversion of the recursive algorithm using a
stack and bottom-up modifications. The code is longer, but the logic is
identical once you get past the apparent complexity of using two stacks
to save the traversal path. If you find it hard to understand, use the
recursive code as an example and compare the similar parts, then you can
more easily figure out how the rest of the code does what it does. If
you've gone through Binary Search Trees I, you should have no trouble
with the logic:

    int jsw_insert(struct jsw_tree *tree, int data)
    {
        /* Empty tree case */
        if (tree->root == NULL)
        {
            tree->root = make_node(data);
    
            if (tree->root == NULL)
            {
                return 0;
            }
        }
        else
        {
            struct jsw_node *it, *up[50];
            int upd[50], top = 0;
    
            it = tree->root;
    
            /* Search for an empty link, save the path */
            for (;;)
            {
                /* Push direction and node onto stack */
                upd[top] = it->data < data;
                up[top++] = it;
    
                if (it->link[upd[top - 1]] == NULL)
                {
                    break;
                }
    
                it = it->link[upd[top - 1]];
            }
    
            /* Insert a new node at the bottom of the tree */
            it->link[upd[top - 1]] = make_node(data);
    
            if (it->link[upd[top - 1]] == NULL)
            {
                return 0;
            }
    
            /* Walk back up the search path */
            while (--top >= 0)
            {
                /* Update balance factors */
                up[top]->balance += upd[top] == 0 ? -1 : +1;
    
                /* Terminate or rebalance as necessary */
                if (up[top]->balance == 0)
                {
                    break;
                }
                else if (abs(up[top]->balance) > 1)
                {
                    up[top] = jsw_insert_balance(up[top], upd[top]);
    
                    /* Fix the parent */
                    if (top != 0)
                    {
                        up[top - 1]->link[upd[top - 1]] = up[top];
                    }
                    else
                    {
                        tree->root = up[0];
                    }
    
                    break;
                }
            }
        }
    
        return 1;
    }

Instead of an implicit stack through recursion, this non-recursive
version of AVL insertion caches only the required information using an
explicit stack in the form of two arrays that hold the nodes along a
search path and the directions taken at each node. Then the same
rebalancing logic is used as the top of each stack is popped off in an
upward traversal back to the root. Because a simple break can be used
when one of the stopping cases is encountered, a status flag is no
longer needed.

#### Bounded Deletion

Deletion from an AVL tree is only marginally more complicated than
insertion. This is shocking to most people because deletion is always
left as an exercise to the reader, but not before scaring the wits out
of the reader by talking about how long and complicated it is and that
the algorithms are beyond the scope of a textbook or paper devoted to
the topic. In reality, there is only one extra case for rebalancing in
AVL deletion, and that is a simple single rotation case. The extra
complexity comes from the task of removing a node from the tree rather
than any rebalancing effort. The five cases are as follows:

    Single case 1 (y is not balanced):
    
    
         x,+1                         y,0
    
       /      \                     /     \
    
     A          y,+1     ->     x,0         C
    
              /      \        /     \
    
            B          C    A         B
    
    
    Single case 2 (y is balanced):
    
    
         x,+1                         y,-1
    
       /      \                     /      \
    
     A          y,0     ->     x,+1          C
    
              /     \        /      \
    
            B         C    A          B
    
    
    Double case 1 (z is balanced):
    
    
         x,+1                   x,*                               z,0
    
       /      \               /     \                          /       \
    
     A          y,-1        A         z,*                  x,0           y,0
    
              /      \   ->         /     \         ->   /     \       /     \
    
          z,0          D          B         y,*        A         B   C         D
    
        /     \                           /     \
    
      B         C                       C         D
    
    
    Double case 2 (z's right subtree is taller):
    
    
         x,+1                   x,*                                z,0
    
       /      \               /     \                           /       \
    
     A          y,-1        A         z,*                  x,-1           y,0
    
              /      \   ->         /     \         ->   /      \       /     \
    
          z,+1         D          B         y,*        A          B   C         D
    
        /      \                          /     \
    
      B          C                      C         D
    
    
    Double case 2 (z's left subtree is taller):
    
    
         x,+1                   x,*                               z,0
    
       /      \               /     \                          /       \
    
     A          y,-1        A         z,*                  x,0           y,+1
    
              /      \   ->         /     \         ->   /     \       /      \
    
          z,-1         D          B         y,*        A         B   C          D
    
        /      \                          /     \
    
      B          C                      C         D

Amazingly enough, all but one of those cases are solved problems from
insertion. So the code to add the extra case and rebalance after a
deletion is trivial:

    struct jsw_node *jsw_remove_balance(struct jsw_node *root, int dir, int *done)
    {
        struct jsw_node *n = root->link[!dir];
        int bal = dir == 0 ? -1 : +1;
    
        if (n->balance == -bal)
        {
            root->balance = n->balance = 0;
            root = jsw_single(root, dir);
        }
        else if (n->balance == bal)
        {
            jsw_adjust_balance(root, !dir, -bal);
            root = jsw_double(root, dir);
        }
        else /* n->balance == 0 */
        {
            root->balance = -bal;
            n->balance = bal;
            root = jsw_single(root, dir);
            *done = 1;
        }
    
        return root;
    }

Wait, why does the new case set the status flag to show that rebalancing
is finished? Unlike insertion, an AVL deletion could potentially require
rebalancing all of the way back up the tree, so there are fewer cases
where the updating terminates early and this is one of them. In the new
case, the the height of the tree doesn't change, so there is no need to
continue rebalancing up the tree. An important key to how deletion works
is to remember that instead of rebalancing the subtree that was
increased after an insertion, we need to rebalance the opposite subtree
that the node is deleted from. So look very closely at the values of
**dir** and **bal** when **jsw\_remove\_balance** is called from the
following AVL deletion algorithm:

    struct jsw_node *jsw_remove_r(struct jsw_node *root, int data, int *done)
    {
        if (root != NULL)
        {
            int dir;
    
            /* Remove node */
            if (root->data == data)
            {
                /* Unlink and fix parent */
                if (root->link[0] == NULL || root->link[1] == NULL)
                {
                    struct jsw_node *save;
    
                    dir = root->link[0] == NULL;
                    save = root->link[dir];
                    free(root);
    
                    return save;
                }
                else
                {
                    /* Find inorder predecessor */
                    struct jsw_node *heir = root->link[0];
    
                    while (heir->link[1] != NULL)
                    {
                        heir = heir->link[1];
                    }
    
                    /* Copy and set new search data */
                    root->data = heir->data;
                    data = heir->data;
                }
            }
    
            dir = root->data < data;
            root->link[dir] = jsw_remove_r(root->link[dir], data, done);
    
            if (!*done)
            {
                /* Update balance factors */
                root->balance += dir != 0 ? -1 : +1;
    
                /* Terminate or rebalance as necessary */
                if (abs(root->balance) == 1)
                {
                    *done = 1;
                }
                else if (abs(root->balance) > 1)
                {
                    root = jsw_remove_balance(root, dir, done);
                }
            }
        }
    
        return root;
    }
    
    int jsw_remove(struct jsw_tree *tree, int data)
    {
        int done = 0;
    
        tree->root = jsw_remove_r(tree->root, data, &done);
    
        return 1;
    }

The code to rebalance is actually shorter than insertion because of
fewer cases where the updating would terminate early. What makes this
code longer is the extra work of deleting a node. The idea behind the
recursive deletion is to search for the node to delete. Then once this
node is found, if it has only one child or no children, simply replace
the node with its child. If the node has two children, the inorder
predecessor is found and its data is copied into the node to be deleted.
This is where the tricky part comes in. Instead of stopping the
recursion and bending over backward to remove the inorder predecessor,
the recursion continues by replacing the search key with with the value
of the inorder predecessor. This is somewhat confusing because the
search key changes in the middle of the recursive path. But this
tutorial is about AVL trees, not the variations of binary search tree
deletion, so I highly recommend that you work through this function to
get a feel for how it works.

Because the recursive code is potentially confusing, we'll look at a
non-recursive version. But first, let's walk through the deletion of
items from an existing AVL tree. This execution will continue what was
started with insertion by tracing through the deletion of a degenerate
case, 0 through 7:

    Remove 0:
    
                 3,1
    
         /                \
    
     1,1                    7,0
    
         \                /     \
    
           2,0        5,0         8,1
    
                    /     \           \
    
                4,0         6,0         9,0
    
    Remove 1:
    
                 7,-1
    
               /      \
    
           3,1          8,1
    
         /     \            \
    
     2,0         5,0          9,0
    
               /     \
    
           4,0         6,0
    
    Remove 2:
    
                  7,-1
    
                /      \
    
           5,-1          8,1
    
         /      \            \
    
     3,1          6,0          9,0
    
         \
    
           4,0
    
    Remove 3:
    
                 7,0
    
               /     \
    
           5,0         8,1
    
         /     \           \
    
     4,0         6,0         9,0
    
    Remove 4:
    
           7,0
    
         /     \
    
     5,1         8,1
    
         \           \
    
           6,0         9,0
    
    Remove 5:
    
           7,1
    
         /     \
    
     6,0         8,1
    
                     \
    
                       9,0
    
    Remove 6:
    
           8,0
    
         /     \
    
     7,0         9,0
    
    Remove 7:
    
     8,1
    
         \
    
           9,0

The non-recursive code to remove from an AVL tree is identical to the
recursive code in logic, and equivalent to the solution used for
non-recursive insertion through the use of two stacks to save the search
path. Rather than any sneaky tricks for handling the case where the node
to be deleted has two children, a direct approach from Binary Search
Trees I will be used:

    int jsw_remove(struct jsw_tree *tree, int data)
    {
        if (tree->root != NULL)
        {
            struct jsw_node *it, *up[32];
            int upd[32], top = 0;
            int done = 0;
    
            it = tree->root;
    
            for (;;)
            {
                /* Terminate if not found */
                if (it == NULL)
                {
                    return 0;
                }
                else if (it->data == data)
                {
                    break;
                }
    
                /* Push direction and node onto stack */
                upd[top] = it->data < data;
                up[top++] = it;
    
                it = it->link[upd[top - 1]];
            }
    
            /* Remove the node */
            if (it->link[0] == NULL || it->link[1] == NULL)
            {
                /* Which child is not null? */
                int dir = it->link[0] == NULL;
    
                /* Fix parent */
                if (top != 0)
                {
                    up[top - 1]->link[upd[top - 1]] = it->link[dir];
                }
                else
                {
                    tree->root = it->link[dir];
                }
    
                free(it);
            }
            else
            {
                /* Find the inorder successor */
                struct jsw_node *heir = it->link[1];
    
                /* Save the path */
                upd[top] = 1;
                up[top++] = it;
    
                while (heir->link[0] != NULL)
                {
                    upd[top] = 0;
                    up[top++] = heir;
                    heir = heir->link[0];
                }
    
                /* Swap data */
                it->data = heir->data;
                /* Unlink successor and fix parent */
                up[top - 1]->link[up[top - 1] == it] = heir->link[1];
    
                free(heir);
            }
    
            /* Walk back up the search path */
            while (--top >= 0 && !done)
            {
                /* Update balance factors */
                up[top]->balance += upd[top] != 0 ? -1 : +1;
    
                /* Terminate or rebalance as necessary */
                if (abs(up[top]->balance) == 1)
                {
                    break;
                }
                else if (abs(up[top]->balance) > 1)
                {
                    up[top] = jsw_remove_balance(up[top], upd[top], &done);
    
                    /* Fix parent */
                    if (top != 0)
                    {
                        up[top - 1]->link[upd[top - 1]] = up[top];
                    }
                    else
                    {
                        tree->root = up[0];
                    }
                }
            }
        }
    
        return 1;
    }

#### Top-Down Insertion

The non-recursive bounded insertion algorithm is based on the ideas
behind non-recursive deletion and simulates recursion through the use of
two stacks. However, this is not the best way to write a non-recursive
AVL insertion because it uses an unnecessary amount of extra space (for
the stacks) and time (for walking back up the search path). Let's
consider some of the absolutes that were discussed for bounded AVL
insertion. First, **jsw\_insert\_balance** is only called once for any
possible insertion. Second, and this wasn't mentioned explicitly, a
rebalance operation will only be performed at the last node on the
search path with a non-zero balance factor. If you don't believe me,
follow the execution of insertion above and watch where a rotation is
made.

We can take advantage of these two facts by avoiding the use of a stack
in favor of a few extra variables. Because only one rebalance operation
is ever needed for insertion into an AVL tree, there's no need to walk
back up the path. Through careful use of helper variables, we can
search, update balance factors, and rebalance if necessary all in one
straight search down the tree. The trick is to save the last node along
the path with a non-zero balance factor and its parent. Consider the
following tree prior to the insertion of 9:

``` 
                 3,1

           /            \

       1,0                5,1

     /     \            /     \

 0,0         2,0    4,0         7,0

                              /     \

                          6,0         8,0
```

We know that if a rebalance is needed, it will be performed at 5,
because 5 is the last node along the search path with a non-zero balance
factor. To avoid walking back up the tree or having a stack of any kind,
we need to save both 5 and its parent so that when the root changes from
5 to 7, 3's right link is updated to point to the correct subtree. We
will refer to these varibles as s and t. s will be the node where a
rebalance is necessary and t will be its parent:

``` 
                 3,1  <--------------------- t

           /            \

       1,0                5,1  <------------ s

     /     \            /     \

 0,0         2,0    4,0         7,0

                              /     \

                          6,0         8,0
```

Of course, we also need to add a new node at the bottom of the tree, so
we need a variable, which we will call q, to refer to the newly inserted
node as well as an iterator variable, called p, that will walk down the
tree from s to q and update balance factors. For example, when 9 is
inserted, 5, 7, and 8 will need to be updated to reflect the right
subtree's growth by one. This will force a rebalance by causing 5's
balance factor to grow to +2:

``` 
                 3,1  <----------------------------- t

           /            \

       1,0                5,2  <-------------------- s

     /     \            /     \

 0,0         2,0    4,0         7,1

                              /     \

                          6,0         8,1

                                          \

                                            9,0  <-- q
```

At this point **jsw\_insert\_balance** can be called on s, the result of
that operation can be assigned to t's right link, and the tree becomes
balanced without recursion or stacks (or even parent pointers\!) to walk
back up the tree. The code to do this is straightforward if you know
what it's doing, but otherwise it can be somewhat tricky through the
need to carefully update the root of the tree as well as save both s and
t for every case:

    int jsw_insert(struct jsw_tree *tree, int data)
    {
        /* Empty tree case */
        if (tree->root == NULL)
        {
            tree->root = make_node(data);
    
            if (tree->root == NULL)
            {
                return 0;
            }
        }
        else
        {
            struct jsw_node head = { 0 }; /* False tree root */
    
            struct jsw_node *s, *t;     /* Place to rebalance and parent */
            struct jsw_node *p, *q;     /* Iterator and save pointer */
            int dir;
    
            /* Set up false root to ease maintenance */
            t = &head;
            t->link[1] = tree->root;
    
            /* Search down the tree, saving rebalance points */
            for (s = p = t->link[1];; p = q)
            {
                dir = p->data < data;
                q = p->link[dir];
    
                if (q == NULL)
                {
                    break;
                }
    
                if (q->balance != 0)
                {
                    t = p;
                    s = q;
                }
            }
    
            /* Insert the new node */
            p->link[dir] = q = make_node(data);
    
            if (q == NULL)
            {
                return 0;
            }
    
            /* Update balance factors */
            for (p = s; p != q; p = p->link[dir])
            {
                dir = p->data < data;
                p->balance += dir == 0 ? -1 : +1;
            }
    
            q = s; /* Save rebalance point for parent fix */
    
            /* Rebalance if necessary */
            if (abs(s->balance) > 1)
            {
                dir = s->data < data;
                s = jsw_insert_balance(s, dir);
            }
    
            /* Fix parent */
            if (q == head.link[1])
            {
                tree->root = s;
            }
            else
            {
                t->link[q == t->link[1]] = s;
            }
        }
    
        return 1;
    }

This function uses a special dummy root to avoid special cases with the
root of the tree except if rebalancing affects the root itself, in which
case it will be updated at the end of the algorithm before the function
returns. This dummy root allows us to easily set both t and s without
any special cases and update them in the search loop. Notice that t and
s are only updated if the search has not ended by finding a null link
and the next node in the search path has a non-zero balance factor.

The updating of balance factors is a little confusing because the
iterator variable is reused to walk from the rebalance point down to
(but not including\!) the newly inserted node and update balance factors
in the usual way. Once the balance factors are updated, the rebalance
point is saved because if any rotations are made, we won't have an easy
way of updating the true root since s is the argument to
**jsw\_insert\_balance**. If any rotations were made, we fix the right
or left link of t, or replace the true root if the rotation was made at
the root.

This algorithm is far more efficient (and elegant\!) than the recursive
or two-stacks methods given previously, but I would be a terrible liar
if I said that it was my invention. In fact, this very algorithm,
including the variable names, comes from Donald Knuth's “The Art of
Computer Programming”, volume 3. Because it was the most efficient
algorithm for AVL insertion that I could find or invent (no surprise), I
felt compelled to describe it here for your benefit.

#### Unbounded Insertion

Unbounded insertion into an AVL tree is theoretically simpler than
bounded insertion because instead of using the difference in height
between two subtrees as a balance factor, the actual height of the
subtree itself is used. In this way, as long as you know which balance
factors to replace during a rotation, the code to maintain an AVL tree's
balance factors is relatively easy to follow. Our version of unbounded
insertion will use a different implementation of **jsw\_single** and
**jsw\_double** for variety. Instead of just performing the rotations,
**jsw\_single** will also handle the updating of balance factors, then
**jsw\_double** will call **jsw\_single** twice:

    #define height(p) ((p) == NULL ? -1 : (p)->balance)
    #define jsw_max(a,b) ((a) > (b) ? (a) : (b))
    
    struct jsw_node *jsw_single(struct jsw_node *root, int dir)
    {
        struct jsw_node *save = root->link[!dir];
        int rlh, rrh, slh;
    
        /* Rotate */
        root->link[!dir] = save->link[dir];
        save->link[dir] = root;
    
        /* Update balance factors */
        rlh = height(root->link[0]);
        rrh = height(root->link[1]);
        slh = height(save->link[!dir]);
    
        root->balance = jsw_max(rlh, rrh) + 1;
        save->balance = jsw_max(slh, root->balance) + 1;
    
        return save;
    }
    
    struct jsw_node *jsw_double(struct jsw_node *root, int dir)
    {
        root->link[!dir] = jsw_single(root->link[!dir], !dir);
    
        return jsw_single(root, dir);
    }

This is conceptually more complicated than the rotation functions used
for bounded AVL trees, but that is due to the extra temporary variables
used to avoid long expressions. Notice that a null pointer has a height
of -1 and nodes with no children have a height of 0. Before discussing
the method behind how the balance factors are updated, we will walk
through a simple example for single rotation. The following tree is an
AVL tree with the new insertion of 8 violating the AVL invariant:

``` 
                 3,3 <----------------- No yet reached

           /            \

       1,1                5,1 <-------- Imbalance here

     /     \            /     \

 0,0         2,0    4,0         6,2 <-- Source of imbalance

                                    \

                                      7,1

                                          \

                                            8,0
```

As you know from the previous discussions, a rotation will be made at 6,
but this time the way to tell is by subtracting the balance factor of
one subtree from the balance factor of another. If the absolute value of
that subtraction is greater than or equal to 2, an imbalance has
occurred and must be remedied. Notice how this is a direct translation
of the AVL invariant, where the difference in height between two
subtrees cannot be larger than 1. Let's now perform a single left
rotation around 6 and think about how to update the balance factors:

``` 
                 3,3

           /            \

       1,1                5,1

     /     \            /     \

 0,0         2,0    4,0         7,1

                              /     \

                          6,2        8,0
```

The balance factor for 8 is still correct, but 6 is now way off, and 7
is accurate. To fix 6 we take the largest of its subtrees and add 1,
which is 0 because 6 only has null links and a null link has a height of
-1. Then, even though 7 has an accurate height, we still make sure by
doing the same thing. The largest height in 7's subtrees is 0, adding 1
gives us 1 and the tree is correct. Then move up the tree and continue
to do perform the same operation all of the way back up to the root. The
resulting tree is:

``` 
                 3,3

           /            \

       1,1                5,2

     /     \            /     \

 0,0         2,0    4,0         7,1

                              /     \

                          6,0        8,0
```

This implies a very straightforward implementation for insertion. Simply
use a basic binary search tree recursive insertion, compare the
difference in heights to look for a rebalance, rebalance as necessary,
and then add 1 to the largest of the subtrees to update the balance
factors. A status flag can be used to avoid unnecessary work because we
already know that only one single or double rotation is necessary to
bring an AVL tree into balance, and the balance factors changed are
localized enough so that no further changes need to be made further up
the search path. The only thing missing from this function is the actual
code to perform a rebalance:

    struct jsw_node *jsw_insert_r(struct jsw_node *root, int data, int *done)
    {
        if (root == NULL)
        {
            root = make_node(data);
        }
        else
        {
            int dir = root->data < data;
            int lh, rh, max;
    
            root->link[dir] = jsw_insert_r(root->link[dir], data, done);
    
            if (!*done)
            {
                /* Rebalance if necessary */
                lh = height(root->link[dir]);
                rh = height(root->link[!dir]);
    
                if (lh - rh >= 2)
                {
                    /* Rebalancing magic */
                    *done = 1;
                }
    
                /* Update balance factors */
                lh = height(root->link[dir]);
                rh = height(root->link[!dir]);
                max = jsw_max(lh, rh);
    
                root->balance = max + 1;
            }
        }
    
        return root;
    }
    
    int jsw_insert(struct jsw_tree *tree, int data)
    {
        int done = 0;
    
        tree->root = jsw_insert_r(tree->root, data, &done);
    
        return 1;
    }

The only trick now is to figure out how to rebalance a violation of the
AVL invariant (I cleverly hid it with a voodoo cloaking comment).
Fortunately, you already know how to do that. There are no longer four
cases because the rotations handle updating balance factors that always
works the way we want it to. So the four cases become only two cases,
those of single and double rotation. This can be generalized into a
height comparison using the direction index. If the **dir** subtree is
taller then a single rotation will suffice, otherwise the **\!dir**
subtree is taller and a double rotation is needed. Remember the
operations again and think about why this test would work (hint: if the
subtrees are of equal length then there cannot be a violation):

    dir == 1:
    
     3                   4
    
       \               /   \
    
         4      -->  3       5
    
           \
    
             5
    
    
     3           3                   4
    
       \           \               /   \
    
         5  -->      4      -->  3       5
    
       /               \
    
     4                   5

This realization paves the way for a simple solution. Just find the
balace factors of the root's two subtrees. If the **dir** subtree is
taller, perform a single rotation, otherwise perform a double rotation.
The code to do this is short (even though it too is cleaner with
temporary variables) and we can plug it into the framework given above
with no trouble at all (mostly because the framework was devised by
cutting this part out ;-D):

    struct jsw_node *jsw_insert_r(struct jsw_node *root, int data, int *done)
    {
        if (root == NULL)
        {
            root = make_node(data);
        }
        else
        {
            int dir = root->data < data;
            int lh, rh, max;
    
            root->link[dir] = jsw_insert_r(root->link[dir], data, done);
    
            if (!*done)
            {
                /* Rebalance if necessary */
                lh = height(root->link[dir]);
                rh = height(root->link[!dir]);
    
                if (lh - rh >= 2)
                {
                    struct jsw_node *a = root->link[dir]->link[dir];
                    struct jsw_node *b = root->link[dir]->link[!dir];
    
                    if (height(a) >= height(b))
                    {
                        root = jsw_single(root, !dir);
                    }
                    else
                    {
                        root = jsw_double(root, !dir);
                    }
    
                    *done = 1;
                }
    
                /* Update balance factors */
                lh = height(root->link[dir]);
                rh = height(root->link[!dir]);
                max = jsw_max(lh, rh);
    
                root->balance = max + 1;
            }
        }
    
        return root;
    }
    
    int jsw_insert(struct jsw_tree *tree, int data)
    {
        int done = 0;
    
        tree->root = jsw_insert_r(tree->root, data, &done);
    
        return 1;
    }

Unbounded insertion without recursion is the basic framework for bounded
insertion with unbounded rebalancing code spliced in. By now there
should be no problems with understanding it. Don't let the length scare
you, there's nothing new here. All of the pieces are simple and fit
together in a straightforward manner. Notice how a status flag is needed
for this insertion where it wasn't for the bounded iterative insertion.
The reason is that the balance factor update step is after the rebalance
step and still needs to be performed even if a stopping case or a
rebalance occurs. So we can't simply break from the loop this time:

    int jsw_insert(struct jsw_tree *tree, int data)
    {
        if (tree->root == NULL)
        {
            tree->root = make_node(data);
    
            if (tree->root == NULL)
            {
                return 0;
            }
        }
        else
        {
            struct jsw_node *it, *up[32];
            int upd[32], top = 0;
            int done = 0;
    
            it = tree->root;
    
            /* Search for an empty link, save the path */
            for (;;)
            {
                /* Push direction and node onto stack */
                upd[top] = it->data < data;
                up[top++] = it;
    
                if (it->link[upd[top - 1]] == NULL)
                {
                    break;
                }
    
                it = it->link[upd[top - 1]];
            }
    
            /* Insert a new node at the bottom of the tree */
            it->link[upd[top - 1]] = make_node(data);
    
            if (it->link[upd[top - 1]] == NULL)
            {
                return 0;
            }
    
            /* Walk back up the search path */
            while (--top >= 0 && !done)
            {
                int dir = up[top]->data < data;
                int lh, rh, max;
    
                lh = height(up[top]->link[upd[top]]);
                rh = height(up[top]->link[!upd[top]]);
    
                /* Terminate or rebalance as necessary */
                if (lh - rh == 0)
                {
                    done = 1;
                }
    
                if (lh - rh >= 2)
                {
                    struct jsw_node *a = up[top]->link[upd[top]]->link[upd[top]];
                    struct jsw_node *b = up[top]->link[upd[top]]->link[!upd[top]];
    
                    if (height(a) >= height(b))
                    {
                        up[top] = jsw_single(up[top], !upd[top]);
                    }
                    else
                    {
                        up[top] = jsw_double(up[top], !upd[top]);
                    }
    
                    /* Fix parent */
                    if (top != 0)
                    {
                        up[top - 1]->link[upd[top - 1]] = up[top];
                    }
                    else
                    {
                        tree->root = up[0];
                    }
    
                    done = 1;
                }
    
                /* Update balance factors */
                lh = height(up[top]->link[upd[top]]);
                rh = height(up[top]->link[!upd[top]]);
                max = jsw_max(lh, rh);
    
                up[top]->balance = max + 1;
            }
        }
    
        return 1;
    }

That's unbounded insertion. In theory it's simpler than bounded
insertion because there are fewer cases to consider, but the code tends
to be more verbose with the need to cleanly calculate heights and
differences. You'll find this to be the trend with deletion as well,
which we will look at next.

#### Unbounded Deletion

The interesting part about unbounded deletion is that we can continue to
use the existing rotation algorithms and balance factor updates (the
tallest height incremented by 1) that were used for insertion. Only one
conceptual difference exists. Because a subtree may shrink instead of
grow, rebalancing must be performed on the opposite subtree as the
deletion was made in, just like with bounded deletion. This means that
we will be using the inverse tests for height differences.
Alternatively, we could use absolute values for both insertion and
deletion. The cases for rebalancing are identical to bounded deletion:
if the new difference between subtrees is -1, the algorithm terminates.
If the new difference is -2 or less, rebalancing is performed. Otherwise
the updating of balance factors propagates up the search path. The rules
remain the same, only the approach changes:

    struct jsw_node *jsw_remove_r(struct jsw_node *root, int data, int *done)
    {
        if (root != NULL)
        {
            int dir;
    
            /* Remove node */
            if (root->data == data)
            {
                /* Unlink and fix parent */
                if (root->link[0] == NULL || root->link[1] == NULL)
                {
                    struct jsw_node *save;
    
                    dir = root->link[0] == NULL;
                    save = root->link[dir];
                    free(root);
    
                    return save;
                }
                else
                {
                    /* Find inorder predecessor */
                    struct jsw_node *heir = root->link[0];
    
                    while (heir->link[1] != NULL)
                    {
                        heir = heir->link[1];
                    }
    
                    /* Copy and set new search data */
                    root->data = heir->data;
                    data = heir->data;
                }
            }
    
            dir = root->data < data;
            root->link[dir] = jsw_remove_r(root->link[dir], data, done);
    
            if (!*done)
            {
                /* Update balance factors */
                int lh = height(root->link[dir]);
                int rh = height(root->link[!dir]);
                int max = jsw_max(lh, rh);
    
                root->balance = max + 1;
    
                /* Terminate or rebalance as necessary */
                if (lh - rh == -1)
                {
                    *done = 1;
                }
    
                if (lh - rh <= -2)
                {
                    struct jsw_node *a = root->link[!dir]->link[dir];
                    struct jsw_node *b = root->link[!dir]->link[!dir];
    
                    if (height(a) <= height(b))
                    {
                        root = jsw_single(root, dir);
                    }
                    else
                    {
                        root = jsw_double(root, dir);
                    }
                }
            }
        }
    
        return root;
    }
    
    int jsw_remove(struct jsw_tree *tree, int data)
    {
        int done = 0;
    
        tree->root = jsw_remove_r(tree->root, data, &done);
    
        return 1;
    }

Let's look through a quick example to see how deletion works with a
double rotation case. In the following tree, we will delete 0. After the
removal of 0, a double rotation is needed, first at 69, then at 58 to
restore balance completely:

``` 
              58,3

            /      \

       24,1          69,2

     /             /      \

 0,0          62,1          78,0

                   \

                     64,0

Delete 0:

        58,3

      /      \

 24,0          69,2

             /      \

        62,1          78,0

             \

               64,0

Rotate right at 69:

        58,3

      /      \

 24,0          62,2

                    \

                      69,1

                    /      \

               64,0          78,0

Rotate left at 58:

              62,2

            /      \

       58,1          69,1

     /             /      \

24,0          64,0          78,0
```

As always, while a recursive algorithm is temptingly concise, many
people trust a non-recursive algorithm to be more efficient. With
balanced trees this is not always the case because the performance hit
of recursion is minimal at best. But being a closet purist, that
understanding doesn't stop me from preferring non-recursive tree
algorithms. Here is the code for unbounded AVL deletion without using
recursion. As with most of the algorithms in this tuorial, the
non-recursive version is simply a translation of the recursive version
using explicit stacks:

    int jsw_remove(struct jsw_tree *tree, int data)
    {
        if (tree->root != NULL)
        {
            struct jsw_node *it, *up[32];
            int upd[32], top = 0;
    
            it = tree->root;
    
            for (;;)
            {
                /* Terminate if not found */
                if (it == NULL)
                {
                    return 0;
                }
                else if (it->data == data)
                {
                    break;
                }
    
                /* Push direction and node onto stack */
                upd[top] = it->data < data;
                up[top++] = it;
    
                it = it->link[upd[top - 1]];
            }
    
            /* Remove the node */
            if (it->link[0] == NULL || it->link[1] == NULL)
            {
                /* Which child is not null? */
                int dir = it->link[0] == NULL;
    
                /* Fix parent */
                if (top != 0)
                {
                    up[top - 1]->link[upd[top - 1]] = it->link[dir];
                }
                else
                {
                    tree->root = it->link[dir];
                }
    
                free(it);
            }
            else
            {
                /* Find the inorder successor */
                struct jsw_node *heir = it->link[1];
    
                /* Save the path */
                upd[top] = 1;
                up[top++] = it;
    
                while (heir->link[0] != NULL)
                {
                    upd[top] = 0;
                    up[top++] = heir;
                    heir = heir->link[0];
                }
    
                /* Swap data */
                it->data = heir->data;
                /* Unlink successor and fix parent */
                up[top - 1]->link[up[top - 1] == it] = heir->link[1];
    
                free(heir);
            }
    
            /* Walk back up the search path */
            while (--top >= 0)
            {
                int lh = height(up[top]->link[upd[top]]);
                int rh = height(up[top]->link[!upd[top]]);
                int max = jsw_max(lh, rh);
    
                /* Update balance factors */
                up[top]->balance = max + 1;
    
                /* Terminate or rebalance as necessary */
                if (lh - rh == -1)
                {
                    break;
                }
    
                if (lh - rh <= -2)
                {
                    struct jsw_node *a = up[top]->link[!upd[top]]->link[upd[top]];
                    struct jsw_node *b = up[top]->link[!upd[top]]->link[!upd[top]];
    
                    if (height(a) <= height(b))
                    {
                        up[top] = jsw_single(up[top], upd[top]);
                    }
                    else
                    {
                        up[top] = jsw_double(up[top], upd[top]);
                    }
    
                    /* Fix parent */
                    if (top != 0)
                    {
                        up[top - 1]->link[upd[top - 1]] = up[top];
                    }
                    else
                    {
                        tree->root = up[0];
                    }
                }
            }
        }
    
        return 1;
    }

Unlike unbounded insertion, the non-recursive unbounded deletion has no
need for a status flag, so a simple loop break is all that we need to
terminate the algorithm. This makes deletion cleaner than insertion for
once, even though it's still longer. :-)

#### Conclusion

That's it\! You're all done with my AVL tree tutorial, congratulations.
It wasn't that hard now, was it? Well, you're not quite finished. Now
I'm going to give some general information about the performance
properties and a few parting words, then I'll let you go.

AVL trees are about as close to optimal as balanced binary search trees
can get without eating up resources. You can rest assured that the O(log
N) performance of binary search trees is guaranteed with AVL trees, but
the extra bookkeeping required to maintain an AVL tree can be
prohibitive, especially if deletions are common. Insertion into an AVL
tree only requires one single or double rotation, but deletion could
perform up to O(log N) rotations, as in the example of a worst case AVL
(ie. Fibonacci) tree. However, those cases are rare, and still very
fast.

AVL trees are best used when degenerate sequences are common, and there
is little or no locality of reference in nodes. That basically means
that searches are fairly random. If degenerate sequences are not common,
but still possible, and searches are random then a less rigid balanced
tree such as red black trees or Andersson trees are a better solution.
If there is a significant amount of locality to searches, such as a
small cluster of commonly searched items, a splay tree is theoretically
better than all of the balanced trees because of its move-to-front
design.

The sad reality is that an AVL tree is a complicated data structure that
is very fragile in practice. Unfortunately, both of these conspire to
keep it from being quickly considered in the real world, and
implementations will usually only come from students. Hopefully this
tutorial helped you to understand the ideas behind AVL trees as well as
potential implementation methods. In reality it's not terribly difficult
to write a good AVL tree, but it does take a good understanding of how
they work and an attention to detail. The biggest step is in forcing
yourself to believe that they really aren't as complicated as everyone
tries to make you think.

Even if you don't care about implementing AVL trees, this tutorial also
provided enough code for a complete implementation of several variations
that can be plugged into any framework code without needing to
understand in great detail how the algorithms work. Though I don't
recommend using algorithms you don't understand, I can sympathize with
needing a solution now, and forget about having time to understand it,
so I offer my experience, and my code, to people in that awful (and too
common) situation. Once again, all of the code in this tutorial is in
the public domain. You can do whatever you want with it, but I assume no
responsibility for any damages from improper use. ;-)

-----

© 2018 - Eternally Confuzzled
