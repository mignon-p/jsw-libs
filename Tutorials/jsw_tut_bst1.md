Binary Search Trees I
=====================

Trees are among the oldest and most heavily used data structures in computer programming. At the most general, trees are simply undirected graphs called free trees. However, the most common type of tree is a binary search tree, because it allows for efficient sorting and searching of data, which are the two most heavily performed operations in software. Binary search trees are so common that when someone talks about trees, they usually mean a binary search tree. Henceforth, when I say “tree”, I really mean “binary search tree” to save on space and in doing so avoid carpal tunnel. Also, when I say “binary search tree”, I mean “binary search tree”, which could be confusing, but bear with me. :-) This tutorial will discuss, often in painful detail, the basic concept of trees.

I'll do my best to cover the important and practical areas (always providing real, working code) without dipping into theory except where absolutely necessary. I have no desire to add yet another dry textbook description of trees, so I'll focus on what you can use in the real world without having to get your PhD first. If you're a graduate student looking for lemmas and proofs, this tutorial is not for you. I'm neither qualified nor interested in filling page after page with mathematical mumbo jumbo. If you want to know what these “binary thingies” are or have a vague idea but just “don't quite get it”, you've come to the right place! If you know what trees are and have used them in real or test code, this tutorial can help you take your knowledge further by offering variations and alternatives that you may not have considered before.

I'm a programmer, not a computer scientist. I make a living from writing code, not theories. As such, I have a strong opinion about useless theory. Sure, it makes you look smart, but it's not accessible to the average Joe, and in the end most theory really makes no difference in practice. I want my writings to have the widest possible usefulness, so I write for people like me, who don't give a hoot about the theory unless it's important for a decision, but care about writing great code. I also write like a programmer, with puns, off-color jokes, and various social defects included. I like to think of this as adding character to the text, and it's a ton of fun to read too. :-)

A few notes on the code are in order before we start. All of the code is written in C. I chose C because I can fit the meat of the topics into each example without a great deal of framework. C++ or Java seem to be good choices on the surface because they're so hyped to be better than Jolt! cola these days, but to do it properly requires the classes to be written using good style for those languages, and that adds a lot of unnecessary clutter. Since I'm talking about trees and not the details of how to write a good container class, I cut out the fluff by using C instead because the necessary framework is minimal. C is also a good base for the common languages these days. C++ and Java are based off of C, so the syntax is similar enough that a reasonably competent programmer can do a good translation.

I don't dumb down my code. It's my belief that contriving useless examples that everyone can understand is silly and helps nobody. I expect you to know C well enough to follow the examples, but I'll explain anything that isn't immediately obvious. All of the code is written with a production library in mind, driving home that the intended readers are people who want working code that they can actually use! I've seen too many tutorials that give code with syntax errors, logical errors, impractical constructs, or simplifying assumptions that make the code useless everywhere but in that tutorial. Such is not the case here. Not only do I allow you to cut code out of the tutorial and get it to work with minimal effort, I encourage it!

Everything in this tutorial is in the public domain. That means you can quote me without worry, take cool stuff from the code without fear, copy the entire contents of the tutorial and post it somewhere else, and even copy the code verbatim into a library that you intend to sell without asking permission first! Naturally the originals will be the most up-to-date, but I impose no restrictions on use elsewhere. Taking my code and calling it your own is also allowed, but it's bad karma. I trust that you're honest enough to give me credit where credit is due. Tutorials are hard work, so is writing good code.

#### Concept

Okay, we're among friends now, right? You guys are programmers or aspiring programmers, so naturally you've been in a situation where you need to save a lot of data. It might be something like generating a histogram of unique words in a file, or sorting a huge and unknown number of IP addresses gathered from a socket, or even a simple database of email contacts. Well, most likely you ended up using a linked list, or even (gasp!) an array, both of which are totally unsuited for all of the above jobs. What we need is a data structure that keeps everything sorted without bending over backward, is easy to insert and delete efficiently, and is easy to search efficiently. Neither arrays nor linked lists are a good fit, so they're poor solutions.

Well, we know that a binary search is fast because it divides the number of items that need to be searched in half at each step. The only downside is that binary search requires the list to be sorted. Let's try something crazy and actually store the items in the list just like they would be found if we did a binary search for each. That is, the start of the data structure would be the middle item. If we move to the left then we see the middle of the left subset. Basically we're taking a linear list and changing it to an explicit binary search structure:

     [0][1][2][3][4][5][6][7][8][9]


                 becomes


                     5

               /           \

             2               8

           /   \           /   \

         1       4       7       9

       /       /       /

     0       3       6

This is the basic idea behind a binary search tree. It's called a tree because it branches like one (duh!), even though it grows down instead of up like normal trees. Maybe we should be calling them binary search roots instead, since that's easier to link to a common depiction of a tree's roots in your typical high school textbook. Sadly, trees are too well entrenched in computer science, so renaming them would be a bad idea.

If you start a search at the highest node in the tree, called (confusingly!) the root, then you can search for any item in the tree just by testing whether or not that item is less than the root or greater than the root. Say we're looking for 3. 3 is less than 5, so we move to the left and call 2 the root. 3 is greater than 2, so we move to the right and call 4 the root. 3 is less than 4, so we move left and call 3 the root. 3 is equal to 3, so we found what we were looking for. You can follow that pattern to look for any item in the tree.

Okay, but what about an unsuccessful search? What if the item isn't in the tree? It's simple! Just do a search like we did above, and if you walk off the bottom of the tree then the search was unsuccessful. Well, maybe not so simple because how do we know when we walked off the bottom of the tree? There's a special node called a leaf that doesn't contain a valid item and tells us that we went too far in a search. With my convention of using a tilde ('~') for the leaf, the tree would look like this:

                               5

                     /                   \

                   2                        8

               /       \                /       \

             1           4            7           9

           /   \       /   \        /   \       /   \

         0       ~   3       ~    6       ~   ~       ~

       /   \       /   \        /   \

     ~       ~   ~       ~    ~       ~

That's harder to draw with my ASCII art, so I won't use it often, but it's easy to see now that you can stop the search when you reach a leaf. Just for future reference, let's go through some terminology. We know that the top of the tree is called the root, but since trees can be defined recursively, every node in the tree can also be called a root (the root of the subtree). If a node is the root of a subtree then it has a parent that points to it. So 2 is the parent of 1, and 5 is the parent of 2. Looking from the other direction, 2 is a child of 5 and 1 is a child of 2. This all falls back to the family tree connection, so don't be surprised if you hear things like grandparent, great grandparent, and the like when discussing trees.

Nodes that have two children are called internal nodes, and nodes that have one or fewer children are called external nodes. A leaf is a leaf. The height of a tree (or subtree because trees are recursive) is the number of nodes from the root to a leaf, not including the root. In the above diagram, the height of 8 is 2 because the longest path to a leaf has two nodes in it, 7 and 6. Sometimes you see the height including the root as well, so the height of 8 could also be 3. Either way is correct as long as it's consistently used.

Okay, so it's easy to search efficiently in a binary search tree. But what about easily inserting and deleting nodes? That doesn't seem easy at first glance. Well, inserting into a tree is both simple and efficient if you look at it from the perspective of an unsuccessful search. If you search for the new item that you know isn't there, you can replace the leaf you're sure to get to with a new node that has that item. You can also handle duplicates with an extra special case that either treats a duplicate as less than or greater than itself. Either way works, and the duplicates will line up down the path.

Deletion is harder (but not as much as many people would like you to think), so we'll save it for now and look at it in detail a bit later. But what about keeping the data sorted? Since every node's left children are less than or equal to it, and every node's right children are greater than or equal to it, the items are kept in sorted order. Sometimes it's a little tricky to actually take advantage of that order because the next item isn't necessarily in an adjacent node. Consider 2 and 3 being separated by 4, and 4 and 5 being separated by 2. We'll look at several ways of exploiting this structure to get what we want, but for now just remember that it really is sorted!

So trees meet all of our requirements. They're easy to search efficiently, items are sorted by default, so no extra work is needed there, it's easy to insert new items, and if you take my word for it, deletion isn't that bad either. Okay, so we know what we want, now let's try to put concept to code. The first thing we need is a structure for a node. It's also convenient to treat a tree as a whole, so we'll use a second structure that just holds the root of the tree:

    struct jsw_node 
    {
        int data;
        struct jsw_node *link[2];
    };

    struct jsw_tree 
    {
        struct jsw_node *root;
    };

Okay, this is a typical self-referential data structure. If you know how a linked list works, there shouldn't be any problem. A structure can contain a pointer to itself, so it's easy to link together two instances by assigning the address of one to the pointer member of the other. The only confusing part should be the link array. Yes, I could have used two pointers called left and right, but as you'll see shortly, the operations on a binary search tree are symmetric. By using an array and a boolean value as the index for that array, we can avoid unnecessary repetition of code with only a minor increase in complexity. Once you get used to the idiom, it's actually simpler than the usual left/right idiom. We'll simply use a null pointer for a leaf, since that's the most common solution and it's pretty easy to test for.

With those preliminaries out of the way, we can go straight to the meat of the tutorial. We'll start with searching, since you already know how it works. That way you can see how the operations are performed with code to strike home all of the details.

#### Search

Okay, we know that a search starts at the root and moves either left or right depending on whether the item we're searching for is greater than or less than the item at the root (ignoring a tree that allows duplicates for now). Then we go left or right and do the same thing with the next node. Since we're basically treating each subtree as if it were a unique tree, and each step does the same thing, we can do all of this recursively. The code is so simple, it's almost frightening:

    int jsw_find_r(struct jsw_node *root, int data)
    {
        if (root == NULL)
        {
            return 0;
        }
        else if (root->data == data)
        {
            return 1;
        }
        else
        {
            int dir = root->data < data;

            return jsw_find_r(root->link[dir], data);
        }
    }

    int jsw_find(struct jsw_tree *tree, int data)
    {
        return jsw_find_r(tree->root, data);
    }

The base cases for the recursion are a successful and unsuccessful search. If the root is a null pointer, we've reached a leaf and can return a failure code. If the item contained in the root matches the item we're searching for, we can return a success code. Otherwise, we test whether the item we're looking for is less than or greater than the root and move left or right, respectively. The return codes are pushed all the way back up the recursion, so **jsw\_find** can return what it gets. 0 means the item wasn't found and 1 means that it was. The point of **jsw\_find** is for convenience. Now a user can just say **jsw\_find ( tree, data )** and treat the tree as a black box instead of grabbing the root with **jsw\_find\_r ( tree-\>root, data )**.

Okay, I don't imagine that this code is a big stumbling block for you, and if it is then you're probably not quite ready for binary search trees. However, the magic with **dir** and the link array is almost always confusing to people who are new to it, so I'll go over how it works. We know that **link[0]** is the left subtree and **link[1]** is the right subtree because I told you before, so to go left we need to make sure that the comparison results in 0 and to go right we need to make sure that the result of the comparison is 1. That's a problem because the most obvious test, **data \< root-\>data**, gives us 1 and that's the wrong direction because we want to go left. So to get things to work the correct way, we reverse the test and ask if **root-\>data \< data**. This makes sure that if **data** is less than **root-\>data**, we get 0 and move in the correct direction.

While some things are easier with trees when you use recursion, many people prefer non-recursive solutions for a number of good reasons. Searching a tree without recursion is even simpler than with recursion, removes the problem of stack overflow, and has a tendency to execute faster and with less memory use:

    int jsw_find(struct jsw_tree *tree, int data)
    {
        struct jsw_node *it = tree->root;

        while (it != NULL)
        {
            if (it->data == data)
            {
                return 1;
            }
            else
            {
                int dir = it->data < data;

                it = it->link[dir];
            }
        }

        return 0;
    }

Some notes on duplicate items are in order. These search algorithms will find the first match, but if you want to search for duplicates, such as counting the duplicates, it's a little harder. In such a case you would probably want to return a pointer to the data, and also save a pointer to the node that matched, so that you can restart the search where you left off last. That way you can find subsequent matches past the first. The most common trees disallow duplicates, so for the most part we'll assume that in this tutorial. It's also easier on me because I don't have to write extra code for the functions that work with duplicates. ;-)

#### Insertion

A lot of the stuff you want to do with a tree involves searching, that's why they're so efficient. A search will ideally divide the problem in half at each step, so it's a good idea to take advantage of that for other operations too. For example, insertion into a tree is a variation of an unsuccessful search. You search for an item that's not in the tree, then replace the first leaf you get to with a new node that contains that item:

     Find 7

               5

           /       \

         2           8

       /   \       /   \

     1       4   ~       9

     Not found, insert at leaf

               5

           /       \

         2           8

       /   \       /   \

     1       4   7       9

The code to pull this off is just a simple variation of the search algorithm, but the recursive code takes advantage of recursion to simplify updates. Here's the recursive code to insert into a binary search tree (**make\_node** simply allocates memory, assigns the data and sets the links to NULL):

    struct jsw_node *jsw_insert_r(struct jsw_node *root, int data)
    {
        if (root == NULL)
        {
            root = make_node(data);
        }
        else if (root->data == data)
        {
            return root;
        }
        else
        {
            int dir = root->data < data;

            root->link[dir] = jsw_insert_r(root->link[dir], data);
        }

        return root;
    }

    int jsw_insert(struct jsw_tree *tree, int data)
    {
        tree->root = jsw_insert_r(tree->root, data);

        return 1;
    }

The only difference between **jsw\_insert\_r** and **jsw\_find\_r** is what you return for the base cases. However, it might not be immediately obvious how the recursive update works. As we ride the wave of recursion back up the tree, we re-assign the next link down the path to the return value of **jsw\_insert\_r**. This way we can be sure that any changes made to the lower parts of the tree can be recognized by the higher parts of the tree. That's a common pitfall, by the way. Always remember to reset the parent of a node that changes so that your changes stick.

Since we only insert at a leaf, and leaves are at the bottom of the tree, trees grow downward, which make the whole “tree” concept confusing and tempts us again to think of trees as “roots”. Let's insert a few numbers into a new tree to see how this works:

     Insert 5

     5

     Insert 2

         5

       /

     2

     Insert 8

         5

       /   \

     2       8

     Insert 4

         5

       /   \

     2       8

       \

         4

     Insert 9

         5

       /   \

     2       8

       \       \

         4       9

     Insert 3

         5

       /   \

     2       8

       \       \

         4       9

       /

     3

     Insert 1

             5

           /   \

         2       8

       /   \       \

     1       4       9

           /

         3

As with **jsw\_find**, **jsw\_insert** can be implemented without recursion as well. Just like the recursive and non-recursive search functions, non-recursive insertion has several advantages over recursive insertion. For large trees, you don't have to worry about hitting some arbitrary limit for the depth of the recursion. A loop is almost always faster than the equivalent behavior of recursive function calls, and it saves on local variable space as well. Sometimes recursive is just the best option because of its simplicity, but in my opinion, many of the non-recursive solutions for trees aren't that much more complicated. It's usually worth the effort to do an implementation without recursion.

The only real difference between the non-recursive search and the non-recursive insertion is that insertion needs to be careful not to actually get to a leaf. Because a leaf is a null pointer, there's no way to determine which node in the tree we came from and it's impossible to link the new node into the tree. That defeats the purpose of insertion since we can't make the change stick. So the loop tests the next link before going to it, and if it's a leaf, we break out of the loop and tack a new node onto that link. Since there's no next link that points to the root of the tree itself, we treat that as a special case. There are ways to get around the special case of an empty tree, but I think it's cleaner this way:

    int jsw_insert(struct jsw_tree *tree, int data)
    {
        if (tree->root == NULL)
        {
            tree->root = make_node(data);
        }
        else
        {
            struct jsw_node *it = tree->root;
            int dir;

            for (;;)
            {
                dir = it->data < data;

                if (it->data == data)
                {
                    return 0;
                }
                else if (it->link[dir] == NULL)
                {
                    break;
                }

                it = it->link[dir];
            }

            it->link[dir] = make_node(data);
        }

        return 1;
    }

Since the conditions for exiting the loop are between the code to choose a direction and the code to move in that direction, an infinite loop is used. Sure, it's possible to set up a “proper” loop, but I don't think those methods are as clean in this case, and since I'm the author, my opinion rules for my tutorial, so there. :-) Note that in all of these implementations, you can allow duplicates simply by removing the test for equality. Everything will just work and duplicates will fall underneath each other.

#### Deletion

The ease with which we could insert into a tree might lull you into a false sense of security. Well, if all you need to do is search and insert then everything is fine, but most of us at some point find the need to remove an existing node from the tree. This isn't as easy as inserting into a tree because this time we don't have the option of where to remove the node. If it's an external node then the process is about as simple as inserting a node. However, if it's an internal node, things get tricky.

To remove an external node, all you have to do is replace it with its non-leaf child, or a leaf if it doesn't have any. There are three simple cases for that. If the node has a left child, replace it with the left child because it's an external node and the right child is sure to be a leaf. If the node has a right child, this is the symmetric case for if the node has a left child. If the node has no children, just pick one because they're both leaves. Be sure to reset the parent to point to the child, and it unlinks the node from the tree:

     Left child

         p              p

           \              \

             x      ->      c

           /   \          /   \

         c       ~      *       *

       /   \

     *       *

     Right child

     p                  p

       \                  \

         x          ->      c

       /   \              /   \

     ~       c          *       *

           /   \

         *       *

     No children

     p              p

       \              \

         x      ->      ~

       /   \

     ~       ~

The key element to success for this is making sure that the parent's link is updated to point to either **c** or a leaf. If this isn't done then **x** won't be successfully removed from the tree, and any subsequent freeing of memory will cause “issues”. Fortunately, as long as we have access to the parent node, these three cases can be lumped together with a single block of code. It's probably the ugliest thing I can think of for basic trees, because you need to test which child of **x** is non-null and which child of **p** **x** is with our funky little reverse test:

    p->link[p->link[1] == x] = x->link[x->link[0] == NULL];

Cool, huh? It's like removal for a linked list, we replace **p**'s next link with **x**'s next link, thus removing **x** from the chain so that we can free it safely. The test for **x-\>link[0]** against **NULL** will give us 1 if the left link is null (the second case), and we'll use the right link to replace **x**. If the right link is also null, **x** had no children (the third case). If the test gives us 0, the left link is not null (the first case). Checking **x** against **p**'s right link does the same thing but in a different way. If **x** is **p**'s right link, the test will give us 1, which is correct. Otherwise, the test will result in 0, and since **x** isn't a leaf, we can be sure that it's **p**'s left child. All done with one line (we'll handle the special case of deleting the tree's root in a bit, since that's a special case), this is the simple case for deletion. It gets harder when we want to delete a node with two children.

               5

           /       \

         2           8

       /   \       /   \

     1       4   7       9

Let's delete 5 from the tree above, just pretend that it's not the tree's root. ;-) We can't simply replace it with one of its children because that would cause some uncomfortable problems, most notably what do we do with the extra child that would be floating around? Okay, what about attaching 5's left subtree to 7's left link and replacing 5 with 8?

                 8

               /   \

             7       9

           /

         2

       /   \

     1       4

That maintains the rule that left nodes are less and right nodes are greater, but it's not exactly trivial to do surgery like that, and we can do better with less work and avoid changing the structure around so much. How about instead of moving around subtrees, we just replace 5 with 4 or 7? These are the inorder predecessor and successor, respectively. All of the work is in finding one of those nodes, because then all you need to do in most trees is copy the data of one to the other, then remove the predecessor or successor that you replaced it with. In this tutorial, we'll use the successor, but it really makes no difference which you choose:

     Copy 7 to 5

               7

           /       \

         2           8

       /   \       /   \

     1       4   7       9

     Remove the external 7

               7

           /       \

         2           8

       /   \       /   \

     1       4   ~       9

The nice thing about this trick (it's called deletion by copying) is that we can take the hard case of deleting a node with two children and turn it into a case of deleting a node with only one child. Wait, how does that work though? What if the successor has two children?

The inorder predecessor of a node never has a right child, and the inorder successor never has a left child. So we can be sure that the successor always has at most one child, and the left child is a leaf. Following from this, we have an easy way of finding the successor of any node provided it's in the right subtree of that node. Simply walk once to the right, then go all the way to the left. Likewise, to find the predecessor, you walk once to the left and then all the way to the right.

Now, this works for finding a successor down the tree, but what if we want to know the successor of 4? Well, that's harder, but we won't ever see that situation in the basic deletion algorithm, so we can set it aside for now. We have enough foundation in place to actually go about removing a node from the tree. First we search for the node to delete, just as we did with insertion, except this time we'll also save the parent as we go down. When we get to the found item (we just return failure if we reach a leaf), we test to see how many children it has and do either the simple case or the hard case depending on that. Here's the code:

    int jsw_remove(struct jsw_tree *tree, int data)
    {
        if (tree->root != NULL)
        {
            struct jsw_node *p = NULL, *succ;
            struct jsw_node *it = tree->root;
            int dir;

            for (;;)
            {
                if (it == NULL)
                {
                    return 0;
                }
                else if (it->data == data)
                {
                    break;
                }

                dir = it->data < data;
                p = it;
                it = it->link[dir];
            }

            if (it->link[0] != NULL && it->link[1] != NULL)
            {
                p = it;
                succ = it->link[1];

                while (succ->link[0] != NULL)
                {
                    p = succ;
                    succ = succ->link[0];
                }

                it->data = succ->data;
                p->link[p->link[1] == succ] = succ->link[1];

                free(succ);
            }
            else
            {
                dir = it->link[0] == NULL;

                if (p == NULL)
                {
                    tree->root = it->link[dir];
                }
                else
                {
                    p->link[p->link[1] == it] = it->link[dir];
                }

                free(it);
            }
        }

        return 1;
    }

The search is almost identical to non-recursive insertion, except this time we don't need to split the direction and movement code, and we also set **p** to **it** before setting **it** to **it-\>dir**. This saves the parent, and if the **p** is **NULL** when the search is over, we're looking at a special case of deleting the root of the tree.

Let's look at the two cases after the search is over. In the first case, if the node as two children, we need to find the inorder successor. As you already know, to do that we simply move right once, then left until the next left is a leaf. During this process, we also take the parent with us so that at the end of this walk down the tree, **succ** is the successor and **p** is the successor's parent. We then copy the successor's data to the node we want to delete, but actually remove the successor instead. Notice that the parent's next link is always given **succ-\>link[1]** in this case, because we know that the left link is a leaf. The reverse test **p-\>link[1] == succ** is basically the same thing as we've been doing. It results in 0 if succ is **p**'s left link and 1 if **succ** is **p**'s right link. Free the unlinked node and we're done.

In the second case, the node we want to delete only has one child, it's an external node. This is the easy case because we don't need to find the successor, we can just cut to the part where we remove the node directly. We pick which side is not a leaf, and give that subtree to **p** as **it**'s replacement link. However, if **p** is **NULL** then we're trying to delete the root of the tree. That can be an issue because any changes we make have to be assigned to **tree-\>root** or they won't be saved, so we treat that as a special case. This special case doesn't exist if the node we want to delete has two children, because even if it's the root of the tree, we'll be replacing its value instead of actually unlinking it.

Now, while this method is relatively short and easy, it's somewhat of a naive approach. We're solving the problem directly when we could be working smarter. Notice how both cases actually end up removing an external node, but they're still separate cases because we need to find the successor for one of them. Let's try something different. Instead of stopping when we find the node we want to delete, let's save it and keep going all of the way down to an external node, since we'll do that anyway. Then when we get to the bottom, we copy the current node's data into the saved node, and remove the current node:

    int jsw_remove(struct jsw_tree *tree, int data)
    {
        if (tree->root != NULL)
        {
            struct jsw_node head = { 0 };
            struct jsw_node *it = &head;
            struct jsw_node *p, *f = NULL;
            int dir = 1;

            it->link[1] = tree->root;

            while (it->link[dir] != NULL)
            {
                p = it;
                it = it->link[dir];
                dir = it->data <= data;

                if (it->data == data)
                {
                    f = it;
                }
            }

            if (f != NULL)
            {
                f->data = it->data;
                p->link[p->link[1] == it] = it->link[it->link[0] == NULL];
                free(it);
            }

            tree->root = head.link[1];
        }

        return 1;
    }

This function introduces two tricks to avoid special cases. The first trick is a dummy root so that the root of the tree always has a parent. The second trick is saving the found node so that we can copy the data when we get to an external node. Now we can avoid the special case of deleting the root of the tree, and the special case of finding the successor. The code is much shorter and more elegant. Notice that we also test whether **it-\>data** is less than or equal to **data** because we want to keep going even if we found a matching item, and we want to go to the right to get to the successor of that item.

We can delete from a binary search tree with recursion as well, but it's not as clean. The following function follows a similar approach as the smarter deletion described above. When we find the node we want to delete, we then find the successor and copy the data. Then we trick the function by changing the data we're searching for to the successor's (also testing for less than or equal because there will be a match after copying the successor's data). The recursion continues until we get to the successor, at which point we do a simple case deletion. Then we ride the recursion wave back up to make sure that all of the changes stick:

    struct jsw_node *jsw_remove_r(struct jsw_node *root, int data)
    {
        if (root != NULL)
        {
            int dir;

            if (root->data == data)
            {
                if (root->link[0] != NULL && root->link[1] != NULL)
                {
                    struct jsw_node *succ = root->link[1];

                    while (succ->link[0] != NULL)
                    {
                        succ = succ->link[0];
                    }

                    data = succ->data;
                    root->data = data;
                }
                else
                {
                    struct jsw_node *save = root;

                    root = root->link[root->link[0] == NULL];
                    free(save);

                    return root;
                }
            }

            dir = root->data <= data;
            root->link[dir] = jsw_remove_r(root->link[dir], data);
        }

        return root;
    }

    int jsw_remove(struct jsw_tree *tree, int data)
    {
        tree->root = jsw_remove_r(tree->root, data);

        return 1;
    }

#### Destruction

At some point you'll get sick and tired of trees, and you'll want to kill it. But you don't want memory leaks, so you want to remove every node and free its memory correctly. Sure, you could call **jsw\_remove** a bunch of times, but that's hackish. A better way is to traverse every node in the tree and delete them all in one shot. Ignoring the details of traversal for now because we'll discuss it shortly, this can be done with a postorder traversal, and the recursive solution is trivial:

    void jsw_destroy_r(struct jsw_node *root)
    {
        if (root != NULL)
        {
            jsw_destroy_r(root->link[0]);
            jsw_destroy_r(root->link[1]);
            free(root);
        }
    }

    void jsw_destroy(struct jsw_tree *tree)
    {
        jsw_destroy_r(tree->root);
    }

It can also be done without using recursion, but the non-recursive postorder traversal isn't exactly easy. However, you don't need to do a postorder traversal to destroy a tree. Not if you're willing to change the structure to suit your needs, that is. ;-) If we have a tree where every left link is a leaf, it's easy to walk down the tree and delete every node: just save the right link, delete the node, then go to the saved link:

         0

       /   \

     ~       1

           /   \

         ~       2

               /   \

             ~       3

                   /   \

                 ~       4

                       /   \

                     ~       5

                           /   \

                         ~       ~

The trick is forcing this particular structure from any possible tree. We can go about it with an operation called a rotation. A rotation can be either left or right. A left rotation takes the right child of a node and makes it the parent, with the node being the new left child. A right rotation is the symmetric inverse of a left rotation:

     Right rotation

             3              1

           /   \          /   \

         1       4  ->  0       3

       /   \                  /   \

     0       2              2       4

Notice how 2 moves from the left subtree to the right subtree. The only nodes that are changed in the rotation are 1 and 3. 3's left link becomes 1's right link, and 1's right link becomes 3. A left rotation on the second tree would result in the first tree, thus proving that rotations are symmetric. So, if we delete a node when there's no left link and do a right rotation when there is, we can be sure that we'll see and delete every node in the tree. The code to do this is surprisingly short:

    void jsw_destroy(struct jsw_tree *tree)
    {
        struct jsw_node *it = tree->root;
        struct jsw_node *save;

        while (it != NULL)
        {
            if (it->link[0] != NULL)
            {
                /* Right rotation */
                save = it->link[0];
                it->link[0] = save->link[1];
                save->link[1] = it;
            }
            else
            {
                save = it->link[1];
                free(it);
            }

            it = save;
        }
    }

Okay, in case you didn't notice, this section was the end of the foundation sections and the beginning of the intermediate sections. I've divided the tutorial up so that the easy stuff is at the beginning, because I've been told that my last tutorial on binary search trees was complicated. From here on out, I'm going to assume that you have a good foundation and pretty much know what you're doing. The next section will cover traversal, which can get pretty heady, but I'll try to keep it light. Following traversal, we'll talk about parent pointers and threading, which can be considered the advanced parts of the tutorial. Last, some comments on performance properties and hints about what comes after the basics, and then you'll be done with this tutorial. :-)

#### Traversal

Now that we have the code to build a binary search tree, we can do things with it. Useful things. Fun things. Naturally, you can use the search functions defined previously to determine if an item is in the tree, but it would be nice to verify that the insertion functions work properly. It would also be nice to be able to perform an operation on every item in the list, such as printing the items to standard output or a file. This brings us to the field of tree traversal.

You would think that visiting every node in a tree would be simple and consist of one, maybe two different cases, right? Wrong! There are actually N! (N factorial) different ways to traverse a binary search tree of N nodes, but most of them are useless. Of the many ways to traverse a binary search tree, we'll be looking at two categories: breadth-first, where we look at the level-order traversal as an example, and depth-first, where we play with preorder, inorder, and postorder traversals. We'll also look at a more flexible stepwise traversal that you would find in a good tree library.

Depth-first traversals begin by moving as far left or right as possible before backtracking. They then move up one link and move left or right again. This process is repeated until all nodes have been visited. As expected, the depth-first traversals can be written recursively because the movement is based on stack behavior. The question of course, is when do the nodes get visited? Since we can only move in one of two directions, we can determine that there are only six possible ways to traverse and visit in a depth-first traversal. Because each operation can be one of “move left”, “move right”, or “visit”, the following variants are available:

1.  visit, move left, move right
2.  visit, move right, move left
3.  move left, visit, move right
4.  move right, visit, move left
5.  move left, move right, visit
6.  move right, move left, visit

Of these six, three are common enough to give standardized names: traversal 1 is referred to as preorder because the node is visited first, traversal 3 is called inorder because it results in a traversal in the sorted order of the node values, and finally, traversal 5 is called postorder because the node is visited after both movements. Each of these can be written using a short and elegant recursive algorithm:

    void jsw_preorder_r(struct jsw_node *root)
    {
        if (root != NULL)
        {
            printf("%d\n", root->data);
            jsw_preorder_r(root->link[0]);
            jsw_preorder_r(root->link[1]);
        }
    }

    void jsw_preorder(struct jsw_tree *tree)
    {
        jsw_preorder_r(tree->root);
    }

    void jsw_inorder_r(struct jsw_node *root)
    {
        if (root != NULL)
        {
            jsw_inorder_r(root->link[0]);
            printf("%d\n", root->data);
            jsw_inorder_r(root->link[1]);
        }
    }

    void jsw_inorder(struct jsw_tree *tree)
    {
        jsw_inorder_r(tree->root);
    }

    void jsw_postorder_r(struct jsw_node *root)
    {
        if (root != NULL)
        {
            jsw_postorder_r(root->link[0]);
            jsw_postorder_r(root->link[1]);
            printf("%d\n", root->data);
        }
    }

    void jsw_postorder(struct jsw_tree *tree)
    {
        jsw_postorder_r(tree->root);
    }

Let's look at an example. Given the following tree, we'll look at the results of each traversal. The preorder traversal visits a node and then moves, so the nodes would be visited in the order of 5, 3, 2, 7, 6, 8. An inorder traversal moves as far left as possible before visiting the node, so the order would be 2, 3, 5, 6, 7, 8. Postorder traversal would result in 2, 3, 6, 8, 7, 5.

             5

           /   \

         3       7

       /       /   \

     2       6       8

Okay, so we can print out all of the nodes in a tree, but let's do something fun just to take away the monotony. Instead of just getting all of the values, let's actually look at the structure of the tree. You can do this easily with an inorder traversal, and it prints out the tree rotated 90° counter-clockwise:

    void jsw_structure_r(struct jsw_node *root, int level)
    {
        int i;

        if (root == NULL)
        {
            for (i = 0; i < level; i++)
            {
                putchar('\t');
            }

            puts("~");
        }
        else
        {
            jsw_structure_r(root->link[1], level + 1);

            for (i = 0; i < level; i++)
            {
                putchar('\t');
            }

            printf("%d\n", root->data);

            jsw_structure_r(root->link[0], level + 1);
        }
    }

    void jsw_structure(struct jsw_tree *tree)
    {
        jsw_structure_r(tree->root, 0);
    }

Notice how this is basically the same thing as **jsw\_inorder**, except instead of just printing the value of the node, we also print a number of tabs that correspond to the level of the node. We also print leaves, just so that you can tell the tree ended properly. So while the recursive traversals may seem like kiddie toys, they can be very powerful when used creatively. See what you can come up with. :-)

Despite these wonderfully concise solutions, it can be prudent to take the non-recursive approach to traversal. These traversals are more difficult because of the dual recursive calls. If you're the kind of person who reads ahead to try and trick the teacher (shame on you!) then you may have noticed similarities between the preorder traversal and the levelorder travesal if the queue were replaced with a stack. If not, don't worry about it. Believe it or not, that's precisely what needs to be done to write an iterative preorder traversal. Wow:

    void jsw_preorder(struct jsw_tree *tree)
    {
        struct jsw_node *it = tree->root;
        struct jsw_node *up[50];
        int top = 0;

        if (it == NULL)
        {
            return;
        }

        up[top++] = it;

        while (top != 0)
        {
            it = up[--top];

            printf("%d\n", it->data);

            if (it->link[1] != NULL)
            {
                up[top++] = it->link[1];
            }

            if (it->link[0] != NULL)
            {
                up[top++] = it->link[0];
            }
        }
    }

Inorder traversal is harder. We need to walk to the left without losing any of the right links or any of the parents. This implies at least several loops, one to save the links for backtracking, one to visit the saved links, and another to manage successive branches. Fortunately, while the logic is rather complex, the code is surprisingly simple:

    void jsw_inorder(struct jsw_tree *tree)
    {
        struct jsw_node *it = tree->root;
        struct jsw_node *up[50];
        int top = 0;

        while (it != NULL)
        {
            while (it != NULL)
            {
                if (it->link[1] != NULL)
                {
                    up[top++] = it->link[1];
                }

                up[top++] = it;
                it = it->link[0];
            }

            it = up[--top];

            while (top != 0 && it->link[1] == NULL)
            {
                printf("%d\n", it->data);
                it = up[--top];
            }

            printf("%d\n", it->data);

            if (top == 0)
            {
                break;
            }

            it = up[--top];
        }
    }

The outer loop continues until it is a null pointer. This could be due to an empty tree at the very beginning, or because there are no more nodes left on the stack. You'll notice that the last statement of the outer loop is careful to set walk to **NULL** if the stack is empty, so that the algorithm actually stops. The first inner loop handles the saving of right links and parents while moving down the left links. The second inner loop handles the visiting of parents. Lastly, the final call to **printf** takes care of lingering right links. A diagram of the execution using the same tree as the recursive traversals is in order.

    save 7,  stack = { 7 }
    save 5,  stack = { 5, 7 }
    save 3,  stack = { 3, 5, 7 }
    save 2,  stack = { 2, 3, 5, 7 }
    visit 2, stack = { 3, 5, 7 }
    visit 3, stack = { 5, 7 }
    visit 5, stack = { 7 }
    pop 7,   stack = {}
    save 8,  stack = { 8 }
    save 7,  stack = { 7, 8 }
    save 6,  stack = { 6, 7, 8 }
    visit 6, stack = { 7, 8 }
    visit 7, stack = { 8 }
    pop 8,   stack = {}
    save 8,  stack = { 8 }
    visit 8, stack = {}

The most difficult of the non-recursive depth-first traversals is postorder. The difficulty comes from trying to work out a way to visit items on a lower level while still saving the parents and visiting them at the right time. I invariably find myself using a stack and helper counts, where 0 means “save the left link”, 1 means “save the right link”, and 2 means “visit the top of the stack”. This solution is convenient because it fits comfortably into my scheme for using boolean values to determine whether to go left or right:

    void jsw_postorder(struct jsw_tree *tree)
    {
        struct
        {
            struct jsw_node *p;
            int n;
        } up[50], it;
        int top = 0, dir;

        up[top].p = tree->root;
        up[top++].n = 0;

        while (top != 0)
        {
            it = up[--top];

            if (it.n != 2)
            {
                dir = it.n++;
                up[top++] = it;

                if (it.p->link[dir] != NULL)
                {
                    up[top].p = it.p->link[dir];
                    up[top++].n = 0;
                }
            }
            else
            {
                printf("%d\n", it.p->data);
            }
        }
    }

The code is short, but incredibly opaque. A diagram of the execution helps a great deal in figuring out what the algorithm is really doing. It's not as hard as it looks, I swear!

    push 5:0,  stack = { 5:0 }
    increment, stack = { 5:1 }
    push 3:0,  stack = { 3:0, 5:1 }
    increment, stack = { 3:1, 5:1 }
    push 2:0,  stack = { 2:0, 3:1, 5:1 }
    increment, stack = { 2:1, 3:1, 5:1 }
    increment, stack = { 2:2, 3:1, 5:1 }
    visit 2:2, stack = { 3:1, 5:1 }
    increment, stack = { 3:2, 5:1 }
    visit 3:2, stack = { 5:1 }
    increment, stack = { 5:2 }
    push 7:0,  stack = { 7:0, 5:2 }
    increment, stack = { 7:1, 5:2 }
    push 6:0,  stack = { 6:0, 7:1, 5:2 }
    increment, stack = { 6:1, 7:1, 5:2 }
    increment, stack = { 6:2, 7:1, 5:2 }
    visit 6:2, stack = { 7:1, 5:2 }
    increment, stack = { 7:2, 5:2 }
    push 8:0,  stack = { 8:0, 7:2, 5:2 }
    increment, stack = { 8:1, 7:2, 5:2 }
    increment, stack = { 8:2, 7:2, 5:2 }
    visit 8:2, stack = { 7:2, 5:2 }
    visit 7:2, stack = { 5:2 }
    visit 5:2, stack = {}

Levelorder traversal looks at the tree as a stack of levels, where each level consists of all nodes with the same height, and moves along each item in one level before moving on to the next level. The most common implementation starts at the root and traverses each level from left to right. For example, in the following tree, a levelorder traversal would visit the items in the order 5, 3, 7, 2, 6, 8. Note that this isn't the only way to do a levelorder traversal, but because it's the most common, we'll use it as our example for discussion. So there.

             5

           /   \

         3       7

       /       /   \

     2       6       8

The levelorder traversal is one of the few algorithms on a binary search tree that can't be written recursively without bending in all kinds of awkward, and probably uncomfortable, directions. For those that are familiar with recursion, you know that a recursive solution can be simulated through the use of a stack. Levelorder traversal requires the effects of a queue, however, so recursion isn't very practical.

The algorithm itself is very simple in theory, and it follows the same logic as preorder traversal except with a queue. For each node, the left and right links are pushed onto queue. The item is then visited (visiting an item is simply performing an operation on that item, such as printing it to standard output). The next item to visit can be found by popping the first item off of the queue. The following is an example using the tree above:

    save 5,  queue = { 5 }
    visit 5, queue = {}
    save 3,  queue = { 3 }
    save 7,  queue = { 7, 3 }
    visit 3, queue = { 7 }
    save 2,  queue = { 2, 7 }
    visit 7, queue = { 2 }
    save 6,  queue = { 6, 2 }
    save 8,  queue = { 8, 6, 2 }
    visit 2, queue = { 8, 6 }
    visit 6, queue = { 8 }
    visit 8, queue = {}

Once you understand what's going on, an algorithm is short and sweet. It's basically the preorder traversal with a queue instead of a stack. I used a simple array based rotating queue with front and back indices, but beyond that it's a really simple function:

    void jsw_levelorder(struct jsw_tree *tree)
    {
        struct jsw_node *it = tree->root;
        struct jsw_node *q[50];
        int front = 0, back = 0;

        if (it == NULL)
        {
            return;
        }

        q[front++] = it;

        while (front != back)
        {
            it = q[back++];

            printf("%d\n", it->data);

            if (it->link[0] != NULL)
            {
                q[front++] = it->link[0];
            }

            if (it->link[1] != NULL)
            {
                q[front++] = it->link[1];
            }
        }
    }

Since this is a tutorial about trees and not queues, I'll try to restrain myself from describing how it works and we can simply trust that it does. Or you can test my code to make sure that I'm not just pulling your leg. And I have been known to make mistakes from time to time, so it never hurts to double check me. ;-)

All of these traversals are fine for what they do, but they're not usually fine for what we want to do. Or at least what *I* want to do. What I want to do is this:

    int *x = first(tree);

    while (x != NULL)
    {
        printf("%d\n", *x);
        x = next(tree);
    }

That's hard to do with both the recursive and non-recursive traversals that we've looked at so far because we need to save the state of the last step in the traversal. The easiest way to go about this is a separate traversal structure that holds the information that we need. Because we need to save nodes that are further up the tree, a stack is needed, just like the depth-first traversals. We also need to keep track of which node is the current node in the traversal, so we'll go with the following structure (as usual, the size of the stack is pretty arbitrary, but it shouldn't be less than the expected height of the tree):

    struct jsw_trav
    {
        struct jsw_node *up[50]; /* Stack */
        struct jsw_node *it;     /* Current node */
        int top;                 /* Top of stack */
    };

Now for the fun part. Since the most common traversal by far is an inorder traversal, we'll do that one. It's by no means the only traversal we can do in a stepwise manner, but accessing the items in sorted order is probably the most useful traversal, especially since it's pretty easy to add both forward and backward movement. To start off an inorder traversal, we need to find the smallest node, or the node that's farthest to the left. So we write a simple function called **jsw\_first**, which initializes a **jsw\_trav** instance, moves the current node to the smallest item, and saves the path:

    int *jsw_first(struct jsw_trav *trav, struct jsw_tree *tree)
    {
        trav->it = tree->root;
        trav->top = 0;

        if (trav->it != NULL)
        {
            while (trav->it->link[0] != NULL)
            {
                trav->up[trav->top++] = trav->it;
                trav->it = trav->it->link[0];
            }
        }

        if (trav->it != NULL)
        {
            return &trav->it->data;
        }
        else
        {
            return NULL;
        }
    }

Notice how a pointer to the item is returned instead of the item itself. This makes it easier to test for boundaries because when the traversal is finished, we can simply return a null pointer. That fits much more comfortably into the desirable traversal loop that I gave above. **jsw\_first** is a simple function, and we could modify to be **jsw\_last** by simply changing the 0's to 1's, to go right instead of left.

Now for the hard part. We need to perform a single step in the inorder traversal, starting from the smallest node. The code to do this is relatively simple. If the current node has a right link, we find the inorder successor down the tree and update the stack accordingly. If the current node doesn't have a right link, we need to find the inorder successor up the tree, which involves popping the stack as long as we've already visited the right link. If the stack is empty, we terminate the traversal by setting the current node to **NULL**.

    int *jsw_next(struct jsw_trav *trav)
    {
        if (trav->it->link[1] != NULL)
        {
            trav->up[trav->top++] = trav->it;
            trav->it = trav->it->link[1];

            while (trav->it->link[0] != NULL)
            {
                trav->up[trav->top++] = trav->it;
                trav->it = trav->it->link[0];
            }
        }
        else
        {
            struct jsw_node *last;

            do
            {
                if (trav->top == 0)
                {
                    trav->it = NULL;
                    break;
                }

                last = trav->it;
                trav->it = trav->up[--trav->top];
            } while (last == trav->it->link[1]);
        }

        if (trav->it != NULL)
        {
            return &trav->it->data;
        }
        else
        {
            return NULL;
        }
    }

Now I can do what I want to do, with only minor changes, and it only took boat loads of extra thought to make a stepwise traversal from a non-recursive traversal. Aren't you glad that I did it for you? :-)

    struct jsw_trav it;
    int *x = first(&it, tree);

    while (x != NULL)
    {
        printf("%d\n", *x);
        x = next(&it);
    }

All in all, a stepwise traversal isn't much more difficult than a non-recursive traversal, and it's so much more flexible it's not even funny. You can do any number of traversals stepwise, but like I said before, inorder is by far the most common. Play around with the non-recursive traversals and see if you can make them all stepwise. If you can, I can safely say that you understand the concept. :-)

#### Parent Pointers

The biggest problem with binary search trees is when you need to do something that involves walking back up the tree, such as a traversal (or later, balancing). Unless we get clever, we need to use either an explicit stack to save the path down, or an implicit stack through recursion. However, sometimes it makes sense to get clever. For example, with recursion we might hit an arbitrary limit, and there's no way to determine what that limit is in a portable way. With the explicit stack, we've been setting the arbitrary limit because we were using a stack that doesn't grow on its own.

Now, you could use a nice stack library that grows as needed, or you could build the ability to move back up the tree into the tree. As it turns out, the latter is more common than the former, and by far the most common solution is an extra link for every node that points up to the parent. There are called, cleverly, parent pointers:

    struct jsw_node
    {
        int data;
        struct jsw_node *up;
        struct jsw_node *link[2];
    };

    struct jsw_tree
    {
        struct jsw_node *root;
    };

Insertion into a tree with parent pointers is pretty simple. Only a small bit of code needs to be added to the function to set the parent pointer correctly. However, care needs to be taken to give the root's parent pointer a leaf value so we can test for it. All of this is done in **make\_node**, which allocates memory to a new node, assigns the data, and sets all links to **NULL**:

    int jsw_insert(struct jsw_tree *tree, int data)
    {
        if (tree->root == NULL)
        {
            tree->root = make_node(data);
        }
        else
        {
            struct jsw_node *it = tree->root;
            int dir;

            for (;;)
            {
                dir = it->data < data;

                if (it->data == data)
                {
                    return 0;
                }
                else if (it->link[dir] == NULL)
                {
                    break;
                }

                it = it->link[dir];
            }

            it->link[dir] = make_node(data);
            it->link[dir]->up = it;
        }

        return 1;
    }

Since the parent of the new node is it, we have no trouble setting the link. As long as you have access to the parent, setting a parent pointer is triviality itself. Of course, that makes a recursive solution slightly more complicated than it would be without parent pointers.

Deletion from a tree with parent pointers is also slightly more complicated, but we're also saved the need for an extra variable to save the parent. In this case, special care needs to be taken in updating the parent pointer because it's possible that the replacement node is a leaf! This can happen with both the tree's root and further down the tree, so these are two special cases:

    int jsw_remove(struct jsw_tree *tree, int data)
    {
        if (tree->root != NULL)
        {
            struct jsw_node head = { 0 };
            struct jsw_node *it = &head;
            struct jsw_node *f = NULL;
            int dir = 1;

            it->link[1] = tree->root;
            tree->root->up = &head;

            while (it->link[dir] != NULL)
            {
                it = it->link[dir];
                dir = it->data <= data;

                if (it->data == data)
                {
                    f = it;
                }
            }

            if (f != NULL)
            {
                int dir = it->link[0] == NULL;

                f->data = it->data;
                it->up->link[it->up->link[1] == it] = it->link[dir];

                if (it->link[dir] != NULL)
                {
                    it->link[dir]->up = it->up;
                }

                free(it);
            }

            tree->root = head.link[1];

            if (tree->root != NULL)
            {
                tree->root->up = NULL;
            }
        }

        return 1;
    }

Finally we come to traversal. This is the whole reason for adding parent pointers. Now we don't need that kludgy stack to move toward a successor up the tree, we can just follow the parent pointers. **jsw\_trav** and **jsw\_first** are almost identical, differing only in that they don't use a stack:

    struct jsw_trav
    {
        struct jsw_node *it;
    };

    int *jsw_first(struct jsw_trav *trav, struct jsw_tree *tree)
    {
        trav->it = tree->root;

        if (trav->it != NULL)
        {
            while (trav->it->link[0] != NULL)
            {
                trav->it = trav->it->link[0];
            }
        }

        if (trav->it != NULL)
        {
            return &trav->it->data;
        }
        else
        {
            return NULL;
        }
    }

The big differences are in **jsw\_next**, where we now remove the use of the stack and simply follow a parent pointer when upward movement is needed. The logic is basically the same, and the changes are minimal. Not a bad trade for removing that stack, is it?

    int *jsw_next(struct jsw_trav *trav)
    {
        if (trav->it->link[1] != NULL)
        {
            trav->it = trav->it->link[1];

            while (trav->it->link[0] != NULL)
            {
                trav->it = trav->it->link[0];
            }
        }
        else
        {
            for (;;)
            {
                if (trav->it->up == NULL || trav->it == trav->it->up->link[0])
                {
                    trav->it = trav->it->up;
                    break;
                }

                trav->it = trav->it->up;
            }
        }

        if (trav->it != NULL)
        {
            return &trav->it->data;
        }
        else
        {
            return NULL;
        }
    }

#### Right Threading

Parent pointers are useful in many ways, but the extra overhead per node can be prohibitive. A very clever solution was devised where the leaves of a tree can be reused, so that instead of pointing to null, they point to the inorder successor or predecessor of the external node. This is called a threaded tree. Now the overhead of an extra pointer goes away and becomes the overhead of a little flag that tells us whether a link is a thread or a real link. Why the flag? Because we'll get stuck in an infinite loop if we can't differentiate between a legitimate link and a thread. One flag is needed per link in the tree, so a fully threaded binary search tree would need two flags. A more common solution only uses one thread for the right link and simply leaves the left links as they are in a normal tree:

    struct jsw_node
    {
        int data;
        int thread;
        struct jsw_node *link[2];
    };

    struct jsw_tree
    {
        struct jsw_node *root;
    };

In the following tree built using the above structures, every right link that would normally be a leaf is now a link to the inorder successor and every left link that would normally be a leaf is still normally a leaf. Notice how we can now get directly from 4 to 6 instead of working our way up past 3 through a parent pointer or a stacked path. That's the primary benefit of threaded trees, and as you'll see shortly, it simplifies the stepwise traversal drastically. Also notice how a search for 5 would just go around in a circle from 6 to 3 to 4 and back to 6 again. That's generally considered undesirable, hence the flag.

                         6

                 /       |       \
                         |
               3         |         7
                         |                 ~
           /   |  \      |       /   \     |
               |         |                 |
         2     |    4    |     ~       8   |
               |         |                 |
       /   \___|  /   \__|           /   \_|

     ~          ~                  ~

Threaded trees are not very common, but the most common of them will only allow threads for the right links, as shown above, thus allowing an inorder traversal in ascending sorted order, but to traverse in descending sorted order, we need to use one of the other solutions for a traversal (parent pointers, recursion, or a stack). These are called right threaded trees, because only the right side is threaded, and the code is more complicated than with parent pointers. The search algorithm must be changed to handle a thread, where if we reach a thread then it counts the same as if we reached a leaf so as to avoid that nasty little endless loop problem:

    int jsw_find(struct jsw_tree *tree, int data)
    {
        struct jsw_node *it = tree->root;
        int dir;

        for (;;)
        {
            dir = it->data < data;

            if (it->data == data)
            {
                return 1;
            }
            else if (dir == 1 && it->thread == 1)
            {
                break;
            }
            else if (it->link[dir] == NULL)
            {
                break;
            }

            it = it->link[dir];
        }

        return 0;
    }

Because the symmetry of the tree has been broken, we now need to consider the differences between moving to the right (where we might see a thread but never a leaf) and moving to the left (where we might see a leaf, but never a thread). Insertion follows the same search pattern, but the actual insertion needs to make the same distinction because if the new node is inserted as a right link, it needs to take over the thread of its parent before being linked into the tree. If the new node is inserted as a left link, its thread needs to be set to the parent. Consider inserting 5 in to the threaded tree shown above. 5 goes to the right link of 4, but 4's right link is a thread. To maintain a proper threaded tree, we need to shift that thread down to 5 so that 5's right link points to 6. On the other hand, if we were to insert 0, it would be placed on the left link of 2. But we still need to set 0's right thread to point to the inorder successor, which would be 2:

     Insert 5, save thread to 6

                            6

                 /          |       \
                            |
               3            |         7
                            |                 ~
           /   |  \         |       /   \     |
               |            |                 |
         2     |    4       |     ~       8   |
               |            |                 |
       /   \___|  /   \     |           /   \_|
                            |
     ~          ~       5   |         ~
                            |
                      /   \_|

                    ~

     Insert 0, new thread to 2

                                6

                     /          |       \
                                |
                   3            |         7
                                |                 ~
               /   |  \         |       /   \     |
                   |            |                 |
             2     |    4       |     ~       8   |
                   |            |                 |
           / | \___|  /   \     |           /   \_|
             |                  |
         0   |      ~       5   |         ~
             |                  |
       /   \_|            /   \_|

     ~                  ~

As long as you follow these two rules for insertion, building a threaded tree is easy because you only have to make very localized changes for it to work. The code to do this isn't terribly complicated, but it can be confusing if you're not familiar with threading because it does something different for the right and left links. **make\_node** still allocates memory, assigns the data, and sets the links to **NULL**, but for a threaded tree it also sets the flag to 1 because a new node is always an external node with no children. Notice also that the rightmost node in the tree will have a thread to a leaf, which might affect other operations on the tree. Here's the code:

    int jsw_insert(struct jsw_tree *tree, int data)
    {
        if (tree->root == NULL)
        {
            tree->root = make_node(data);
        }
        else
        {
            struct jsw_node *it = tree->root, *q;
            int dir;

            for (;;)
            {
                dir = it->data < data;

                if (it->data == data)
                {
                    return 0;
                }
                else if (dir == 1 && it->thread == 1)
                {
                    break;
                }
                else if (it->link[dir] == NULL)
                {
                    break;
                }

                it = it->link[dir];
            }

            q = make_node(data);

            if (dir == 1)
            {
                q->link[1] = it->link[1];
                it->thread = 0;
            }
            else
            {
                q->link[1] = it;
            }

            it->link[dir] = q;
        }

        return 1;
    }

Instead of setting the value of the new node directly to the parent, we save it first in a temporary variable. That way the current thread of the parent isn't lost if the new node is going to be the parent's right link. All in all, the process of inserting into a threaded tree isn't that tough, as long as you understand the concept. :-)

Deletion from a threaded tree follows the same pattern, but there are four cases for actually unlinking the node. If the node we want to remove has no children and is the left link of its parent, we can just set the left link of the parent to be a leaf. If it's the right link of its parent and it has no children, we need to set the right link of the parent to the right link of the node so as to save the thread.

     Remove 0, replace with leaf

                                6

                     /          |       \
                                |
                   3            |         7
                                |                 ~
               /   |  \         |       /   \     |
                   |            |                 |
             2     |    4       |     ~       8   |
                   |            |                 |
           / | \___|  /   \     |           /   \_|
             |                  |
         0   |      ~       5   |         ~
             |                  |
       /   \_|            /   \_|

     ~                  ~


                            6

                 /          |       \
                            |
               3            |         7
                            |                 ~
           /   |  \         |       /   \     |
               |            |                 |
         2     |    4       |     ~       8   |
               |            |                 |
       /   \___|  /   \     |           /   \_|
                            |
     ~          ~       5   |         ~
                            |
                      /   \_|

                    ~

     Remove 5, save thread

                            6

                 /          |       \
                            |
               3            |         7
                            |                 ~
           /   |  \         |       /   \     |
               |            |                 |
         2     |    4       |     ~       8   |
               |            |                 |
       /   \___|  /   \     |           /   \_|
                            |
     ~          ~       5   |         ~
                            |
                      /   \_|

                    ~


                         6

                 /       |       \
                         |
               3         |         7
                         |                 ~
           /   |  \      |       /   \     |
               |         |                 |
         2     |    4    |     ~       8   |
               |         |                 |
       /   \___|  /   \__|           /   \_|
                                 
     ~          ~                  ~

If the node we want to remove has a child (remember that it's an external node, so it'll only have one if any), and that child is to the right, we simply replace it with the child since no thread changes would be needed. On the other hand, if the child is to the left, we need to give the node's thread to its child before replacing it with the child. It's kind of tricky, but you'll see that it's worth in when we do a stepwise traversal:

     Remove 4, replace with 5

                                6

                     /          |       \
                                |
                   3            |         7
                                |                 ~
               /   |  \         |       /   \     |
                   |            |                 |
             2     |    4       |     ~       8   |
                   |            |                 |
           / | \___|  /   \     |           /   \_|
             |                  |
         0   |      ~       5   |         ~
             |                  |
       /   \_|            /   \_|

     ~                  ~


                             6

                     /       |       \
                             |
                   3         |         7
                             |                 ~
               /   |  \      |       /   \     |
                   |         |                 |
             2     |    5    |     ~       8   |
                   |         |                 |
           / | \___|  /   \__|           /   \_|
             |                
         0   |      ~                  ~
             |
       /   \_|

     ~

     Remove 2, save thread

                             6

                     /       |       \
                             |
                   3         |         7
                             |                 ~
               /   |  \      |       /   \     |
                   |         |                 |
             2     |    5    |     ~       8   |
                   |         |                 |
           / | \___|  /   \__|           /   \_|
             |                
         0   |      ~                  ~
             |
       /   \_|

     ~


                         6

                 /       |       \
                         |
               3         |         7
                         |                 ~
           /   |  \      |       /   \     |
               |         |                 |
         0     |    5    |     ~       8   |
               |         |                 |
       /   \___|  /   \__|           /   \_|
                              
     ~          ~                  ~

The code to implement these cases isn't terribly complicated, but it can be confusing at first. As usual, I'll use the elegant deletion because it's shorter and prettier. Pretty is good. I'm well known for taking the aesthetic appearance of my code very very seriously. Of course, that doesn't mean I don't also take correctness very very very seriously. ;-)

    int jsw_remove(struct jsw_tree *tree, int data)
    {
        if (tree->root != NULL)
        {
            struct jsw_node head = { 0 };
            struct jsw_node *it = &head;
            struct jsw_node *q, *p, *f = NULL;
            int dir = 1;

            it->link[1] = tree->root;

            while (it->link[dir] != NULL)
            {
                if (dir == 1 && it->thread == 1)
                {
                    break;
                }

                p = it;
                it = it->link[dir];
                dir = it->data <= data;

                if (it->data == data)
                {
                    f = it;
                }
            }

            if (f != NULL)
            {
                q = it->link[it->link[0] == NULL];
                dir = p->link[1] == it;
                f->data = it->data;

                if (p == q)
                {
                    p->link[0] = NULL;
                }
                else if (it->link[0] == NULL && it->thread)
                {
                    p->thread = 1;
                    p->link[1] = it->link[1];
                }
                else if (it->link[0] == NULL)
                {
                    p->link[dir] = q;
                }
                else
                {
                    q->thread = it->thread;
                    q->link[1] = it->link[1];
                    p->link[dir] = q;
                }

                free(it);
            }

            tree->root = head.link[1];
        }

        return 1;
    }

Okay, so how do these statements match the cases described above? Well, since **p** is the parent and **q** is the child we want to replace with, if **p** and **q** are the same node then **q** is **it**'s right link, and it's a thread to **p**. That amounts to the case where we remove 0 in the above diagrams. If the left link of the node we want to delete is a leaf and the right link is a thread, it matches the case where we remove 5. If the left link of the node we want to delete is null and the right link is not a thread, we've hit the same case as we did when removing 4. Finally, the last case covers removing 2 in the diagrams. With a good understanding of what happens at each case, the code is pretty simple.

The stepwise traversal is simplified greatly because to find the inorder successor up the tree, we only have to follow a link. All of the tricky gobbledygook for moving up, but only as long as we haven't gone to the right before is gone. The following is pretty much the holy grail of inorder traversals (with the exception that being able to go both ways using a fully threaded tree would be cooler). It's a shame we had to go to such lengths to build a tree that allows it:

    struct jsw_trav
    {
        struct jsw_node *it;
    };

    int *jsw_first(struct jsw_trav *trav, struct jsw_tree *tree)
    {
        trav->it = tree->root;

        if (trav->it != NULL)
        {
            while (trav->it->link[0] != NULL)
            {
                trav->it = trav->it->link[0];
            }
        }

        if (trav->it != NULL)
        {
            return &trav->it->data;
        }
        else
        {
            return NULL;
        }
    }

    int *jsw_next(struct jsw_trav *trav)
    {
        if (trav->it->thread == 0)
        {
            trav->it = trav->it->link[1];

            if (trav->it != NULL)
            {
                while (trav->it->link[0] != NULL)
                {
                    trav->it = trav->it->link[0];
                }
            }
        }
        else
        {
            trav->it = trav->it->link[1];
        }

        if (trav->it != NULL)
        {
            return &trav->it->data;
        }
        else
        {
            return NULL;
        }
    }

Parent pointers are more common than threaded trees simply because it's easier to visualize what's happening. Right threaded trees are the most common threaded tree, but you can also have a symmetrically threaded tree that's similar, but allows threads in both directions. The logic is similar enough that we only covered the more common variant so as to minimize my medical bills when I get carpal tunnel for writing these tutorials. ;-)

#### Performance

Some theory is useful for trees. You have to know how well they'll work in practice, and the analyses done by people smarter than I are beneficial in that respect. When insertions and deletions are made at random, the average case performance of a binary search tree is O(log N), or base 2 logarithmic, or the number of powers of 2 you need to get to N. That's really good, by the way. If a search is O(log N) then you shrink the number of potential matches by about half with each step. So a search in a binary search tree is fast when insertions and deletions are random.

Note that I keep saying “when insertions and deletions are random”. That's me covering my butt from nitpickers who would love to burn me for making an absolute statement when I have so much fun doing the same to them. In reality, binary search trees have a really nasty worst case if every node in the tree is external. The easiest way to obtain and visualize this worst case is to insert numbers in ascending sorted order:

     0

       \

         1

           \

             2

               \

                 3

                   \

                     4

                       \

                         5

Instead of that wonderful O(log N) performance, we basically have a glorified linked list and the performance degrades to O(N). This is called a degenerate tree, and it can occur with a sequence of exceptionally unlucky deletions and insertions that are impossible to predict. That's not exactly conducive to confidence in your trees. A tree that thinks about this and tries to correct it is called a balanced tree. All of the trees discussed in this tutorial are not balanced trees, so the only way to stack the deck in your favor is to make sure that insertions and deletions are random. That minimizes the chances of hitting a degenerate case.

There are ways to guarantee near optimal performance either with every operation or amortized over a bunch of operations, but those ways can be very complicated. I've written tutorials on a few of the more common and simpler ones, so I won't repeat myself here. It's also possible to globally rebalance a tree. One way is to copy the tree into an array of nodes, then do a something like a binary search on the array, recursively choosing the middle node in the array and inserting it into a new tree. That results in a well balanced tree. The above degenerate tree would look like this after such a rebalancing effort:

               3

           /       \

         1           5

       /   \       /

     0       2   4

Even better is that this rebalancing scheme is pretty easy to imagine and implement, but it's not very efficient. Another good way to globally rebalance a tree is to change the structure into the worst possible case as shown above (only temporarily though!), then perform what's called a rotation at every other node down the tree repeatedly. I'll leave an implementation up to you (it's called DSW, if you want to google it), but the process would look something like this:

     0 <-- rotate here

       \

         1

           \

             2 <-- rotate here

               \

                 3

                   \

                     4 <-- rotate here

                       \

                         5


         1 <-- rotate here

       /   \

     0       3

           /   \

         2       5

               /

             4


               3

           /       \

         1           5

       /   \       /

     0       2   4

Global rebalancing is an expensive operation because it's at least O(N), so if you choose to do it, put it off as long as possible. :-) Usually though, if degenerate trees are expected then a balanced tree scheme (such as AVL or red black) that fixes balance locally is a better choice because it spreads the inefficiency of maintaining balance across all of the individual operations instead of wrapping it all up in one big and inefficient operation.

The final conclusion is that the average case for a binary search tree of size N is O(log N) and the worst case is O(N), which makes the unbalanced binary search trees somewhat less useful than the balanced trees, with their guaranteed O(log N) performance in the average case. However, if the incoming data is known to be in random order then an unbalanced tree is way easier to implement and maintain than a balanced tree.

#### Conclusion

Whew! Who knew trees were so diverse? Well, I did, and now you do too. There are so many other topics concerning trees that I could fill my web space with tutorials about them. Most of the stuff is either impractical in the real world or only interesting to intellectuals who don't write real code though. I covered the topics that give you the most bang for your \<insert local currency\> that can actually be implemented by a normal human being. As always with data structures, you can get as complicated as you want, and some of the balanced trees do just that.

In this tutorial, you (hopefully!) learned what a binary search tree is and how to implement one. We went over not just the basics but also more elegant or efficient variations, and nifty tricks to improve the flexibility of the basic binary search tree by making traversals less awkward. Finally, we talked a little bit (very little) about some of the theory behind performance that concerns us and discovered not only the advantages of these basic trees, but also the disadvantages.

------------------------------------------------------------------------

© 2018 - Eternally Confuzzled
