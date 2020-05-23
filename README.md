This repository contains some useful public-domain libraries by
Julienne Walker, which I rescued from her now-defunct site
[eternallyconfuzzled.com][1].

I don't have anything to do with these libraries; I just like them and
didn't want to see them get lost.  I felt that putting them in a
Github repository would make them more accessible than having to go
spelunking for them in the [Wayback Machine][2].  Also, pull requests
can be accepted if any bugs are ever found.

In addition, I rescued the [tutorials](Tutorials) and
[articles](Articles) from Eternally Confuzzled, converted them to
[Markdown][3] using [Pandoc][4], and placed them in this repository,
as well.

## Tutorials

### Data Structures

| Tutorial                       | Library                      |
| ------------------------------ | ---------------------------- |
| [Andersson Trees][andersson]   | [jsw\_atree](jsw\_atree)     |
| [AVL Trees][avl]               | [jsw\_avltree](jsw\_avltree) |
| [Basic Tries][trie1]           |                              |
| [Binary Search Trees I][bst1]  |                              |
| [Binary Search Trees II][bst2] |                              |
| [Hash Tables][hashtable]       | [jsw\_hlib](jsw\_hlib)       |
| [Linked Lists][linklist]       |                              |
| [Red Black Trees][rbtree]      | [jsw\_rbtree](jsw\_rbtree)   |
| [Skip Lists][skip]             | [jsw\_slib](jsw\_slib)       |

### Algorithms

* [Hashing][hashing]
* [Random Numbers][rand]
* [Sorting][sorting]

### Miscellaneous

* [Pointers][pointers]
* [Windows Services Simplified][winservice]

## Articles

* [Asymptotic Notation][bigo]
* [atoi() is evil!][atoi]
* [Using rand()][rand]

## Licensing

Both the libraries and the articles on Eternally Confuzzled were in
the public domain, as stated [here][1].  (Despite the contradictory
"© 2018 - Eternally Confuzzled" at the bottom of each page.)

[1]: https://web.archive.org/web/20180225130248/http://www.eternallyconfuzzled.com/jsw_home.aspx
[2]: https://en.wikipedia.org/wiki/Wayback_Machine
[3]: https://github.github.com/gfm/
[4]: https://pandoc.org/

[avl]: Tutorials/jsw_tut_avl.md
[andersson]: Tutorials/jsw_tut_andersson.md
[trie1]: Tutorials/jsw_tut_trie1.md
[bst1]: Tutorials/jsw_tut_bst1.md
[bst2]: Tutorials/jsw_tut_bst2.md
[hashtable]: Tutorials/jsw_tut_hashtable.md
[linklist]: Tutorials/jsw_tut_linklist.md
[rbtree]: Tutorials/jsw_tut_rbtree.md
[skip]: Tutorials/jsw_tut_skip.md
[hashing]: Tutorials/jsw_tut_hashing.md
[rand]: Tutorials/jsw_tut_rand.md
[sorting]: Tutorials/jsw_tut_sorting.md
[pointers]: Tutorials/jsw_tut_pointers.md
[winservice]: Tutorials/jsw_tut_winservice.md
[bigo]: Articles/jsw_art_bigo.md
[rand]: Articles/jsw_art_rand.md
[atoi]: Articles/jsw_art_atoi.md
