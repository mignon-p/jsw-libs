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

| Tutorial                           | Library                      |
| ---------------------------------- | ---------------------------- |
| [Andersson Trees][dst-andersson]   | [jsw\_atree](jsw\_atree)     |
| [AVL Trees][dst-avl]               | [jsw\_avltree](jsw\_avltree) |
| [Basic Tries][dst-trie1]           |                              |
| [Binary Search Trees I][dst-bst1]  |                              |
| [Binary Search Trees II][dst-bst2] |                              |
| [Hash Tables][dst-hashtable]       | [jsw\_hlib](jsw\_hlib)       |
| [Linked Lists][dst-linklist]       |                              |
| [Red Black Trees][dst-rbtree]      | [jsw\_rbtree](jsw\_rbtree)   |
| [Skip Lists][dst-skip]             | [jsw\_slib](jsw\_slib)       |

### Algorithms

| Tutorial                           | Library                      |
| ---------------------------------- | ---------------------------- |
| [Hashing][alg-hashing]             |                              |
| [Random Numbers][alg-rand]         | [jsw\_rand](jsw\_rand)       |
| [Sorting][alg-sorting]             |                              |

### Miscellaneous

* [Pointers][msc-pointers]
* [Windows Services Simplified][msc-winservice]

## Articles

* [Asymptotic Notation][art-bigo]
* [atoi() is evil!][art-atoi]
* [Using rand()][art-rand]

## Licensing

Both the libraries and the articles on Eternally Confuzzled were in
the public domain, as stated [here][1].  (Despite the contradictory
"© 2018 - Eternally Confuzzled" at the bottom of each page.)

## Tests

I (Patrick Pelletier) have added some tests for the jsw libraries.  To
run the tests, just run the script `test/run-tests.pl`.  You will need
to have Perl, gcc, and [valgrind][5] installed to run the tests.

[1]: https://web.archive.org/web/20180225130248/http://www.eternallyconfuzzled.com/jsw_home.aspx
[2]: https://en.wikipedia.org/wiki/Wayback_Machine
[3]: https://github.github.com/gfm/
[4]: https://pandoc.org/
[5]: https://valgrind.org/

[dst-avl]: Tutorials/jsw_tut_avl.md
[dst-andersson]: Tutorials/jsw_tut_andersson.md
[dst-trie1]: Tutorials/jsw_tut_trie1.md
[dst-bst1]: Tutorials/jsw_tut_bst1.md
[dst-bst2]: Tutorials/jsw_tut_bst2.md
[dst-hashtable]: Tutorials/jsw_tut_hashtable.md
[dst-linklist]: Tutorials/jsw_tut_linklist.md
[dst-rbtree]: Tutorials/jsw_tut_rbtree.md
[dst-skip]: Tutorials/jsw_tut_skip.md
[alg-hashing]: Tutorials/jsw_tut_hashing.md
[alg-rand]: Tutorials/jsw_tut_rand.md
[alg-sorting]: Tutorials/jsw_tut_sorting.md
[msc-pointers]: Tutorials/jsw_tut_pointers.md
[msc-winservice]: Tutorials/jsw_tut_winservice.md
[art-bigo]: Articles/jsw_art_bigo.md
[art-rand]: Articles/jsw_art_rand.md
[art-atoi]: Articles/jsw_art_atoi.md
