# Pointers

  

Most newcomers to programming have trouble with the concept of pointers.
This is almost entirely due to the reputation that pointers have for
being difficult, and they are approached with a feeling of apprehension.
That feeling is completely unfounded, as pointers are really very
simple. However, pointers are also very powerful, and the potential for
abuse and very difficult to find errors exists. It is this potential
that has earned pointers the bad reputation that they have. In this
tutorial, I will attempt to describe pointers in as much of their
entirety as reasonable.

#### Memory addressing (abstract)

Memory in a computer system is basically just an array of addresses.
Each address typically represents a single byte (8 bits, also called an
octet, on many systems). This is called byte-addressable memory, and
each address has an offset, or index, to represent its location in the
physical memory space. The total addressable memory is determined by the
address bus size of the of the processor, which has a limit of
2<sup>n</sup> bytes.

Put simply, memory can be viewed as an array of unsigned char, where
unsigned char represents a byte. On the newer Pentium processors (Pro,
II, III, and IV), the address bus supports 36 bits, so the array could
be represented in C as:

    #define ADDRESS_BUS_SIZE 36 /* Pentium 4: 36-bit address bus */
    unsigned char memory[1 << ADDRESS_BUS_SIZE];

Naturally, this code is unlikely to actually run, but if you know how to
use arrays, it gives you an idea of how addresses in memory work. Now,
this is only part of the concept of memory. Even though most systems are
byte-addressable, it makes sense for the processor to move as much data
around as possible. This is done by the data bus, and the size of the
data bus is where the names 8-bit system, 16-bit system, 32-bit system,
64-bit system, etc.. come from. When the data bus is 8 bits wide, it can
transfer 8 bits in a single memory operation. When the data bus is 32
bits wide (as is most common at the time of writing), at most, 32 bits
can be moved in a single memory operation.

This is a good thing because now larger values can be loaded from memory
in the same amount of time that it takes to load a byte on an 8-bit data
bus. However, it introduces the problem of alignment. While some systems
go out of their way to guarantee that a byte-addressable system can load
a multi-byte value from any arbitrary address, many do not, and even
those that do have performance penalties for accessing a byte with an
unexpected alignment. Consider this example where memory is divided into
two byte integers:

    #include <stdio.h>
    #include <string.h>
    
    #define ADDRESS_BUS_SIZE 20 /* 2^20 address bus */
    unsigned char memory[1 << ADDRESS_BUS_SIZE];
    
    int main ( void )
    {
      int i;
      unsigned short val = 12345;
    
      for ( i = 0x12340; i < 0x1234A; i += 2 ) {
        memcpy ( &memory[i], &val, sizeof val );
        ++val;
      }
    
      val = 0;
    
      for ( i = 0x12340; i < 0x1234A; i += 2 ) {
        memcpy ( &val, &memory[i], sizeof val );
        printf ( "%hu\n", val );
      }
    
      return 0;
    }

This naturally assumes that unsigned short is two bytes, but at the time
of writing, this is a fairly safe assumption. Looking at the memory as
two byte pairs, it is fairly easy to see that problems can occur when
odd addresses are accessed directly. While such an operation may be
legal depending on the processor, it may take extra instructions to
disambiguate the request from the more common even-address operation:

``` 
 Byte1    Byte2
-----------------
0x12340 | 0x12341 /* Contains 12345 */
0x12342 | 0x12343 /* Contains 12346 */
0x12344 | 0x12345 /* Contains 12347 */
0x12346 | 0x12347 /* Contains 12348 */
0x12348 | 0x12349 /* Contains 12349 */
```

Let us say that a program wants to read an unsigned short from address
**0x12343**. If the data bus always places even addresses in **Byte1**,
and odd addresses in **Byte2**, a request for a starting byte at the odd
address would require the processor to first read **0x12343** into
**Byte2**, then **0x12344** into **Byte1**. Because this is the inverse
of what is wanted, the processor must then swap the addresses (or the
contents of the addresses). As expected, this takes more time than a
memory request from an even address.

This is what people mean when they talk about alignment. It's not
directly relevant to a discussion of pointers, but an understanding of
how memory works simplifies pointer arithmetic.

#### Concept of pointers (abstract)

Pointers are variables that hold an address. That's it\! The secret
behind the infamous pointer is nothing more than “variables that hold an
address”. Given the collective dread that programmers seem to attribute
to the complexity of pointers, the reality is somewhat anticlimactic.

When we speak of a pointer, we really mean a variable that holds an
address. When we talk about a pointer “pointing to” something, we mean
whatever object resides at that address. To “dereference” a pointer is
to retrieve the value of the object at the address.

Because a pointer is a variable, there are actually four parts to it.
The direct value of a pointer is an address. The indirect value of a
pointer is the value at that address. The direct address of a pointer is
the address of itself, and the indirect address of a pointer is the same
as the value of the pointer. So a pointer has an address, and a pointer
holds an address. Therefore, a pointer can point to another pointer. The
logic may seem circular, but it makes sense when you think about it for
a bit.

#### What are pointers?

A pointer is simply a variable that refers to another object in memory.
In other words, a pointer is a variable that contains a memory address,
much like an integer is a variable that contains a whole number. Once
this single fact is understood, the entire concept becomes much more
manageable. However, because a pointer contains a memory address, it
actually has *two* values, unlike the integer. The first value is the
address itself; this is the *direct* value of a pointer. The second
value is the contents of the address; this is the *indirect* value of a
pointer. Assume that we have a pointer p (represented as an index) that
contains an address **0x12340**. Using the array example, it might look
like this:

    p = 0x12340;
    memory[p] = 212;

Notice that the immediate value of **p** is the address, **0x12340**,
but through **p** we can assign **212** to the integer at that address.
The act of accessing memory through a pointer is called indirection
(accessing the indirect value), or dereferencing. At first glance, this
entire concept may seem overly complicated for no gain. If you have
access to the original variable, what need is there for a pointer to its
address? Well, sometimes you do not have access to the original variable
*except* through a pointer. For example, in C, arguments are passed to a
function by value, meaning that a copy of the variable's value is made
and assigned to a new variable. Under this scheme, there is no way to
change the value of the original variable.

However, if you pass a pointer to the variable by value, a copy of the
address is made instead of the value of the variable, and you can use
indirection to access the original variable's contents. This
functionality is very consistent with pointers. If you need to change
the address of a pointer when you do not have access to the original
pointer, a pointer to that pointer can be used:

    p1 = 0x12340;
    memory[p1] = 0x12342;
    memory[memory[p1]] = 212;

This example is somewhat convoluted, but the key to understanding it is
to notice that **p1** is a pointer, but so is **memory\[p1\]**. This is
an example of double indirection, accessing memory through two pointers.
The first pointer, **p1**, refers to the second pointer,
**memory\[p1\]**, which then refers to the address of an integer where
**212** can be assigned. Single and double indirection are the most
common operations, though occasionally you might see more levels of
indirection. Fortunately, the concept is scalable and consistent to
however many levels of indirection you might need.

#### Uses of pointers (abstract)

The myriad ways that pointers can be used is overwhelming at first.
However, they can be broken down into a few broad categories of common
use:

1\) Creating pass-by-reference semantics

In languages such as C, where arguments to a function are passed only by
value, a copy of the value is made and assigned to a new variable. Under
such a scheme, it is impossible to change the value of the original
object from within a function because the function body only sees a copy
of it. For example (using a fictional programming language):

    function increment: x
    begin
      x := x + 1
    end
    
    program
    begin
      i := 0
    
      while i < 10 do
        increment ( i )
        print i
      loop
    end

This will print **0** infinitely (or until some outside power stops the
program). The reason is because **i** is not actually passed to
increment, its value is. The value is then copied into **x**. Inside
**increment**, **x** is incremented by one and then promptly destroyed
when the function returns. Back in the main program, **i** still has the
value **0** because a completely separate variable was incremented.
Removing all of the hidden framework, the example would look like this:

    program
    begin
      i := 0
    
      while i < 10 do
        x := i
        x := x + 1
        print i
      loop
    end

The error is more obvious with this example. To fix the problem in a
language that only supports pass-by-value semantics, you must pass the
address of the object by value. A copy of the address is made, but
because the address is still that of the original object, it can be
dereferenced and the original value can be changed. The example could be
corrected like so:

    function increment: pointer x
    begin
      deref x := deref x + 1
    end
    
    program
    begin
      i := 0
    
      while i < 10 do
        increment ( addressof i )
        print i
      loop
    end

2\) Walking through memory

With a pointer to an address, it makes sense that you might want to move
forward to the next address or backward to the previous address. Using
pointer arithmetic, this is possible, and often useful. Most of the
time, this technique is used to walk across the elements of an array.

3\) Referring to dynamic memory

Because dynamic memory is not bound to a variable, there is no way of
accessing it without some form of reference to the memory. This is an
ideal use for pointers. Simply assign the beginning address of the
memory block to a pointer, and use dereferencing to access that memory.
This technique is used very often in both C and C++ for such tasks as
creating arrays that grow and shrink on demand.

4\) Creating linked data structures

While arrays are simple to use, they are not well suited to operations
that require insertions or removals from anywhere but the end. This is
because to make a hole for insertions, or to remove a hole for removals,
a potentially expensive shifting of elements is required. Pointers can
be used to link together non-contiguous blocks of memory so that they
can be treated as if they were all adjacent.

#### Declaration and syntax

To declare a pointer, simply place an asterisk between the type and
identifier of a variable. Any number of levels of indirection can be
created by adding more asterisks:

    int *i;    /* Pointer to int */
    double *d; /* Pointer to double */
    char **p;  /* Pointer to pointer to char */

Multiple pointers can be declared on the same line, but care must be
taken to use the asterisk for every variable:

    int *p, *q;

If the asterisk is omitted, then the variable without an asterisk is not
a pointer:

    int *p, q; /* p is a pointer, but q is not! */

A pointer can have two levels of constness. The first level is for the
object being pointed to, and in such a case, the keyword const will need
to be inserted on the left side of the asterisk. As usual, const can be
on either side of the type:

    const int *i; /* Pointer to const int */
    int const *j; /* Pointer to const int */

When a pointer points to a const type, the pointer can be changed, but
the value pointed to cannot. To create a const pointer, place the const
keyword on the right side of the asterisk, but before the identifier:

    int * const i; /* Const pointer to non-const int */

These two levels can be combined to create a const pointer to const:

    const int * const i; /* Const pointer to const int */

The two operators needed for working with a pointer are the indirection
operator, **\***, and the address-of operator, **&**. The address-of
operator is a unary operator that returns the address of an object that
has an address. It basically “makes a pointer” out of an object by
evaluating to an address that can be assigned to a pointer:

    int i;
    int *p;
    
    p = &i; /* p now "points to" i */

The indirection operator does just that; it dereferences a pointer.
Provided the pointer has been assigned an address, indirection will
return the expected value:

    int i = 123;
    int *p = &i;
    
    printf ( "%d\n", *p ); /* Prints 123 */

A pointer in C and C++ can have one of three values. First, a pointer
can point to the address of an object. This is the most common value.
Second, a pointer can point to nothing. This is the so called, “null”
pointer. Last, a pointer can point to a random address. This is the
initial state of a local pointer, and is the cause of many problems for
beginners. If a pointer points to a random address, such as by declaring
the pointer but not intializing it, and then dereferences it, the
behavior of the program will be unpredictable. Therefore, it is critical
that a pointer be initialized before it is used.

Here is a complete example that shows these operations in action:

    #include <stdio.h>
    
    int main ( void )
    {
      int i = 123;
      int j = 321;
      int *p = &i;
      int *q;
    
      printf ( "i is %d\n", i );
      printf ( "*p is %d\n\n", *p );
    
      q = p; /* q points to p's value, which is i */
    
      printf ( "i is %d\n", i );
      printf ( "*p is %d\n", *p );
      printf ( "*q is %d\n\n", *q );
    
      /* Increment i three times */
      ++*p;
      ++*q;
      ++i;
    
      printf ( "i is %d\n", i );
      printf ( "*p is %d\n", *p );
      printf ( "*q is %d\n\n", *q );
    
      /* Reassign p */
      p = &j;
    
      printf ( "i is %d\n", i );
      printf ( "*p is %d\n", *p );
      printf ( "*q is %d\n\n", *q );
    
      return 0;
    }

#### Types of pointers

Astute readers may have noticed that despite the variable being a
pointer, it still has a type. Every pointer in C and C++ must have a
type. The type of the pointer determines what operations are allowed,
and how those operations are carried out.

#### Null pointers

The first type is actually a value, the null pointer that was mentioned
earlier. A null pointer is created by assigning the integer literal
**0** to the pointer. This does not actually cause the pointer to point
to the address 0x0, despite the common misconception that the bit
pattern of a null pointer is all bits zero. It is considered good style
to initialize all pointers to either an object's address, or null when
first declared:

    int i;
    
    int *p = &i;  /* Points to i */
    float *q = 0; /* Null pointer */

For your convenience, several headers define a macro called **NULL**
that correctly defines a null pointer value. The most common headers for
this macro are **stdio.h**, **stdlib.h**, and **stddef.h**. (the C++
headers would be **cstdio**, **cstdlib**, and **cstddef**). Using
**NULL**, it is well understood that a null pointer is intended:

    #include <stddef.h>
    
    float *q = NULL;

As expected, a null pointer cannot be dereferenced or bad things will
happen. Unpredictable bad things. But always bad things. No good can
come from dereferencing a null pointer just as no good can come from
dereferencing an uninitialized pointer.

#### Pointers to basic types

A pointer can point to any of the built-in “simple” types, such as
**int**, **char**, **double**, **float**, **long**, and **short**.
Pointers differentiate between these types because any movement of the
pointer will take into account the size of the object being pointed to,
and scale the movement accordingly. So if an int is four bytes, moving a
pointer to int forward in memory by two will actually move the pointer
by eight bytes instead of two. Likewise, a pointer to **char** would
move forward by two bytes because the size of a **char** is always
**1**.

#### Pointers to void

In order to fulfill the obvious need of a generic pointer, the pointer
to **void** was introduced. A pointer to **void** cannot be
dereferenced, but it makes for a convenient (in C) way to move around
pointers that could be of any type. As an example, the standard library
function **malloc** returns a block of suitably aligned memory in the
form of a pointer. However, because **malloc** cannot know what type of
pointer is required, a pointer to **void** is returned instead.

Pointers to **void** are generally not used as much in C++ because
other, less awkward solutions exist. In C, however, the pointer to
**void** is the simplest way to create and maintain a heterogeneous data
structure. Pointers to **void** are move convenient in C because a cast
not required to convert to and from a pointer to **void**. For example:

    int *p = malloc ( 100 * sizeof *p ); /* Allocate 100 integers */

In C++, this line of code would need to include a cast to **int\***:

    int *p = static_cast<int*> ( malloc ( 100 * sizeof *p ) );

#### Pointers to user-defined types

A pointer to structure or class creates a new problem. Because
user-defined data types allow for member access using the **.**
operator, and the member access operator has higher precedence than the
indirection operator, the only way to access a member of a user-defined
type through a pointer is to surround the indirection with parentheses
and then apply the member access operator to the result:

    struct s {
      int i;
    };
    
    struct s t;
    struct s *p = &t;
    
    (*p).i = 123;

Fortunately, the designers of C realized how awkward this is and added a
special member access operator for pointers to user-defined types. The
so called “arrow” operator is this addition:

    struct s {
      int i;
    };
    
    struct s t;
    struct s *p = &t;
    
    p->i = 123;

#### Pointers to pointers

As mentioned several times before, a pointer can point to another
pointer because a pointer is just another variable with an address. The
syntax is consistent and intuitive:

    int i = 123;
    int *p = &i;
    int **pp = &p;
    
    printf ( "%d\n", **pp );

#### Pointers to functions

Many people are surprised to learn that functions have an address.
Therefore, it is possible to have a pointer to a function. This is
incredibly useful for providing callback functions where a given
function performs an operation that the client code defines. A standard
example is **qsort** in the standard C library header **stdlib.h**:

    #include <stdio.h>
    #include <stdlib.h>
    
    #define length(x) ( sizeof ( x ) / sizeof *( x ) )
    
    int compare ( const void *a, const void *b )
    {
      const int *ia = a;
      const int *ib = b;
    
      if ( *ia < *ib )
        return -1;
      else if ( *ia > *ib )
        return +1;
      else
        return 0;
    }
    
    int main ( void )
    {
      int a[] = {9,8,7,6,5,4,3,2,1,0};
      int i;
    
      for ( i = 0; i < length ( a ); i++ )
        printf ( "%-2d", a[i] );
      printf ( "\n" );
    
      qsort ( a, length ( a ), sizeof a[0], compare );
    
      for ( i = 0; i < length ( a ); i++ )
        printf ( "%-2d", a[i] );
      printf ( "\n" );
    
      return 0;
    }

Notice that in the call to **qsort**, we are passing the name of a
function. The address-of operator is not required because the compiler
can deduce whether or not the function is being called, and if a
function is not being called then the only other valid operation that
can be performed on it is taking its address. Passing a function to
handle some application specific detail is a very useful trick. Because
**qsort** only works with pointers to **void**, it cannot know what
types are actually being sorted. Therefore, it uses a callback
comparison function that handles this need.

A pointer to function is declared just like a regular function except an
asterisk precedes the function name, and both the name and asterisk must
be surrounded with parentheses:

    void (*fp) ( int );

In this example, **fp** is a variable. It is a pointer to a function
that takes an integer argument and returns no value. Without the
parentheses, around **\*fp**, it would be a declaration of the function
**fp** that takes an integer argument and returns a pointer to **void**.
This is a big difference, so take care in placing the parentheses when
declaring a pointer to function.

#### Pointers to arrays

A pointer can point to an array, but the syntax is awkward enough that
this feature is not often used. To create a pointer to an array, simply
declare an array and then, just as with a pointer to a function, add an
asterisk and surround both the name and the asterisk with parentheses:

    int (*pa)[10];

This example declares a pointer to an array of ten integers. The
awkwardness comes from trying to subscript a pointer to an array, where
the same problem as with pointers to user-defined types rears its head
again. Unfortunately, this time there is no operator to save the day:

    int a[5] = {1,2,3,4,5};
    int (*pa)[5] = &a;
    
    printf ( "%d\n", (*pa)[2] ); /* Prints 3 */

#### Pointers to members (C++ only)

A pointer to member is a way to indirectly access a member through an
object or pointer to object. In other words, you can use a pointer to
member to access a member only knowing its type. The syntax is esoteric,
at best:

    #include <iostream>
    
    struct s {
      int i;
      void f() { std::cout<<"i is "<< i <<'\n'; }
    };
    
    int main()
    {
      s a, b;
    
      int s::*pi = &s::i;
      void (s::*pf)() = &s::f;
    
      a.*pi = 10;
      b.*pi = 20;
    
      (a.*pf)();
      (b.*pf)();
    }

A more realistic use is printing the contents of a **map**. Since the
value of a **map** element is a **pair**, you might want to print a
columnar list with the first value on each column of a row and the
second value on the same column of the next row. A clever programmer
would factor this out into a single function using a pointer to member:

    #include <iomanip>
    #include <iostream>
    #include <map>
    #include <utility>
    
    void print ( std::map<int, int>& table, int std::pair<int, int>::*item )
    {
      std::map<int, int>::iterator it = table.begin();
    
      while ( it != table.end() ) {
        std::cout<< std::left << std::setw ( 3 ) << ( (std::pair<int, int>)*it ).*item;
        ++it;
      }
    }
    
    int main()
    {
      std::map<int, int> table;
    
      for ( int i = 0; i < 10; i++ )
        table[i] = i * 10;
    
      print ( table, &std::pair<int, int>::first );
      std::cout<<'\n';
      print ( table, &std::pair<int, int>::second );
      std::cout<<'\n';
    }

Now instead of maintaining two loops, or two functions, or something
worse, the programmer only needs to control the formatting from a single
loop. However, this example is filled with complicated features, and it
is somewhat brittle. In my experience, pointers to members are not used
often. I have yet to see them used in any production code.

#### Pointer arithmetic

Arithmetic on pointers is very simple, you can add an integer to a
pointer, subtract an integer from a pointer, and subtract a pointer from
a pointer. These operations are only valid on arrays, or blocks of
contiguous memory owned by the program. Anything else would be accessing
memory outside of the program's address space and would then be subject
to unpredictable behavior.

Adding an integer to a pointer causes the pointer to move “forward”,
toward the end of the array. You can add any integral value to a pointer
as long as it does not exceed the boundaries of the array:

    #include <stdio.h>
    
    int main ( void )
    {
      int a[] = {1,2,3,4,5};
      int *p;
    
      for ( p = a; p != a + 5; p++ )
        printf ( "%d\n", *p );
    
      return 0;
    }

This example introduces three important concepts. The first, that the
address-of operator is not required to assign the address of an array to
a pointer, will be discussed later. The second concept is that of a
past-the-end address. C and C++ guarantee that every array allocates an
address past the end that can be used for comparison. Note that you can
compare with the address, but the value of the past-the-end element is
off limits in all cases. The last important concept is that pointers can
be compared with the relational operators, but the details will be
provided later. The next example does the same thing as the previous
example except it increments the pointer by two instead of one:

    #include <stdio.h>
    
    int main ( void )
    {
      int a[] = {1,2,3,4,5, 6};
      int *p;
    
      for ( p = a; p != a + 5; p += 2 )
        printf ( "%d\n", *p );
    
      return 0;
    }

Notice that an even number of elements is used because incrementing the
pointer by two would exceed the boundaries of the array. The comparison
with the past-the-end element would never be made, and an infinite loop
would be created as a result.

Subtraction of an integer from a pointer works exactly the same as
addition. The only difference is that there is no past-the-end element
for the front of an array. Use caution when walking backward in an array
with a pointer. The interesting part of subtraction comes when a pointer
is subtracted from a pointer. Both pointers must point to elements
within the same array. The result of subtraction is the distance between
the two pointers. The result can be either negative or positive
depending on the relationship of the pointers:

    #include <stdio.h>
    
    int main ( void )
    {
      int a[] = {1,2,3,4,5};
      int *p = a, *q = a + 5;
    
      printf ( "%td\n", p - q );
      printf ( "%td\n", q - p );
    
      return 0;
    }

The result type of pointer subtraction is **ptrdiff\_t**, a typedef for
a signed integral value. Note that most C compilers at the time of
writing will not support the type specifier for printf that correctly
prints a **ptrdiff\_t** value (it was added in the C99 standard). On
those compilers, a hack to force the value to the largest possible
signed integer is required:

    #include <stdio.h>
    
    int main ( void )
    {
      int a[] = {1,2,3,4,5};
      int *p = a, *q = a + 5;
    
      printf ( "%ld\n", (long)( p - q ) );
      printf ( "%ld\n", (long)( q - p ) );
    
      return 0;
    }

#### Pointer comparisons

A pointer comparison only compares the direct value of two pointers.
Pointers can only be compared if they point to the same array;
otherwise, the relative position of the pointers is irrelevant. However,
tests for equality using **==** and **\!=** are always legal, because a
pointer either points to the same location in memory, or it does not.

#### What are references? (C++ only)

A reference in C++ is another name for an object. Because it's a
synonym, it has to have something to be a synonym for, therefore a
reference must always refer to an object. At first glance, it might seem
like references are a cleaner version of pointers, where you do not have
to use the address-of operator or dereference the resulting reference,
but in reality there are subtle differences.

#### Uses of references (abstract)

References were designed almost entirely to support operator
overloading. Therefore, the best use of them is to replace pointers in
achieving pass-by-reference semantics. The general recommendation for
C++ is to use references when you can, and pointers when you must. One
of the most important issues with references is that once bound to an
object, it cannot be rebound to refer to another object. There is also
no such thing as a null reference, so if either of those features are
needed, a pointer must be used instead of a reference.

#### Declaration and syntax

A reference is, confusingly, declared much like a pointer, except using
an ampersand instead of an asterisk. This is confusing because the
ampersand is also used as the address-of operator with pointers. A
reference must be initialized, therefore there is no such thing as an
uninitialized reference:

    #include <iostream>
    
    int main()
    {
      int i = 123;
      int& s = i;
    
      std::cout<< s <<'\n';
    }

Because a reference is just another name for the object it refers to,
there are no operations on references, per se. This makes using
references very simple because you can just treat them like the type
they refer to.

#### Types of references

As can be expected, because a reference to T must act like a T, it is
required that a reference have a type. The good news is that the syntax
for declaring references is nearly identical to the syntax for declaring
pointers.

#### References to basic types

The syntax for a reference to a built-in simple type should be obvious:

    int i;
    double d;
    char c;
    int& r = i;
    double& s = d;
    char& t = c;

#### References to user-defined types

Unlike with pointers, references to user-defined type can use the member
access operator and do not need a special operator, as pointers need the
arrow operator:

    struct s {
      int i;
    };
    
    struct s t;
    struct s& r = t;
    
    r.i = 123;

#### References to pointers

Because the type of a reference must be on the left side of the
ampersand in a reference declaration, and the asterisk of a pointer is a
part of the type, the declaration for a reference to a pointer might
look a little funny:

    char *p;
    char*& r = p;

This is logical, but not immediately obvious at first glance. It helps
to read the declaration from right to left. When you do that, it makes
more sense: **r** is a reference to a pointer to **char**. References to
pointers eliminate many needs for pointers to pointers. In fact, many
C++ programmers have little or no experience with pointers to pointers,
much to the surprise of C programmers, who use them regularly. Pointers
to references are not legal, nor are references to references (at the
time of writing, though this is the topic of a defect report and will
likely be changed in the next revision of the C++ standard).

#### References to functions

While references to functions are legal, they are almost never used
because they have no benefit over pointers to functions. To declare a
reference to a function, simply take the declaration of a pointer to a
function and change the asterisk to an ampersand.

#### References to arrays

A reference to an array is far more useful than a pointer to an array.
The syntax is, like references to functions, a pointer to an array with
the asterisk replaced by an ampersand:

    #include <iostream>
    
    void print ( int (&ra)[5] )
    {
      for ( int i = 0; i < sizeof ra / sizeof ra[0]; i++ )
        std::cout<< ra[i] <<'\n';
    }
    
    int main()
    {
      int a[] = {1,2,3,4,5};
    
      print ( a );
    }

The biggest benefit to using a reference to an array over a pointer to
an array is that the **sizeof** trick for determining the size of an
array works as expected.

#### Using pointers

Theory and syntax is all well and good, but all of the clever ways for
using pointers in real code can be a shock to newcomers. In this part of
the tutorial, I will cover some of the more common ways of using
pointers, as well as common pitfalls and solutions to those pitfalls.
Many of these solutions can be found in FAQ documents in one form or
another as the answer to a common question.

#### Pointers and functions

As described several times before, one common use for pointers is to
simulate pass-by-reference semantics when the only choice is
pass-by-value. No tutorial on pointers would be complete without the
canonical swap example. Assume that you want to swap two integers. A
first attempt might look like this:

    #include <stdio.h>
    
    void swap ( int a, int b )
    {
      int save = a;
      a = b;
      b = save;
    }
    
    int main ( void )
    {
      int a = 10;
      int b = 20;
    
      printf ( "a: %d, b: %d\n", a, b );
      swap ( a, b );
      printf ( "a: %d, b: %d\n", a, b );
    
      return 0;
    }

Of course, that does not work because arguments are passed by value in
C. The solution is to pass pointers to **int**, so that the address can
be dereferenced and the actual value modified instead of a copy:

    #include <stdio.h>
    
    void swap ( int *a, int *b )
    {
      int save = *a;
      *a = *b;
      *b = save;
    }
    
    int main ( void )
    {
      int a = 10;
      int b = 20;
    
      printf ( "a: %d, b: %d\n", a, b );
      swap ( &a, &b );
      printf ( "a: %d, b: %d\n", a, b );
    
      return 0;
    }

This works as expected, though the code is no longer as clean. Being
able to change the value of an object from within a separate function is
the first reason for using pointers to pass an object to a function. The
same rule applies for pointers. Assume that you want to pass a pointer
to a function to allocate memory to it. The following does not work:

    #include <stdio.h>
    #include <stdlib.h>
    
    void alloc ( int *p )
    {
      p = malloc ( 10 * sizeof *p );
    
      if ( p == NULL )
        printf ( "Memory exhausted\n" );
    }
    
    int main ( void )
    {
      int *p = NULL;
    
      alloc ( p );
    
      if ( p == NULL )
        printf ( "alloc did not work\n" );
      else
        printf ( "alloc worked!\n" );
    
      return 0;
    }

Well, it works as far as an example in that it prints “alloc did not
work”, but if we wanted to use the memory allocated by **alloc** back in
**main**, it definitely does not work, because **malloc** is assigning
an address to a copy of the pointer. The solution is to pass a pointer
to the pointer. That way the address of the pointer can be dereferenced
and memory assigned to the original pointer, just like with the **swap**
example, except this time using the address of a pointer instead of the
address of an integer:

    #include <stdio.h>
    #include <stdlib.h>
    
    void alloc ( int **p )
    {
      *p = malloc ( 10 * sizeof **p );
    
      if ( *p == NULL )
        printf ( "Memory exhausted\n" );
    }
    
    int main ( void )
    {
      int *p = NULL;
    
      alloc ( &p );
    
      if ( p == NULL )
        printf ( "alloc did not work\n" );
      else
        printf ( "alloc worked!\n" );
    
      return 0;
    }

Now **alloc** will work as expected. If **malloc** fails, an error is
printed, but if **malloc** does not fail, “alloc worked\!” is printed,
as it should. The ability to change where a pointer points by passing a
pointer to the pointer is a useful feature, as the example suggests, and
it scales to any number of levels of indirection.

Another reason for passing pointers instead of the original object is
when the object is very large, and making a copy would be an expensive
operation:

    #include <stdio.h>
    
    struct s {
      char name[256];
      char address[1024];
      char phone[20];
    };
    
    void f ( struct s arg )
    {
      puts ( arg.name );
      puts ( arg.address );
      puts ( arg.phone );
    }
    
    int main ( void )
    {
      struct s arg = {
        "J. Random Guy",
        "123 Nowhere Lane, Some Place, Some Country",
        "1-111-111-1111",
      };
    
      f ( arg );
    
      return 0;
    }

With a size of at least 1300 bytes, **struct s** is somewhat large.
Passing an instance of that structure by value would require all of the
data to be copied, which might be very slow or require a lot of CPU
cycles. So, rather than pass that big object, it makes sense to pass a
(significantly smaller) pointer to the object:

    #include <stdio.h>
    
    struct s {
      char name[256];
      char address[1024];
      char phone[20];
    };
    
    void f ( struct s *arg )
    {
      puts ( arg->name );
      puts ( arg->address );
      puts ( arg->phone );
    }
    
    int main ( void )
    {
      struct s arg = {
        "J. Random Guy",
        "123 Nowhere Lane, Some Place, Some Country",
        "1-111-111-1111",
      };
    
      f ( &arg );
    
      return 0;
    }

Of course, because **f** has no intention of changing any of the value
of its argument, this might introduce a bug if any changes are
accidentally made. This is where const-correctness comes into play. If
it makes sense to pass a pointer to a function, but the object that the
pointer points to should not be modified, a pointer to **const** can be
passed instead, thus telling the compiler to warn about possible
modifications to the object:

    #include <stdio.h>
    
    struct s {
      char name[256];
      char address[1024];
      char phone[20];
    };
    
    void f ( const struct s *arg )
    {
      arg->name[0] = 'j'; /* Error! arg->name is const */
    
      puts ( arg->name );
      puts ( arg->address );
      puts ( arg->phone );
    }
    
    int main ( void )
    {
      struct s arg = {
        "J. Random Guy",
        "123 Nowhere Lane, Some Place, Some Country",
        "1-111-111-1111",
      };
    
      f ( &arg );
    
      return 0;
    }

Those are the two most common reasons for passing a pointer to a
function as an argument. However, pointers can also be returned from
functions. The syntax is consistent with all other declarations for a
pointer, but there are a few pitfalls in returning pointers that should
be discussed. These pitfalls apply to pointers as arguments, though it
is not as easy to fall into the traps that way. The first, and foremost,
pitfall of returning pointers is returning a pointer to local memory.
Most of the time this falls under the desire to return a scratch array
used to transform an argument:

    #include <stdio.h>
    #include <string.h>
    
    char *reverse ( const char *s )
    {
      char save[1024];
      int i = 0, j = strlen ( s );
    
      while ( j > 0 )
        save[i++] = s[--j];
      save[i] = '\0';
    
      return save; /* Wrong! */
    }
    
    int main ( void )
    {
      char *p = reverse ( "J. Random Guy" );
    
      puts ( p );
    
      return 0;
    }

While this code will compile and run, the most likely output will be
garbage characters. Why? Because **p** points to memory that was local
to **reverse**, and when **reverse** returned control to **main**, that
memory was released for other uses. Therefore, the memory that **p**
points to no longer belongs to the program. There are three solutions to
the problem. First, you can make the local array static, thus forcing
the lifetime of the array to be that of the entire program. Since the
problem was that the lifetime of the array was local to **reverse**, and
ended when **reverse** ended, this is a quick fix since it only requires
that the keyword **static** be prepended to the declaration of the local
array:

    #include <stdio.h>
    #include <string.h>
    
    char *reverse ( const char *s )
    {
      static char save[1024];
      int i = 0, j = strlen ( s );
    
      while ( j > 0 )
        save[i++] = s[--j];
      save[i] = '\0';
    
      return save; /* Safe now */
    }
    
    int main ( void )
    {
      char *p = reverse ( "J. Random Guy" );
    
      puts ( p );
    
      return 0;
    }

Unfortunately, static local variables are more trouble than they are
worth. First, this new **reverse** function will not play well in a
multi-threaded environment because there is only one copy of the array
even though it appears to be on the stack at first glance. Second, and
for the same reason, the string that is returned from **reverse** must
be immediately used and then forgotten, or immediately copied. Otherwise
a future call to **reverse** will overwrite the contents of the array,
and the result of the last call will be lost:

    int main ( void )
    {
      char *p;
    
      p = reverse ( "J. Random Guy" );
      reverse ( "This is a test" );
      puts ( p );
    
      return 0;
    }

The result will be, unintuitively, “tset a si sihT”, because the second
call to **reverse** caused the static array to be overwritten with the
new string. Several POSIX functions have this very problem because of a
local static variable, and many a curse has been uttered because of it.
The next solution does not have this problem because it forces the
calling function to supply a buffer. Naturally, this requires another
argument to the function, and the calling function can then create a
local array to pass to it:

    #include <stdio.h>
    #include <string.h>
    
    void reverse ( const char *s, char buffer[] )
    {
      int i = 0, j = strlen ( s );
    
      while ( j > 0 )
        buffer[i++] = s[--j];
      buffer[i] = '\0';
    }
    
    int main ( void )
    {
      char buffer[1024];
    
      reverse ( "J. Random Guy", buffer );
      puts ( buffer );
    
      return 0;
    }

Using a buffer argument and returning a pointer to that buffer can be a
powerful combination:

    #include <stdio.h>
    #include <string.h>
    
    char *reverse ( const char *s, char buffer[] )
    {
      int i = 0, j = strlen ( s );
    
      while ( j > 0 )
        buffer[i++] = s[--j];
      buffer[i] = '\0';
      
      return buffer;
    }
    
    int main ( void )
    {
      char buffer[1024];
      char *p;
    
      p = reverse ( "J. Random Guy", buffer );
      puts ( buffer );
      puts ( p );
    
      return 0;
    }

The third common solution to the original problem is to dynamically
allocate memory inside **reverse**, and then return a pointer to that
memory. Because dynamic memory has a lifetime of from when it is
explicitly allocated to when it is explicitly freed, it will exist until
the calling function calls **free** on the pointer:

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    char *reverse ( const char *s )
    {
      int i = 0, j = strlen ( s );
      char *save = malloc ( j + 1 );
    
      if ( save == NULL )
        return NULL;
    
      while ( j > 0 )
        save[i++] = s[--j];
      save[i] = '\0';
      
      return save;
    }
    
    int main ( void )
    {
      char *p;
    
      p = reverse ( "J. Random Guy" );
      puts ( p );
      free ( p );
    
      return 0;
    }

The problem with this solution is that it requires the calling function
to remember to free the memory. Unless the calling function explicitly
allocates memory, it is unlikely that the programmer will remember to
free it, especially if the allocation is hidden in a function.
Therefore, of the three solutions, passing a buffer to the function is
the best option most of the time.

Notice that in all of these examples, I have returned an array by
actually returning a pointer to the first element. This is the most
common way of returning an array and accepting an array as an argument.
It is possible to return a pointer to a whole array, but the syntax is
obscure and as descibed previously, pointers to arrays are not terribly
useful, while pointers to the first element of an array work just fine
and are easier to get right. Consider the **reverse** function, except
this time returning a pointer to an array:

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    char (*reverse ( const char *s ))[100]
    {
      int i = 0, j = strlen ( s );
      static char save[100];
    
      while ( j > 0 )
        save[i++] = s[--j];
      save[i] = '\0';
      
      return &save;
    }
    
    int main ( void )
    {
      char (*p)[100];
    
      p = reverse ( "J. Random Guy" );
      puts ( *p );
    
      return 0;
    }

#### Pointers as arrays

There is a close relationship between arrays and pointers. So close, in
fact, that many programmers confuse the two and believe that they are
the same. However, this is not the case. A pointer is not an array and
an array is not a pointer. But, in all but two cases, C behaves as if an
array name is converted to a pointer to the first element, so both can
be used in the same manner, most of the time. While there are most
certainly differences in the machine code produced, the differences are
negligable (usually involving an extra instruction to dereference the
pointer), and most programmers need not worry about it.

Consider two different contexts, value context and object context. In
value context, only the value of the array is required, so a pointer to
the first element can be used and through that pointer, all of the
elements can be discovered using pointer arithmetic. In object context,
the array must be treated as a whole, and therefore cannot be converted
to a pointer to the first element. The two cases where an array must be
evaluated in object context are:

1\) As the operand to the **sizeof** operator

Because the expected result of **sizeof** on an array is the total
number of bytes in the array, converting it to a pointer would be silly.
Instead of the size of an array, the size of a pointer would be the
result, and 9 times out of 10, the byte size of a pointer is
considerably smaller than an array. A common mistake is when new
programmers do not realize that an array argument is actually value
context, and try to use **sizeof** from within a function:

    #include <stdio.h>
    
    void f ( int a[] )
    {
      printf ( "%zd\n", sizeof a );
    }
    
    int main ( void )
    {
      int a[10];
    
      f ( a );
    
      return 0;
    }

If the size of int is 4 and the size of a pointer to int is 4, this
program will print 4 instead of the expected 40. That is because an
array argument is actually value context, and the array is converted to
a pointer to the first element. For that reason, these two function
declarations are equivalent:

    void f ( int a[] ); /* a is a pointer! */
    void g ( int *a );  /* a is a pointer */

Note that the **%zd** type specifier for printing **size\_t** values may
not be supported in compilers that do not conform to the C99 standard.
To avoid undefined behavior for such compilers, the result of **sizeof**
must be cast to the largest unsigned integral type:

    printf ( "%lu\n", (unsigned long)sizeof a );

2\) As the operand to the address-of operator

This one is more subtle. Because a pointer requires a type, and the
address-of operator evaluates to a pointer, and the size of an array is
a part of the type, the address-of operator must evaluate an array in
object context or the pointer will not have the size information needed
for pointer arithmetic.

All other uses of an array are in value context, and are subject to
conversion to a pointer to the first element. That is why, when pointing
a pointer to the first element of an array, you can do this:

    int *p = a;

Instead of being forced to do this:

    int *p = &a[0];

It is also why arrays appear to follow pass-by-reference semantics. In
reality, they follow pass-by-pointer semantics and the pointer is passed
by value. So a seemingly contradictory rule in C and C++ is actually not
contradictory at all, once you know the rule that an array is almost
always converted to a pointer to the first element. Once you know that
rule, all of the idiosyncrasies of arrays begin to make more sense.

#### Pointers as iterators

A pointer is often used to traverse the contents of an array. This is a
concrete example of the concept of an iterator. Through pointer
arithmetic, a pointer can walk across every element of an array with
ease. Here is an example of the standard C function, **strcpy**:

    char *jsw_strcpy ( char *dst, const char *src )
    {
      char *p = dst;
    
      while ( *src != '\0' )
        *p++ = *src++;
      *p = '\0';
    
      return dst;
    }

Compare that to the index-based version:

    char *jsw_strcpy ( char *dst, const char *src )
    {
      int i = 0, j = 0;
    
      while ( src[j] != '\0' )
        dst[i++] = src[j++];
      dst[i] = '\0';
    
      return dst;
    }

The usual argument in favor of the pointer version is speed. Many claims
that pointers are faster than array indexing are false (the effort
required to get any measurable performance boost is significantly
greater than most of the people who make those claims are aware of).
However, in many cases it is more convenient to use pointer arithmetic
than to maintain an index. On the other hand, it is sometimes more
convenient to use indices, such as when multiple indices are needed, or
when the location of the array might change, and thus invalidate all
pointers.

#### Pointers and dynamic memory

The majority of pointers to dynamic memory are to simulate C-style
strings and arrays of various dimensions. When allocating memory for a
C-style string, always remember to include one extra element for the
terminating null character (**'\\0'**).

When simulating arrays with pointers and dynamic memory, it helps to
remember that internally, all subscripting on arrays is converted to
pointer arithmetic using an offset. Why? Because array subscripting is
in value context, of course\! In the following program I use both array
subscripting and the internal equivalent of array subscripting:

    #include <stdio.h>
    
    int main ( void )
    {
      char a[] = "This is a test";
      int i;
    
      /* Using array subscripting */
      for ( i = 0; a[i] != '\0'; i++ )
        printf ( "%c", a[i] );
      printf ( "\n" );
    
      /* Using a pointer offset */
      for ( i = 0; *(a + i) != '\0'; i++ )
        printf ( "%c", *(a + i) );
      printf ( "\n" );
    
      return 0;
    }

From this example, it is safe to conclude that **a\[i\]** is actually
converted by the compiler to the equivalent of **\*(a + i)**. With this
in mind, it is easy to see how a block of dynamic memory can be
allocated to a pointer and then treated like an array, even with the
subscript operator\!

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    int main ( void )
    {
      char *p = malloc ( sizeof "This is a test" );
      int i;
    
      if ( p == NULL )
        return EXIT_FAILURE;
    
      strcpy ( p, "This is a test" );
    
      /* Using array subscripting */
      for ( i = 0; p[i] != '\0'; i++ )
        printf ( "%c", p[i] );
      printf ( "\n" );
    
      /* Using a pointer offset */
      for ( i = 0; *(p + i) != '\0'; i++ )
        printf ( "%c", *(p + i) );
      printf ( "\n" );
    
      free ( p );
    
      return EXIT_SUCCESS;
    }

This program will produce exactly the same output, even though instead
of an array, we are now using a pointer to dynamic memory. Notice the
strange use of **sizeof** on a string literal for **malloc**. The type
of a string literal is an array of **const char**...so we can use sizeof
on a string literal, it will evaluate the size of the array in object
context, and return the number of characters including the null
terminator\! Of course, there's very little practical use for that trick
because if you need the length of the string literal, you might as well
just use an array as we did in the previous example.

All of this works for simulating arrays of other types as well, but you
need to remember to calculate the size of the type because **malloc**
expects a byte count, and if you want ten integers, it will give you ten
bytes instead of **10 \* sizeof ( int )** bytes, unless you tell it to.
No size calculation is needed for dynamic arrays of **char** because
char is always 1:

    #include <stdio.h>
    #include <stdlib.h>
    
    int main ( void )
    {
      int *p = malloc ( 10 * sizeof *p );
      int i;
    
      if ( p == NULL )
        return EXIT_FAILURE;
    
      for ( i = 0; i < 10; i++ )
        p[i] = 10 * i;
    
      for ( i = 0; i < 10; i++ )
        printf ( "%d\n", p[i] );
    
      free ( p );
    
      return EXIT_SUCCESS;
    }

Take note that **sizeof \*p** is used to get the size of an **int**.
This seems like it is trying to dereference an uninitialized pointer,
but in fact it is perfectly legal. The **sizeof** operator does not
evaluate its operand, so no dereferencing is taking place, even though
the size of the correct type, **int**, is returned. This technique of
allocating memory is the recommended method over the more obvious, but
harder to maintain if types change, method used by older books and
certain stubborn programmers:

    int *p = malloc ( 10 * sizeof ( int ) );

In C++, this is all even easier. Using the **new\[\]** operator, you can
allocate enough memory for N objects and the compiler will handle the
size calculation for you:

    #include <iostream>
    
    int main()
    {
      int *p = new int[10];
    
      for ( int i = 0; i < 10; i++ )
        p[i] = 10 * i;
    
      for ( int i = 0; i < 10; i++ )
        std::cout<< p[i] <<'\n';
    
      delete [] p;
    }

In all cases, you must be careful to remember to free the memory that
you have allocated. This is done with the **free** function in C, and
the **delete\[\]** operator in C++. Note, however, that C++ also
supports the **new** and **delete** operators for creating single
objects instead of arrays. In C, if **malloc** fails then you must
handle the error accordingly (in this case we simply terminated the
program). In C++, if terminating the program is not a viable option, a
**bad\_alloc** exception must be caught and dealt with.

While simulating a one dimensional array with dynamic memory is an
almost trivial affair, multi-dimensional arrays are more difficult. If
you do not care about using the subscript operator for multi-dimensional
arrays, memory allocation is a simple matter of tallying up the sizes of
each dimension and the type size:

    int *p = malloc ( 2 * 3 * sizeof *p ); /* 2x3 array */

However, at this point, any indexing must be done manually, and the
expression to index an array allocated in this way with separate indices
**i** and **j** is **\*( ( p + i \* 3 ) + j )**, which is much more
complicated than the simple **\*(p + i)** of one dimensional arrays. The
problem only gets worse as more dimensions are added. Therefore, it is
beneficial to set up the memory such that the subscript operator works
properly.

But why doesn't the subscript operator work? It does not work because
the dynamic array is an array of int, but the subscript operator expects
an array of pointers to **int**. The subscript operator is converted by
the compiler into this **\*( \*( p + i ) + j )**, which treats **p**
like an array of arrays, and thus tries to dereference it more than
once. So, the only logical action to take is to create a structure where
this expression would work, then the subscript operator will also work.
There are two widely accepted methods for doing this. First, allocate
memory for an array of pointers, then allocate memory for an array of T
to each of those pointers. Naturally, this will require a pointer to a
pointer and M + 1 requests for memory with an MxN array:

    T **p = malloc ( M * sizeof *p );
    
    for ( int i = 0; i < M; i++ )
      p[i] = malloc ( N * sizeof *p[i] );

Because the subscript expression works for this layout, the subscript
operator will also work:

    #include <stdio.h>
    #include <stdlib.h>
    
    int main ( void )
    {
      int **p;
      int i, j;
    
      /* Error handing removed for simplicity */
      p = malloc ( 2 * sizeof *p );
    
      for ( i = 0; i < 2; i++ )
        p[i] = malloc ( 3 * sizeof *p[i] );
    
      /* Initialize using pointer notation */
      for ( i = 0; i < 2; i++ ) {
        for ( j = 0; j < 3; j++ )
          *( *( p + i ) + j ) = i * j;
      }
    
      /* Test with subscript notation */
      for ( i = 0; i < 2; i++ ) {
        for ( j = 0; j < 3; j++ )
          printf ( "%d ", p[i][j] );
        printf ( "\n" );
      }
    
      return 0;
    }

The biggest problem with this solution is that each array is not
required to be contiguous in memory. Arrays have this guarantee, but
because **malloc** does not have to return adjacent blocks, this
solution cannot be considered to be a correctly aligned two-dimensional
array. The next solution fixes this by allocating one big block, as we
did for manual indexing, and then allocates another array of pointers to
point into the block at regular intervals:

    #include <stdio.h>
    #include <stdlib.h>
    
    int main ( void )
    {
      int *base;
      int **p;
      int i, j;
    
      /* Error handing removed for simplicity */
      base = malloc ( 2 * 3 * sizeof *base );
      p = malloc ( 2 * sizeof *p );
    
      for ( i = 0; i < 2; i++ )
        p[i] = &base[i * 3];
    
      /* Initialize using pointer notation */
      for ( i = 0; i < 2; i++ ) {
        for ( j = 0; j < 3; j++ )
          *( *( p + i ) + j ) = i * j;
      }
    
      /* Test with subscript notation */
      for ( i = 0; i < 2; i++ ) {
        for ( j = 0; j < 3; j++ )
          printf ( "%d ", p[i][j] );
        printf ( "\n" );
      }
    
      return 0;
    }

The only change made was to add the base pointer, which points to the
big block, and the allocation step. The biggest problem with this
solution, aside from being somewhat obscure, is that if each array needs
to be of a different size, or worse, if each array needs to be able to
grow or shrink, the steps required to maintain this layout will be far,
far more complicated and time consuming than the first solution.

#### Pointers and linked data structures

Probably the most useful aspect of pointers is their ability to create
dynamically linked data structures such as linked lists and binary
search trees. This is done by creating what is called a
“self-referential structure”, where a user-defined type has a pointer
to itself:

    struct jsw_node {
      int data;
      struct jsw_node *next;
    };

While a structure cannot contain an instance of itself, because the
declaration would be infinitely recursive, it can contain a pointer to
an instance of itself, because the pointer does not actually create an
instance of the structure until assigned the address of an existing
instance. By creating multiple instances of the structure, and linking
them together by assigning the address of one to the pointer of another,
you can chain together wholely unique objects into a list:

    #include <stddef.h>
    #include <stdio.h>
    
    struct jsw_node {
      int data;
      struct jsw_node *next;
    };
    
    int main ( void )
    {
      struct jsw_node objects[] = {
        1, NULL,
        2, NULL,
        3, NULL,
        4, NULL,
        5, NULL,
      };
      struct jsw_node *start = objects;
      int i;
    
      for ( i = 1; i < 5; i++ ) {
        start->next = &objects[i];
        start = start->next;
      }
    
      start = objects;
    
      while ( start != NULL ) {
        printf ( "%d\n", start->data );
        start = start->next;
      }
    
      return 0;
    }

Of course, if you already have an array of the objects, there's really
no point to all of this because there are simpler ways to simulate a
linked data structure using arrays than self-referential structures.
However, consider the situation where each new item shows up one at a
time, and you do not know how many there will be. Coupled with dynamic
memory for adding or removing a structure on demade, a linked data
structure can be incredibly powerful:

    #include <stdio.h>
    #include <stdlib.h>
    
    struct jsw_node {
      int data;
      struct jsw_node *next;
    };
    
    int main ( void )
    {
      struct jsw_node *start = NULL;
      struct jsw_node *p;
      int i;
    
      while ( scanf ( "%d", &i ) == 1 ) {
        p = malloc ( sizeof *p );
    
        if ( p == NULL )
          break;
    
        p->data = i;
        p->next = start;
        start = p;
      }
    
      while ( start != NULL ) {
        printf ( "%d\n", start->data );
        p = start;
        start = start->next;
        free ( p );
      }
    
      return 0;
    }

Since this tutorial is already too long, and it is about pointers, not
linked data structures, I will not go into the details of linked lists.
Linked lists are not the only linked data structure, just the simplest.
The limit of what you can do with linked data structures is bound only
by your own creativity. However, there are many existing algorithms for
creating and working with linked data structures. This very site has a
growing number (as I find time to write them) of detailed tutorials on
classical and contemporary data structures.

#### Using references

References in C++ are surprisingly straightforward. As such, this
section on using them will be somewhat short. Though because references
are interesting and have some common and clever uses, we will cover them
briefly.

#### References and functions

References were designed to support operator overloading, but because
they can replace pointers for simulating pass-by-reference semantics
most of the time, this is one of the most common uses. However, because
there is no such thing as a null reference, pointers must still be used
if there is a potential need for a reference that may not refer to a
valid object.

Perhaps the most obvious use of references is to pass large objects to a
function without penalty and without cluttering up the code with the
syntax of a pointer. The pointer example from earlier can be rewritten
using a reference in C++:

    #include <iostream>
    
    struct s {
      char name[256];
      char address[1024];
      char phone[20];
    };
    
    void f ( const s& arg )
    {
      std::cout<< arg.name <<'\n'
        << arg.address <<'\n'
        << arg.phone <<'\n';
    }
    
    int main ( void )
    {
      s arg = {
        "J. Random Guy",
        "123 Nowhere Lane, Some Place, Some Country",
        "1-111-111-1111",
      };
    
      f ( arg );
    }

As with pointers, const-correctness applies when the object will not be
modified. There is one major difference between pointers and references
in this case; while a pointer to const must still point to an object
with an address, a const reference does not need to refer to an object
with an address and actually can refer to a value\!

    #include <iostream>
    
    void f ( const int& arg )
    {
      std::cout<< arg <<'\n';
    }
    
    int main ( void )
    {
      f ( 123 );
    }

By returning a reference, two nifty tricks are allowed. First, you can
return a reference and chain calls of a function together as long as
they take that reference as an argument:

    #include <iostream>
    
    int& f ( int& arg )
    {
      std::cout<< arg++ <<' ';
    
      return arg;
    }
    
    struct s {
      int i;
    
      s& g ( s& arg )
      {
        f ( i );
        return *this;
      }
    };
    
    int main ( void )
    {
      int i = 0;
      s t = {0};
    
      f ( f ( f ( f ( i ) ) ) );
      std::cout<<'\n';
      t.g ( t ).g ( t ).g ( t ).g ( t );
      std::cout<<'\n';
    }

To do the same thing with pointers would be incredibly awkward. This is
how a lot of the overloaded operators in the standard iostreams library
work.

Second, you can return a reference through a function call and assign to
the object that the reference refers to. This is the technique used by
objects that can pretend to be arrays and support subscripting:

    #include <iostream>
    
    class array {
      int base[5];
    public:
      int& operator[] ( int i )
      {
        return base[i];
      }
    };
    
    int main ( void )
    {
      array a;
    
      for ( int i = 0; i < 5; i++ )
        a[i] = 10 * i;
    
      for ( int i = 0; i < 5; i++ )
        std::cout<< a[i] <<'\n';
    }

#### References and pointers

From a language perspective, the following is true: A reference is not a
pointer. A pointer is not a reference. End of story. Anyone who tries to
explain a reference in terms of a pointer, or pointers in terms of a
reference is only going to confuse people in the long run.

#### Conclusion

Pointers are simple and powerful. But they are also flexible, and that
flexibility means they can be used for a huge number of tasks. Because
of the overwhelming usefulness of pointers, newcomers to C and c++ are
misled into believing that they are difficult, and the resulting fear
makes pointers difficult to learn. I have explained the core concept of
pointers to many people, and the usual reaction is, “That's it?”. Yes,
that's it. A powerful, flexible, low-level feature does not have to be
complicated.

-----

© 2018 - Eternally Confuzzled
