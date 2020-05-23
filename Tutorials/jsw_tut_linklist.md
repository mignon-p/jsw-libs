Linked Lists
============

I'm constantly dismayed at the tutorials and descriptions of linked lists that I find online, but I can let that slide because there's no real restriction on what can be put on websites in terms of quality. The real disappointment is that books written on data structures usually gloss over linked lists on the way to more interesting data structures. It's a disappointment because not only are linked lists a foundation concept, they're also quite interesting by themselves.

As a foundation concept, linked lists provide the basis for further linked data structure work. If you don't understand linked lists, you'll be lost when working with binary search trees, for example. A firm understanding of linked lists will make further learning much easier, so the rush to get past them that most resources do is harmful. Linked lists aren't a necessary evil.

Too often people are fooled by the apparent simplicity of linked lists. The core concept really is that simple, but when it comes to creating and using linked lists, the options are virtually limitless. Therefore, this tutorial will aim not only to provide the strongest foundation in the data structure possible, but also to tickle the creative nerve so that you can see more of the options available to you when it comes to this fascinating data structure.

 

**Concept**

From a conceptual standpoint, there's really nothing to a linked list. Imagine a train. Each railroad car on a train would be a single node in a linked list. A node contains the contents of the node, such as gravel or passengers, as well as a link to the next node. This gives us a sequential list of nodes where each node is completely independent. Textually, we can represent a list containing the set of items {3,1,4,2,5} like so:

    3 -> 1 -> 4 -> 2 -> 5 -> ~

Each item is represented by a number, the links are represented by arrows, and the end of the list is represented by my preferred null designation character, the tilde. I use the tilde to signal something like "no node here". In reality it turns out to be a null pointer or a sentinel, but we'll get to that later. The order of the numbers depends on how the nodes are linked together. If 4 linked to 3 and 1 linked to 2, it might look something like this without changing the order of display:

             +-------+
             |       |
    +-> 3 -> 1   4   +-> 2 -> 5 -> ~
    |            |
    +------------+

That's overly confusing, but if you display the items in the order that they're linked, you get this:

    4 -> 3 -> 1 -> 2 -> 5 -> ~

This is an important concept when it comes to linked lists. Simply by changing links, you can re-order a linked list without moving around any data. This flexibility comes from the fact that, as said earlier, nodes are completely independent of each other. A node doesn't know or care where its links point to. But be careful, this can be a source of frustrating bugs if your links don't point where you think they do. ;-)

Because nodes are independent, it's very simple to add nodes to a list, remove them, insert them in between other nodes, splice lists into or out of another list, or any number of structural changes with relative ease. This is all compared to the same operations on an array, where all but the simplest of operations from the end of the array result in a lot of shifting and copying of data.

One drawback of a linked list is that of access. While working with a node is simple and efficient if you have immediate access to that node, getting to the node can be an issue. For example, in an array, you can use an index and get right to the item with a simple jump:

    #include <stdio.h>

    int main()
    {
        int a[] = { 1, 2, 3, 4, 5 };

        printf("%d\n", a[3]);

        return 0;
    }

This code will print 4, but if the same numbers were stored in a linked list, you would have to follow the links from 1 to get to 4 before being able to print. This is because linked lists do not support random access, only sequential access from whichever node you happen to have a reference to. Usually the only guaranteed reference is the first node in the list, so printing the 3rd index of a linked list would look more like this:

    node it = first_node;
    size_t i = 0;

    while (i != 3)
    {
        it = next_node(it);
        ++i;
    }

    printf("%d\n", node_data(it);

The exact details of the linked list code will be covered shortly, so I've kept the snippet to a minimum and used as little specifics as possible. Please accept my apologies and rest assured that you'll be writing linked lists in C, C++, C\#, Java, or whatever your favorite language is like a pro by the end of the tutorial. :-) To be specific about the code, assuming we have a reference to the first node in the list, the only way to get to the 3rd index is to follow the link from the first node, then the second node, then the third, and finally the fourth (where the zero-based index is finally incremented to 3). For longer lists this can become a serious bottleneck.

In summary, a linked list is a sequential collection of nodes where each node is independent and contains a link to the next node in the list. Linked lists shine as a replacement for arrays where insertions and deletions all over the list are common and random access is uncommon.

Okay, enough concept. Let's get on to the good stuff.

 

**Implementation Details**

There are tons of ways to implement a linked list. By far the most common is using a self-referential structure as the node, with a data member and a next member marking the link. The data member in a practical implementation would be a pointer to void such that the list could be used with any number of types. A second structure would contain the first node in the list and a counter for the number of nodes in the list. Other members could also be added for more list-specific information, such as a flag specifying whether the list has a dummy head or not (see Insertion and Deletion).

    struct jsw_node
    {
        void *data;
        struct jsw_node *next;
    };

    struct jsw_list
    {
        struct jsw_node *head;
        int has_dummy_head;
        size_t size;
    };

However, this isn't the only way to create a linked list, and this tutorial will cover some other methods. A few helper functions are in order as well, because common operations like creating and destroying nodes and lists would bulk up the list algorithms. So we'll just toss them into functions to reduce redundancy.

    /*
        Create a new node with the given data and links
        Returns a pointer to the new node, or NULL on error
    */
    struct jsw_node *new_node(void *data, struct jsw_node *next)
    {
        struct jsw_node *rv = malloc(sizeof *rv);

        if (rv != NULL)
        {
            rv->data = data;
            rv->next = next;
        }

        return rv;
    }

    /*
        Create a new list with an optional dummy head
        Returns a pointer to the new list, or NULL on error
    */
    struct jsw_list *new_list(int has_dummy_head)
    {
        struct jsw_list *rv = malloc(sizeof *rv);

        if (rv != NULL)
        {
            rv->head = has_dummy_head ? new_node(NULL, NULL) : NULL;
            rv->has_dummy_head = has_dummy_head;
            rv->size = 0;

            if (has_dummy_head && rv->head == NULL)
            {
                /* Release the list if a dummy couldn't be allocated */
                free(rv);
                rv = NULL;
            }
        }

        return rv;
    }

    /*
        Destroy a single given node, assuming it has been unlinked
        Optionally destroy the data contained in the node
        Returns the next node specified by the link
    */
    struct jsw_node *destroy_node(struct jsw_node *node, void (destroy_data)(void*))
    {
        struct jsw_node *rv = NULL;

        if (node != NULL)
        {
            /*
            Save a reference to the next node
            because we're about to destroy this one
            */
            rv = node->next;

            if (destroy_data != NULL)
            {
                destroy_data(node->data);
            }

            free(node);
        }

        return rv;
    }

    /*
        Destroy all nodes in a given list
        Optionally destroy all data in each node
    */
    void destroy_list(struct jsw_list *list, void (destroy_data)(void*))
    {
        while (list->head != NULL)
        {
            list->head = destroy_node(list->head, destroy_data);
        }
    }

We'll be assuming changes to these helpers later when we start to look at different variations of the linked list, but the changes are trivial and I'll describe what needs to be done.

Now that the basics are out of the way, let's look at inserting and deleting in a linked list!

 

**Insertion and Deletion**

To insert a node into a linked list, you need a reference to the node before where the new node is going to be inserted. Then it's a simple matter of re-linking the links in those two nodes. The new node's link should be set to the previous node's link, and the previous node's link should be set to the new node. Graphically the entire operation looks like this:

              3 -> ~

    1 -> 2 ------> 4 -> 5 -> ~


              3
              |
    1 -> 2 ---+--> 4 -> 5 -> ~


         +--> 3
         |    |
    1 -> 2    +--> 4 -> 5 -> ~


    1 -> 2 -> 3 -> 4 -> 5 -> ~

The fourth step is merely a cleanup of the representation so that you can see how the new node has been completely inserted into the list between 2 and 4. Keep in mind that 2's link cannot be reset to point to 3 until after 3's link has been set. Otherwise 2's link would be lost and you wouldn't know what to set 3's link to. Order of operations is very important here. The code to perform this magic is extremely simple:

    /*
        Insert a new node after the given node
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_after(struct jsw_list *list, struct jsw_node *pos, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            /* Create a new node and set the next link */
            rv = new_node(data, pos->next);

            if (rv != NULL)
            {
                pos->next = rv;
                ++list->size;
            }
        }

        return rv;
    }

There are a few problems by design in this function. The first problem is that if the list is empty, there's nothing to insert after. In such a case pos would be a null pointer and the insertion would fail. However, taking a null pointer and assuming that it refers to an empty list is potentially dangerous as well. If the list isn't actually empty, but the function assumes it is and terminates successfully, you have a difficult to trace bug.

The second problem is that it's impossible with to add a node to the head of the list with this function. Even if pos refers to the head of the list and the head of the list is not a null pointer, the new node will always be inserted after the given node. That is, the new node can only be the second in the list if added to the front. So the only way to use insert\_after to add a new node to the head of the list is to make sure that the list always has a dummy node for the head (notice the parameter to new\_list).

A dummy node is a node that has no value other than to simplify structural changes to the list. Dummy nodes are used in linked data structures to avoid special cases with the start or end of the data structure. The dummy head has no value, but it does have a link that can be followed to get to the "real" head of the list. The only problem now is that you have two heads to deal with: one for structural changes at the head of the list, and one for marking the first legitimate value in the list.

Now, you might be thinking "Well why not just write an insert\_before function?", and that's an excellent question. In fact, we'll be doing that right now. However, for solving the problem of inserting at the head of the list without a dummy, how might you do it while avoiding the first problem?

Keep in mind that a null pointer representing an empty list is indistinguishable from a null pointer representing a programming oopsie. All null pointers are equal in the world of C (or C++, or Java, or C\#, etc...), so the only way to guarantee that you're looking at a truly empty list is to use a sentinel node instead of a null pointer to mark an empty list. That adds unnecessary complexity because now you have two markers: one for an empty list and one for the end of the list. Using the sentinel as both does nothing to alleviate the problem, and you end up adding extra special cases.

The insert\_before function introduces a whole new bundle of problems. Now, without the dummy node, we're stuck with the problem of trying to insert before a null pointer, with the option of assuming that a null pointer means to insert onto the end of the list. This time the operation won't simply succeed with unexpected results. We would have to specifically add special case code to handle this situation, and it introduces the problem of assuming a null pointer means the calling code wants to insert onto the end of the list.

Option 1 is to disallow insert\_before to the end of the list. That's the safest solution, but it does make the users of the list think harder about which functions to use. Now, that's not necessarily a bad thing, but good library design says that things should work as expected. I'm not inclined to think that insert\_before on the end of the list failing is expected, so let's call this option a last resort.

Option 2 is to fix the problem the same way as with insert\_after, with a dummy node. This time the dummy node becomes a dummy tail. Add a new jsw\_node pointer called tail to the jsw\_list structure as well as a flag, has\_dummy\_tail. This requires changes to new\_list so that the tail can be created properly:

    /*
        Create a new list with an optional dummy head and tail
        Returns a pointer to the new list, or NULL on error
    */
    struct jsw_list *new_list(int has_dummy_head, int has_dummy_tail)
    {
        struct jsw_list *rv = malloc(sizeof *rv);

        if (rv != NULL)
        {
            struct jsw_node *tail = has_dummy_tail ? new_node(NULL, NULL) : NULL;

            if (has_dummy_tail && tail == NULL)
            {
                /* Release the list if a dummy couldn't be allocated */
                free(rv);
                rv = NULL;
            }
            else
            {
                rv->head = has_dummy_head ? new_node(NULL, tail) : NULL;
                rv->tail = tail;

                rv->has_dummy_head = has_dummy_head;
                rv->has_dummy_tail = has_dummy_tail;

                rv->size = 0;

                if (has_dummy_head && rv->head == NULL)
                {
                    /* Release the list if a dummy couldn't be allocated */
                    free(rv);
                    rv = NULL;
                }
            }
        }

        return rv;
    }

The changes are straightforward, but take note that other insertion and deletion algorithms need changes as well. Another problem with insert\_before is what happens when insert\_before is called with the dummy head as pos? This is a legitimate special case because we clearly don't want a node that links to the dummy but the dummy doesn't link to. This is effectively a resource leak for the list and the new node will appear to vanish.

With the introduction of a dummy tail, insert\_after will have the same problem, so both functions will need to be modified with a special case that changes the direction of the insert when on the dummies. The simplest solution is for insert\_after to call insert\_before when pos is the dummy tail, and insert before to call insert\_after when pos is the dummy head:

    /*
        Insert a new node after the given node
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_after(struct jsw_list *list, struct jsw_node *pos, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            if (pos != list->tail)
            {
                /* Create a new node and set the next link */
                rv = new_node(data, pos->next);

                if (rv != NULL)
                {
                    pos->next = rv;
                    ++list->size;
                }
            }
            else
            {
                rv = insert_before(list, pos, data);
            }
        }

        return rv;
    }

    /*
        Insert a new node before the given node
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_before(struct jsw_list *list, struct jsw_node *pos, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            if (pos != list->head)
            {
                /* Find the previous node */
                struct jsw_node *it = list->head;

                while (it->next != pos)
                {
                    it = it->next;
                }

                /* Create a new node and set the next link */
                rv = new_node(data, it->next);

                if (rv != NULL)
                {
                    it->next = rv;
                    ++list->size;
                }
            }
            else
            {
                rv = insert_after(list, pos, data);
            }
        }

        return rv;
    }

The insert\_before function is largely the inverse of insert after, but I'd like to draw your attention to the while loop in insert\_before. This represents a huge failing in this variation of the linked list, because the only way to insert before a node is to find the node that links to it. Because nodes only link to the next node, and not the previous node, this means that a complete search of the list from the very beginning is required.

Deletion from a linked list is extremely simple, provided you have a reference to the previous node. Deletion involves changing the links around a node so that they no longer refer to that node. Then the node itself can be freed. No link changes need to be made to the node being deleted unless it's being added to another list. Here's the graphical representation of deleting a node in a linked list:

    1 -> 2 -> 3 -> 4 -> 5 -> ~


              3
              |
    1 -> 2 ---+--> 4 -> 5 -> ~


              3 -> ~

    1 -> 2 ------> 4 -> 5 -> ~

I've added the update of 3's link to null so that the removal is more obvious. The operation really is the inverse of insertion in that 2's next link is set to 3's next link, then 3's next link is set to null. In code it's equally simple, but we need to take several things into account.

First, deletion of the tail should probably remove the last node in the list since that's what a user might expect. Second, deletion of the head should probably remove the first node in the list for the same reason. Finally, we need to account for an empty list. Have a look at the code, and I'll explain the decisions behind it afterward.

    /*
        Remove the selected node
        Returns the removed node or NULL on failure
    */
    struct jsw_node *remove_node(struct jsw_list *list, struct jsw_node *pos)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            struct jsw_node *it = list->head;
            struct jsw_node *prev = NULL;

            /* Shift the head by one if removing the dummy */
            if (pos == list->head)
            {
                pos = pos->next;
            }

            /* Find the previous node and its previous node */
            while (it->next != pos)
            {
                prev = it;
                it = it->next;
            }

            if (pos != list->tail)
            {
                /* Remove the selected node */
                rv = pos;
                it->next = rv->next;
                rv->next = NULL;
                --list->size;
            }
            else if (it != list->head)
            {
                /* Remove the node before the tail */
                rv = it;
                prev->next = rv->next;
                rv->next = NULL;
                --list->size;
            }
            else
            {
                /* The list was empty */
            }
        }

        return rv;
    }

The first decision is how to handle the case where pos is the dummy head of the list. This turns out to be a relatively simple problem because we can simply shift the node down by one and be at the first value node in the list. If the list is empty, pos would become the dummy tail, and that falls into the next special case.

For the case of when pos is the dummy tail, the obvious solution is also the one to be avoided. Walk down the list once to find the last node, the walk down the list again to remove that node. This is an extremely inefficient solution for finding the previous node of the previous node of pos. Instead, I selected a nice little trick for saving back links. The iterator variable is used to find the previous node of pos starting at the dummy head. Now, if you have another one of those that gets set to the iterator just before it moves to the next link, you have the iterator's previous node. If that previous node is NULL, then the iterator didn't have a previous node. With this technique, we can find the dummy tail's previous node's previous node and save ourselves a second trip down the list.

If you're thinking that this is a complete pain in the butt and totally inefficient when you already have a reference to the node you want to delete, I agree thoroughly. In fact, I agree so much that I'll immediately start writing about how to fix that problem. :-)

 

**Double Linked Lists**

So far, we've been looking at a linked list with one link to the next node. This is known technically as a single linked list, and it's certainly not the only option. In fact, another named variation uses two links: one to the next node and one to the previous node. This variation is known as the double linked list, and it's only slightly more difficult than the single linked list. Graphically, I would represent a double linked list like so:

    ~ <- 1 <-> 2 <-> 3 <-> 4 <-> 5 -> ~

Notice how the links now go both ways, so any node can link to the previous and next nodes. This means quite a few things to us. It means that we can traverse a list in either direction or even change directions and go back if we missed something. It means that we don't need to traverse the list to find the previous node for insertion or deletion because the pos node can now give us that information directly. And of course, it means that we have to work harder to maintain links. ;-)

Let's begin with the changes needed to our helpers. First, add a new link to the jsw\_node structure and call it prev. We'll also add another argument to new\_node before next (of the same type) and call it prev. This way new nodes can be created with data, a previous link, and a next link. Finally, we'll modify new\_list so that it properly links together the head and tail. This involves making sure that the next link of the head points to the tail and the prev link of the tail points to the head.

    /*
        Create a new list with an optional dummy head and tail
        Returns a pointer to the new list, or NULL on error
    */
    struct jsw_list *new_list(int has_dummy_head, int has_dummy_tail)
    {
        struct jsw_list *rv = malloc(sizeof *rv);

        if (rv != NULL)
        {
            struct jsw_node *tail = has_dummy_tail ? new_node(NULL, NULL, NULL) : NULL;

            if (has_dummy_tail && tail == NULL)
            {
                /* Release the list if a dummy couldn't be allocated */
                free(rv);
                rv = NULL;
            }
            else
            {
                rv->head = has_dummy_head ? new_node(NULL, NULL, tail) : NULL;

                /* Finish linking the tail to the head */
                rv->tail = tail;
                rv->tail->prev = rv->head;

                rv->has_dummy_head = has_dummy_head;
                rv->has_dummy_tail = has_dummy_tail;

                rv->size = 0;

                if (has_dummy_head && rv->head == NULL)
                {
                    /* Release the list if a dummy couldn't be allocated */
                    free(rv);
                    rv = NULL;
                }
            }
        }

        return rv;
    }

The prev link of the tail relies on the head already existing in a non-null state, which isn't the case. So the code is a little awkward in that new\_node isn't being used to it's fullest when creating the tail, and the prev link for the tail needs to be updated after the head has been created. This problem doesn't exist with the head because the tail has already been created, but for symmetry, one might choose to ignore new\_node's capabilities and manually set all of the links in this case.

The insert\_before and insert\_after functions suddenly become exact inverses of each other because insert\_before no longer needs to search the list. However, both need to take care to set all of the links (there are up to four that need to be set) without accidentally dereferencing a null pointer. The changes are shockingly simple:

    /*
        Insert a new node after the given node
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_after(struct jsw_list *list, struct jsw_node *pos, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            if (pos != list->tail)
            {
                /* Create a new node and set the next link */
                rv = new_node(data, pos, pos->next);

                if (rv != NULL)
                {
                    if (pos->next != NULL)
                    {
                        pos->next->prev = rv;
                    }

                    pos->next = rv;
                    ++list->size;
                }
            }
            else
            {
                rv = insert_before(list, pos, data);
            }
        }

        return rv;
    }

    /*
        Insert a new node before the given node
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_before(struct jsw_list *list, struct jsw_node *pos, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            if (pos != list->head)
            {
                /* Create a new node and set the next link */
                rv = new_node(data, pos->prev, pos);

                if (rv != NULL)
                {
                    if (pos->prev != NULL)
                    {
                        pos->prev->next = rv;
                    }

                    pos->prev = rv;
                    ++list->size;
                }
            }
            else
            {
                rv = insert_after(list, pos, data);
            }
        }

        return rv;
    }

Finally, deletion becomes an exercise in the trivial. Now that we can access both sides from any node, we no longer need to do the awkward search that was necessary before. The majority of the code consists of dealing with pos being either the dummy head or tail, and making sure that the two remaining links in the list are properly set:

    /*
        Remove the selected node
        Returns the removed node or NULL on failure
    */
    struct jsw_node *remove_node(struct jsw_list *list, struct jsw_node *pos)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            /* Shift off of the dummies */
            if (pos == list->head)
            {
                pos = pos->next;
            }

            if (pos == list->tail)
            {
                pos = pos->prev;
            }

            if (pos != list->head)
            {
                /* Remove a non-dummy node */
                rv = pos;

                /* Reset the list links if necessary */
                if (rv->prev != NULL)
                {
                    rv->prev->next = rv->next;
                }

                if (rv->next != NULL)
                {
                    rv->next->prev = rv->prev;
                }

                /* Clean up the old node */
                rv->prev = rv->next = NULL;
                --list->size;
            }
        }

        return rv;
    }

Notice that removing a non-dummy node only tests for the dummy head. While you could add a test for the dummy tail too, it would be unnecessary. There's no case where pos would be the dummy tail at that point because of the conditional immediately prior. It sets pos to pos-\>prev if it's the dummy tail.

Double linking solves so many problems with single linking, but maintaining the extra links can be a chore. Also note that with each new link, you're introducing more storage into the linked list. Sometimes it's better to restrict your operations in favor of less memory usage.

 

**Circular References**

Now that we've looked at single and double linked lists, null sentinels and dummy nodes, I'm sure you're convinced that linked lists are an exercise in annoying special cases. Now that you're disillusioned, let's simplify the world by changing the rules slightly. One simple modification to how a list ends can remove all of those annoying edge cases. Consider a linked list where the last node in the list wraps around to the first node:

    +-> 1 <-> 2 <-> 3 <-> 4 <-> 5 <-+
    |                               |
    +-------------------------------+

This is called an indirect circular reference. It's kind of like recursion in that 5 refers to itself by way of referring to 1, which refers to 2, which refers to 3, which refers to 4, which then refers to 5. What we have here is an infinite loop by design called a circular linked list. :-)

Now, I imagine you're starting to wonder what possible use could something like this have, so I'll let the code do the talking. Here are the updated helper functions to take advantage of circular references (the structure is the simple one from the beginning of the tutorial without the has\_dummy\_head flag):

    /*
        Create a new list
        Returns a pointer to the new list, or NULL on error
    */
    struct jsw_list *new_list(void)
    {
        struct jsw_list *rv = malloc(sizeof *rv);

        if (rv != NULL)
        {
            rv->head = new_node(NULL, NULL, NULL);
            rv->size = 0;

            if (rv->head != NULL)
            {
                /* Set up a circular reference situation */
                rv->head->next = rv->head;
                rv->head->prev = rv->head;
            }
            else
            {
                /* Release the list if a dummy couldn't be allocated */
                free(rv);
                rv = NULL;
            }
        }

        return rv;
    }

    /*
        Destroy all nodes in a given list
        Optionally destroy all data in each node
    */
    void destroy_list(struct jsw_list *list, void (destroy_data)(void *))
    {
        while (list->size > 0)
        {
            list->head = destroy_node(list->head, destroy_data);
            --list->size;
        }
    }

The new\_list function is greatly simplified because there's less work to do. As with the tail issue before, we can't set the prev or next links for head because they rely on head itself. The object has to be completely defined before the circular references can be set. The destroy\_list function needed to be changed because it relied on a null pointer terminating the list. Since that's not the case anymore, I modified it to use the size of the list instead. This isn't an ideal solution, but it should suffice for this tutorial.

The benefit of a circular linked list is that edge cases no longer exist. None of the algorithms have to check for a null pointer terminating the list, and all (well, most) operations are guaranteed to succeed barring memory allocation errors and other catastrophic meltdowns that break just about any program. Let's look at how insert\_before, insert\_after, and remove\_node were simplified through circular referencing:

    /*
        Insert a new node after the given node
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_after(struct jsw_list *list, struct jsw_node *pos, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            /* Create a new node and set the next link */
            rv = new_node(data, pos, pos->next);

            if (rv != NULL)
            {
                pos->next->prev = rv;
                pos->next = rv;
                ++list->size;
            }
        }

        return rv;
    }

    /*
        Insert a new node before the given node
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_before(struct jsw_list *list, struct jsw_node *pos, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            /* Create a new node and set the next link */
            rv = new_node(data, pos->prev, pos);

            if (rv != NULL)
            {
                pos->prev->next = rv;
                pos->prev = rv;
                ++list->size;
            }
        }

        return rv;
    }

    /*
        Remove the selected node
        Returns the removed node or NULL on failure
    */
    struct jsw_node *remove_node(struct jsw_list *list, struct jsw_node *pos)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL && pos != NULL)
        {
            if (pos != list->head)
            {
                /* Remove a non-header node */
                rv = pos;

                /* Reset the list links if necessary */
                rv->prev->next = rv->next;
                rv->next->prev = rv->prev;

                /* Clean up the old node */
                rv->prev = rv->next = NULL;
                --list->size;
            }
        }

        return rv;
    }

All of those pesky special cases for the end of the list are gone with the exception of removing the header node. Now, it's possible to do without the header node entirely, but then you need to account somehow for a truly empty list, or guarantee that a node is present before doing any work. The header acts as a guaranteed node and makes life easier. I encourage you to try different variations that you can think of and see what happens.

One thing to note about circular references is that in some languages with garbage collection, special care must be taken to clean up the links if a node points to itself. Sometimes this can trip up garbage collectors and the node won't ever be released. This is a subtle memory leak.

 

**Maintaining Order**

Now for a little fun. Often it's useful to keep a linked list sorted, and sorting a linked list can get a little tricky, as EC's Art of Sorting tutorial shows. However, because the nodes are independent and insertion anywhere in the list is an efficient operation, we can write an insert\_sorted function that maintains order in the list even when new items are being added. This is as simple as finding the sorted location of the new item and calling insert\_before to place it:

    /*
        Insert a new node in sorted order
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_sorted(struct jsw_list *list, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL)
        {
            /* Find the sorted position of the new node */
            struct jsw_node *it = list->head->next;

            while (it != list->head && compare(it->data, data) < 0)
            {
                it = it->next;
            }

            /* Delegate the insertion to an existing function */
            rv = insert_before(list, it, data);
        }

        return rv;
    }

The insert\_sorted function is little more than a wrapper around insert\_before that searches for the first node that has a greater or equal value to the new node's data. Once we have that node, we know that the new node's sorted position is directly in front of it. I'm assuming a compare function that knows how to take two pointers to void and compare the pointed-to values.

This technique is relevant to all of the linked list variations we've looked at so far. The number of links is irrelevant unless you want to be able to insert sorted nodes from back to front rather than front to back, and care must be taken to use the correct sentinel value for the end of the list. In the case of a circular list, we've started the search at the header's link and stop the search at the header itself. This produces the correct semantics.

With a dummy tail, the algorithm works properly, however, if the tail sentinel is a null pointer you need to change the algorithm slightly so that it uses insert\_after and the search stops at the node before the sorted location. This is because insert\_before would fail miserably with a null pointer. :-) With a dummy head, it would look like this:

    /*
        Insert a new node in sorted order
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_sorted(struct jsw_list *list, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL)
        {
            /* Find the sorted position of the new node */
            struct jsw_node *it = list->head;

            while (it->next != NULL && compare(it->next->data, data) < 0)
            {
                it = it->next;
            }

            /* Delegate the insertion to an existing function */
            rv = insert_after(list, it, data);
        }

        return rv;
    }

Without a dummy head, the function would have no choice but to use a special case when the new node is to be inserted onto the front of the list. While the following function assumes a single linked list, the only difference between it and the algorithm for a double linked list is the number of parameters to the new\_node function.

    /*
        Insert a new node in sorted order
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_sorted(struct jsw_list *list, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL)
        {
            if (compare(data, list->head->data) < 0)
            {
                rv = new_node(data, list->head);

                if (rv != NULL)
                {
                    /* Only reset the head on success */
                    list->head = rv;
                }
            }
            else
            {
                /* Find the sorted position of the new node */
                struct jsw_node *it = list->head;

                while (it->next != NULL && compare(it->next->data, data) < 0)
                {
                    it = it->next;
                }

                /* Delegate the insertion to an existing function */
                rv = insert_after(list, it, data);
            }
        }

        return rv;
    }

In the majority of cases, it's faster and easier to maintain order in a linked list than to explicitly sort an unsorted list, but what if you want to sort on multiple keys in the data? What if you want both the initial order as well as sorted order at the same time? This is where linked lists can get very complicated, because the sky is the limit when it comes to the number of links in a node and how you use them.

 

**Threads**

A linked list isn't restricted to one or two links, those are simply the most common variations. Further links that provide a different ordering of the nodes, or that offer a subset of nodes can be called threads because they're link chains that work their way through the nodes differently from the "main thread", which would be the prev and next links. For example, let's say you wanted a linked list that threaded the even and odd numbered nodes.

To do something like this we would need extra links for each node that specifies even or odd, and is a null pointer if not set. We would also need links to the start of each thread in the main list structure:

    struct jsw_node
    {
        void *data;
        struct jsw_node *next;
        struct jsw_node *link[2];
    };

    struct jsw_list
    {
        struct jsw_node *head;
        struct jsw_node *link[2];
        int has_dummy_head;
        size_t size;
    };

The new\_node and new\_list functions would need to be updated as well to handle these new links by setting them to null pointers or some other sentinel value. Creating the threads themselves is fairly simple. I'll break the operation out into a separate function for extra simplicity and because it makes sense to only create threads when they're needed. A truly generic approach would be to dynamically allocate new links as needed for the threads, with the thread logic being largely unknown to the list proper. Here is a create\_threads function that builds the even and odd threads:

    /*
        Create the odd/even threads
    */
    void create_threads(struct jsw_list *list)
    {
        struct jsw_node *it = list->has_dummy_head ? list->head->next : list->head;
        struct jsw_node *link[2] = { 0 };
        int i;

        for (i = 1; it != NULL; i++)
        {
            int dir = (i % 2 == 0);

            /* Set the start of the thread if necessary */
            if (link[dir] == NULL)
            {
                list->link[dir] = it;
            }

            /* Set the next link in the thread */
            if (link[dir] != NULL)
            {
                link[dir]->link[dir] = it;
            }

            /* Update the walkers */
            link[dir] = it;
            it = it->next;
        }
    }

Now, it might be difficult to see a situation where this kind of threading is needed, so let's look at something a bit more practical. Given a list of jobs, we want to sort those jobs by a certain name but also retain the original order that the jobs arrived. An example of this is a priority queue by insertion order that needs to be displayed onto a web site both by priority and by job name for user convenience.

To solve this problem, one might re-sort the queue as necessary for printing, or store a separate list with the different ordering. But with a threaded linked list, the same list can handle both orderings. This minimizes storage costs and maintains peformance. Like with the even-odd threads, we need to have both a next thread link in the node structure as well as a first thread link in the list structure. Since this example will be adding to the end of the threaded list, I've also added a tail link for the thread:

    struct jsw_node
    {
        void *data;
        struct jsw_node *next;
        struct jsw_node *next_added;
    };

    struct jsw_list
    {
        struct jsw_node *head;
        struct jsw_node *head_added;
        struct jsw_node *last_added;
        int has_dummy_head;
        size_t size;
    };

The list will be built with insert\_sorted, and the thread will handle the insertion priority. Be sure to select the thread that will be most commonly used as the main thread because this will make the list easier to understand as a whole. I chose the sorted list, but it's very likely that an application would be better off with the insertion priority list as the main thread.

With the structure changes (also the corresponding new\_node and new\_list updates), changing insert\_sorted to add newly inserted nodes to the thread is bordering on trivial:

    /*
        Insert a new node in sorted order
        Maintains the priority thread
        Returns a pointer to the new node or NULL on failure
    */
    struct jsw_node *insert_sorted(struct jsw_list *list, void *data)
    {
        struct jsw_node *rv = NULL;

        if (list != NULL)
        {
            /* Find the sorted position of the new node */
            struct jsw_node *it = list->head;

            while (it->next != NULL && compare(it->next->data, data) < 0)
            {
                it = it->next;
            }

            /* Delegate the insertion to an existing function */
            rv = insert_after(list, it, data);

            /* Add to the priority thread */
            if (rv != NULL)
            {
                if (list->head_added == NULL)
                {
                    list->head_added = rv;
                }
                else
                {
                    list->last_added->next_added = rv;
                }

                list->last_added = rv;
            }
        }

        return rv;
    }

The big changes are at the end. Making sure that rv isn't a null pointer (which would signal that the node failed to be inserted), the algorithm sets the start of the thread if needed and otherwise links the new node onto the end of the thread. Finally, the end of the thread is set to the new node so that further thread links will be in the correct order.

Keep in mind that threading does **not** create new nodes! Given five numbers inserted in the order {2,5,3,1,4}, the sorted list would look like this:

    head -> 1 -> 2 -> 3 -> 4 -> 5 -> ~

With the threads added, the nodes themselves do not change at all. The only difference is that now there are more links that represent the insertion order of {2,5,3,1,4}. Graphically, the threaded list could be represented like this:

    head_added ----+
                   |
                   |
                   |
              +----|---------+
              |    |         |
    head -+-> 1 -> 2 -+-> 3 -+-> 4 -+-> 5 -> ~
          |        |  |   |      |  |   |
          |        +--|---|------|--+   |
          |           +---|------|------+
          +---------------+      +-> ~

Take some time to read that, because it's difficult to follow with my crude ASCII art. Start at head\_added, then go down to 2, then down and over to 5, then down and back to 3, then down and back to 1, then up and over to 4, then down and over to null. The picture is convoluted, but the logic is extremely simple. And that's all there is to threading. The sky is the limit. :-)

 

**Linked Lists vs. Arrays**

The data structure that linked lists compete directly with is the array. There are four significant advantages to using linked lists over arrays that come to mind:

-   Linked lists can grow indefinitely
-   Linked lists can shrink, thus saving memory
-   Insertion is fast and cheap anywhere in the list
-   Deletion is fast and cheap anywhere in the list

Often these advantages outweigh the disadvantages, but there are also several advantages for arrays when it comes to linked lists vs. arrays:

-   Arrays support random access, linked lists only sequential
-   Arrays have a high locality of reference, which is good for the cache
-   Arrays require less memory to store a single item, linked lists need links
-   Arrays are vastly simpler to understand and work with

However, arrays have a big problem growing and shrinking, or adding or removing items anywhere except the end. So the usual advice is to use an array when the size is fairly set in stone, and adding or removing will only occur at the end of the array. Use a linked list when adding and removing is common all across the list, or the size of the list is unknown and likely to change often.

 

**Node Storage**

Up until now we've looked at the nodes in a linked list as little islands returned by malloc. This is by far the most common implementation, but the linked list is actually an abstraction. We're not restricted by an implementation details as long as the basic rule for a linked list is met: each node provides a link to the next.

This means that we can trade the advantages of one storage medium for the disadvantages of another. For example, a linked list where each node is allocated dynamically has awful cache performance. If we're writing a linked list that needs good cache performance but we're reasonably sure that it won't exceed a certain size, we can use an array to store the nodes.

I've introduced this concept to people before, and the typical response is "But that sounds like a lot of extra work and complication!". I'll let you be the judge. Converting all of the above linked list code so that it uses an array instead of dynamic nodes can be done by replacing new\_node and destroy\_node:

    static struct jsw_node free_list[MAX_SIZE];
    static size_t next_node = 0;

    /*
        Create a new node with the given data and links
        Returns a pointer to the new node, or NULL on error
    */
    struct jsw_node *new_node(void *data, struct jsw_node *next)
    {
        struct jsw_node *rv = NULL;

        if (next_node < MAX_SIZE)
        {
            rv = &free_list[next_node];
            rv->data = data;
            rv->next = next;
        }

        return rv;
    }

    /*
        Destroy a single given node, assuming it has been unlinked
        Optionally destroy the data contained in the node
        Returns the next node specified by the link
    */
    struct jsw_node *destroy_node(struct jsw_node *node, void (destroy_data)(void*))
    {
        struct jsw_node *rv = node->next;

        if (node != NULL)
        {
            if (destroy_data != NULL)
            {
                destroy_data(node->data);
            }
        }

        return rv;
    }

That's it! Well, not really. While this works to a point, the destruction of a node only destroys the data; it doesn't handle returning the node back to the free list for later use. This means that once you use a node, it's used up forever. The problem with handling this case is that new\_node is doling out pointers to the nodes in the free list. The only way to avoid invalidating those pointers while still being able to return freed nodes to the pool is to search the pool for unused pointers.

    static struct jsw_node free_list[MAX_SIZE];

    /*
        Create a new node with the given data and links
        Returns a pointer to the new node, or NULL on error
    */
    struct jsw_node *new_node(void *data, struct jsw_node *next)
    {
        struct jsw_node *rv = NULL;
        size_t i;

        for (i = 0; i < MAX_SIZE; i++)
        {
            if (!free_list[i].is_used)
            {
                break;
            }
        }

        if (i != MAX_SIZE)
        {
            rv = &free_list[i];
            rv->data = data;
            rv->is_used = 1;
            rv->next = next;
        }

        return rv;
    }

    /*
        Destroy a single given node, assuming it has been unlinked
        Optionally destroy the data contained in the node
        Returns the next node specified by the link
    */
    struct jsw_node *destroy_node(struct jsw_node *node, void (destroy_data)(void*))
    {
        struct jsw_node *rv = node->next;

        if (node != NULL)
        {
            if (destroy_data != NULL)
            {
                destroy_data(node->data);
            }

            node->is_used = 0;
        }

        return rv;
    }

Notice that I've added a flag to the node struct called is\_used and that it's designed so that the default initialization for the static array will set it to false. This eliminates the need to make an initialization pass over the array to make sure that the flags have the correct default value.

Now, the problem with this is fairly obvious. First, we've added a lot of extra work to new\_node, and with a long and largely full list we should expect it to take some time. Whether this time exceeds that of a call to malloc is up to debate, and I encourage benchmarking. However, the performance will fluctuate with this search, so we've cut ourselves off from certain speed guarantees. Fortunately, returning a node to the pool is very efficient.

Another problem deals with using an array to store the nodes in the first place. We've locked ourselves into one of the disadvantages of an array, and that's a bounded size. The linked list cannot grow larger than MAX\_SIZE with this setup. If MAX\_SIZE is chosen carefully, it probably won't be an issue, but wasted space and not enough space are two concerns.

As you may have noticed by now, when I raise an issue I usually have a solution or a workaround to the issue, and this one is no different. Unfortunately, there's not much you can do if your language of choice doesn't support dynamic allocation. However if it does, you can both minimize the number of allocations as well as keep mostly to arrays by using a linked list of array-based linked lists.

I'll repeat that because it bears repeating. The top level linked list will contain nodes where each node contains an array-based linked list like we defined above. Those inner linked lists will be linked together so that the outer linked list only serves as a container for the arrays. How about a drawing?

    head -+
          |
          +-> {[0]->[1]->[2]->[3]->[4]->[5]} -+
                                              |
          +-----------------------------------+
          |
          +-> {[0]->[1]->[2]->[3]->[4]->[5]} -+
                                              |
          +-----------------------------------+
          |
          +-> {[0]->[1]->[2]->[3]->[4]->[5]} -> ~

At the end of each array-based list, the link will go either to a sentinel (or null pointer), or to the first node in the next array-based list. This creates a linked list of arrays where each array is a linked list, and they are all chained together into one big abstract linked list. The implementation isn't overly difficult, and it's a good way to test your understanding of linked lists, so I'll leave it as an exercise. :-)

------------------------------------------------------------------------

© 2018 - Eternally Confuzzled
