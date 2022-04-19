# Basic Tries



If you're a Jedi, there is no try. But if you're a programmer, trie is
an option. :) But what exactly is a trie? The name is weird, but it
suggests some form of tree data structure given that programmers tend to
be somewhat whimsical when it comes to naming things. And that
assumption is quite correct. The name 'trie' comes from 're**trie**val',
which tells us something of the purpose of this data structure. Other
names are digital tree, radix tree, or prefix tree, of which there are
multiple variations. This tutorial will cover the most basic of
variations to keep things relatively succinct.

Okay, so really, what *is* a trie? It is an n-ary (ie. multi-way) search
tree where each node in the tree contains a common prefix for subsequent
child nodes. What this means is that unlike a binary search tree, the
complete value is not necessarily stored in a single node. Let's look at
an example. Consider a set of strings: {"cat", "can", "cry", "cut",
"bat", "bool", "batch", "bot", "bath"}. A basic trie of this set would
look like the following:

```
            (root)
          /        \
         c           b
      /  |  \      /   \
     a   r   u    a     o
    / \  |   |    |    / \
   n   t y   t    t   o   t
                 / \  |
                c   h l
                |
                h

```

The idea is that each node in the tree represents a character of a word.
Some characters are shared, such as with the 't' in "bath" and "batch".
In this case, the 't' node has one or more links to subsequent unshared
suffixes of the word. Notice that a node can have zero or more links to
child nodes, with no specified limit. In this way a trie is different
from a binary tree where the upper limit of possible child links is 2.
Typically the storage for links in a trie node is represented by an
array, a linked list, or a dictionary.

At first glance, this seems silly. What possible use could this data
structure have? That's an easy question to answer. Consider a text
editor with auto-completion as you type. At each letter, you can follow
all child chains of the trie and quickly retrieve a list of possible
suffixes for auto-completion. Another common use case for tries is spell
checking.

It's important to note that tries are not limited to string data. Any
key type that can be broken down into relevant pieces and is internally
consistent can be used, provided the implementation of the trie supports
it. Strings fall into this category easily, as do the digits in a
number, or bits in a binary value. Tries are also commonly used for IP
matching in network routers. In fact, it is probably a toss-up between
strings and binary data for the most common key type stored in trie
implementations.

One question you might have is whether this structure is inefficient. If
all you're storing per node is a single character, aren't tries
wasteful? Yes and no. There are variations which avoid long chains where
there is only one child, such as in the "cry" chain from our example
above. Discussion of these variations is beyond the scope of this
tutorial though. Aside from that, tries are actually quite efficient.
The height of the tree is bounded by the length of the longest chain,
which is certainly a consideration. Also, by merging shared prefixes
tries tend to branch efficiently as well. By nature, a trie is well
balanced when multiple items are stored (where an item is the
concatenation of chains).

"But wait\!", the observant among you will say, "how do you know that
'bat' is a word in the example when the node has children?" You got me,
this data structure is useless because of that edge case. Psych\! That's
actually quite easy to correct, simply by storing a flag in the node to
mark the node as a terminal. A terminal node may or may not have
children, but still says that this node is the end of an item even if
there are more suffixes for other items. If we mark terminal nodes with
'\!' then the more accurate trie looks like this:

```
              (root)
            /        \
          c             b
       /  |  \       /     \
     a    r   u     a       o
    / \   |   |     |      / \
   n!  t! y!  t!    t!    o   t!
                   / \    |
                  c   h!  l!
                  |
                  h!

```

Every leaf node is also a terminal, but the node in question for "bat"
is also a terminal despite being an internal node. This flag is critical
for ensuring proper operation of the basic trie. Enough yakking though.
Much of the details will become clear by studying a simple
implementation. Let's start with the node:

```csharp
    public class TrieNode
    {
        public char Key { get; set; }
        public bool Terminal { get; set; }
        public SortedDictionary<char, TrieNode> Links { get; private set; }

        public TrieNode(char key, bool terminal = false)
        {
            Key = key;
            Terminal = terminal;
            Links = new SortedDictionary<char, TrieNode>();
        }
    }
```

Each node is keyed on a character in the string, so we make sure to
store that. A boolean flag marking the node as terminal or not is
included, and finally we store child links as a sorted dictionary. The
benefit of the dictionary is threefold. First, it can store a
theoretically unlimited number of children, which is exactly what we
need. Second, only the links that are used get stored, so there is not a
significant amount of wasted memory. Some trie implementations use an
array where the size of the array is however many characters there might
be. For example, all letters in the latin alphabet would see the size of
the array set to 27. But in this case there will be a great amount of
wasted space as most of those links will go unused. Finally, because the
dictionary is sorted, we have similar benefits to a binary search tree
when it comes to search and traversal guarantees. The down side of a
dictionary is slower performance as we need to consider the speed of a
lookup in the `Links` object. You will always need to consider such
trade off options between speed and space, but in my opinion the above
is the best starting point in general.

Of course, a node is useless alone, so we will also create a trie class
to house it:

```csharp
    public class Trie
    {
        private TrieNode Root { get; set; }

        public Trie()
        {
            // Create a dummy root for simplicity
            Root = new TrieNode(default(char));
        }

        // Other operations go here
    }
```

The class still doesn't do anything other than create and initialize a
root node, but we do need to look at the purpose of this root node. The
root in a trie is typically a dummy. The reason for this is the fact
that tries are n-ary trees. Since there could potentially be many
'first' characters, a consistent starting point is quite beneficial. So
we create a root node with a default value and ensure that it is
non-terminal. The default value is largely irrelevant, provided all
operations on the trie take care to recognize the root node as not part
of any legitimate chain.

#### Search

Searching in a trie is an exercise in the trivial. Given a search item,
simply loop through each part of the item and traverse the trie until
you run out of parts or run out of nodes. If you run out of parts and
the resulting node is a terminal, you have found a match:

```csharp
    public bool Match(string s)
    {
        var it = Root;

        foreach (var c in s)
        {
            if (!it.Links.ContainsKey(c))
            {
                // No more nodes, match failure
                return false;
            }

            it = it.Links[c];
        }

        // No more characters, check for a terminal
        return it.Terminal;
    }
```

This is where one of the benefits of the dummy root takes effect. We do
not need to terminate the end of a chain with a null link because the
presence or absence of a key in the dictionary serves this purpose.
Because of the dummy root, this guarantee is consistent throughout the
entire structure and no special cases exist.

#### Insertion

Adding an item to a trie is almost as simple as searching one. It
consists of a search, where instead of stopping when there are no more
nodes, we insert a node and follow that new chain:

```csharp
    public void Add(string s)
    {
        if (string.IsNullOrEmpty(s))
        {
            return;
        }

        var it = Root;

        foreach (var c in s)
        {
            if (!it.Links.ContainsKey(c))
            {
                // No more nodes, extend the chain
                it.Links.Add(c, new TrieNode(c));
            }

            it = it.Links[c];
        }

        // No more characters, ensure a terminal
        it.Terminal = true;
    }
```

So when I said that no special cases for the root exist, that was a lie.
Sorry. In the case of insertion, you'll note that the matched node after
exhausing characters is unconditionally made into a terminal. If you try
to insert a blank string into the trie, this would result in the root
node being made terminal, which is not what we want. So either blank
strings need to be excluded from the insertion algorithm, or the matched
node must be checked against the root and not set as terminal.

Of course, none of that matters if blank strings are allowed. In that
case, the natural behavior of the algorithm is correct. We would also
not need to mark the dummy root as terminal initially because the
terminal flag states that a string was intentionally added to the trie
by the application. After construction, the empty trie should contain no
terminal nodes.

It should be recognized that there are cases where an item can be
successfully inserted without creating new nodes. An example would be
adding "bat" to a trie that already contains "batch". The effect will be
that no new nodes are created because they already exist, but the item
is still inserted by marking the 't' node as a terminal.

#### Intermission: Auto-complete

Before looking at deletion, let's consider one of the use cases
mentioned previously: auto-completion. Given a string prefix, we would
like to get a list of available suffixes from the trie. As with most
search-based algorithms, we start by searching for the prefix. If it is
not found then no suffixes are returned. If it *is* found, we
recursively gather all complete chains from its child nodes:

```csharp
    public IList<string> AutoComplete(string s)
    {
        var result = new List<string>();
        var it = Root;

        foreach (var c in s)
        {
            if (!it.Links.ContainsKey(c))
            {
                return new List<string>();
            }

            it = it.Links[c];
        }

        // Generate all possible suffixes
        AutoComplete(it, string.Empty, result);

        return result;
    }

    private void AutoComplete(TrieNode node, string temp, List<string> suffixes)
    {
        if (node.Terminal)
        {
            suffixes.Add(temp);
        }

        foreach (var link in node.Links)
        {
            AutoComplete(link.Value, temp + link.Value.Key, suffixes);
        }
    }
```

As evidenced by the simplicity of the algorithm, this task is very easy,
and the reason it's easy is because the trie structure makes it so. I
cannot stress enough that choosing good data structures is the
difference between pulling your hair out and cranking out solutions
without a care in the world. The above methods I got right on the first
try, whereas if there weren't a suitable data structure involved, it
would have taken quite a bit longer. In fact, all of the methods and
classes in this tutorial were completed on the first attempt. Basic
tries are shockingly simple for how efficient they are.

#### Deletion

As with every other tree-based data structure, most resources
disturbingly fail to include the deletion algorithm. This creates no
lack of irritation for me, so I'm very careful to show you how to delete
these things too. With the above trie, there is no simple top-down
deletion algorithm, so we're stuck with either recursion or a
recursive-like approach using stacks.

The process is conceptually simple. Search the trie for a matching
terminal, then walk back up and remove nodes. However, this process is
hindered by the need to maintain the structure. Since we only want to
delete the matching chain, any suffix chains must be preserved. We do
this by only deleting the child chain of a node iff it has no children
of its own *and* is non-terminal. The latter requirement ensures that we
stop trying to delete when the terminal node for a shared chain is
found.

One special case exists for 'deleting' a chain where no nodes are
physically removed. This is actually the simple case of deletion, and
the only thing that needs to be done is set the node as non-terminal.
The structure of the trie does not change, but any future searches for
that particular item will fail due to not matching a terminal node. Here
is a recursive implementation:

```csharp
    public void Remove(string s)
    {
        Remove(Root, s);
    }

    private bool Remove(TrieNode root, string s)
    {
        if (string.IsNullOrEmpty(s))
        {
            // Base case: check for a terminal
            if (root.Terminal)
            {
                // Always remove the terminal flag
                root.Terminal = false;

                return root.Links.Count == 0;
            }

            return false;
        }
        else
        {
            if (root.Links.ContainsKey(s[0]))
            {
                // Found a matching suffix, recurse down
                if (Remove(root.Links[s[0]], s.Substring(1)))
                {
                    // The previous recursive step says we can remove the child node
                    root.Links.Remove(s[0]);

                    return root.Links.Count == 0 && root.Terminal == false;
                }
            }

            return false;
        }
    }
```

The requirement of recursion or simulated recursion smells funny.
Despite tries typically being well balanced, that is not a hard
guarantee. If the chains are exceptionally long, there is still a risk
of stack overflow from the recursive algorithm. Ideally we would use an
iterative approach. More importantly is that while the code is short,
the logic is not exactly transparent. It turns out that with a parent
pointer, the deletion algorithm becomes much clearer, and is actually
comparable in size:

```csharp
    public void Remove(string s)
    {
        var it = Root;

        foreach (var c in s)
        {
            if (!it.Links.ContainsKey(c))
            {
                return;
            }

            it = it.Links[c];
        }

        // Don't remove anything if the search
        // failed to match a complete chain.
        if (it.Terminal)
        {
            // Always remove the terminal, because we might
            // not actually remove this node due to subsequent
            // children.
            it.Terminal = false;

            // Walk back up looking for nodes to remove
            while (!it.Terminal && it.Links.Count == 0 && it.Parent != null)
            {
                it.Parent.Links.Remove(it.Key);
                it = it.Parent;
            }
        }
    }
```

Only minor changes are required to support parent pointers, as you'll
see in our final code block of the tutorial. Here is the complete `Trie`
class using parent pointers:

```csharp
    public class TrieNode
    {
        public char Key { get; set; }
        public bool Terminal { get; set; }
        public TrieNode Parent { get; private set; }
        public Dictionary<char, TrieNode> Links { get; private set; }

        public TrieNode(TrieNode parent, char key, bool terminal = false)
        {
            Parent = parent;
            Key = key;
            Terminal = terminal;
            Links = new Dictionary<char, TrieNode>();
        }
    }

    public class Trie
    {
        private TrieNode Root { get; set; }

        public Trie()
        {
            // Create a dummy root for simplicity
            Root = new TrieNode(null, default(char));
        }

        public void Add(string s)
        {
            if (string.IsNullOrEmpty(s))
            {
                return;
            }

            var it = Root;

            foreach (var c in s)
            {
                if (!it.Links.ContainsKey(c))
                {
                    // No more nodes, extend the chain
                    it.Links.Add(c, new TrieNode(it, c));
                }

                it = it.Links[c];
            }

            // No more characters, ensure a terminal
            it.Terminal = true;
        }

        public void Remove(string s)
        {
            var it = Root;

            foreach (var c in s)
            {
                if (!it.Links.ContainsKey(c))
                {
                    return;
                }

                it = it.Links[c];
            }

            // Don't remove anything if the search
            // failed to match a complete chain.
            if (it.Terminal)
            {
                // Always remove the terminal, because we might
                // not actually remove this node due to subsequent
                // children.
                it.Terminal = false;

                // Walk back up looking for nodes to remove
                while (!it.Terminal && it.Links.Count == 0 && it.Parent != null)
                {
                    it.Parent.Links.Remove(it.Key);
                    it = it.Parent;
                }
            }
        }

        public bool Match(string s)
        {
            var it = Root;

            foreach (var c in s)
            {
                if (!it.Links.ContainsKey(c))
                {
                    // No more nodes, match failure
                    return false;
                }

                it = it.Links[c];
            }

            // No more characters, check for a terminal
            return it.Terminal;
        }

        #region Auto-completion
        public IList<string> AutoComplete(string s)
        {
            var result = new List<string>();
            var it = Root;

            foreach (var c in s)
            {
                if (!it.Links.ContainsKey(c))
                {
                    return new List<string>();
                }

                it = it.Links[c];
            }

            // Generate all possible suffixes
            AutoComplete(it, string.Empty, result);

            return result;
        }

        private void AutoComplete(TrieNode node, string temp, List<string> suffixes)
        {
            if (node.Terminal)
            {
                suffixes.Add(temp);
            }

            foreach (var link in node.Links)
            {
                AutoComplete(link.Value, temp + link.Value.Key, suffixes);
            }
        }
        #endregion

        #region Structure
        public void Print()
        {
            Console.WriteLine("(root)");
            Print(Root, 0);
        }

        private void Print(TrieNode root, int level)
        {
            Console.WriteLine(new string('\t', level) + root.Key + (root.Terminal ? "!" : ""));

            foreach (var link in root.Links)
            {
                Print(link.Value, level + 1);
            }
        }
        #endregion
    }
```

#### Performance

Tries span the gap between binary search trees and hash tables. On
average a search is faster for a trie than for a binary search tree,
being O(m) where m is the length of the key. This is on average slower
than a hash table where the search itself is O(1) with an O(m) hash
execution. However, hash tables depend heavily on the quality of the
hash function, which is not necessary in a trie. Further, tries are
sorted by definition like binary search trees, and unlike hash tables.

Construction of a trie has O(Nm) performance, where N is the number of
items and m is the length of the largest item (see the article on
asymptotic notation for why the largest item is used). Insertion and
deletion are both also O(m).

When it comes to space efficiency, basic tries are the worst of the trie
variants. Other variants will typically merge chains that consist of
only one link, such as the compressed trie or PATRICIA trie.

#### Conclusion

This is among my shorter tutorials, mostly due to the simplicity of the
data structure. However, there is a lot of potential for optimization,
which gives life to many variations of the trie. I can be long winded in
covering all of the bases, but tried to show restraint in this tutorial.
Further tutorials on tries will cover the more popular variations. But
this isn't to say that the basic trie is not useful. In practice, I
prefer to use the basic version over variations unless there is a
pressing need to complicate the code with optimizations.

-----

© 2018 - Eternally Confuzzled
