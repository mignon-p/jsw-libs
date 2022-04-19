# Random Numbers



The informal definition of a random sequence is where each newly
generated number has no immediately obvious relation to the numbers
generated before it. While in computer science there is no way to
generate random numbers deterministically, algorithms do exist that give
the appearance of a random sequence. This tutorial will cover the most
common class of algorithms for generating pseudo-random numbers: Linear
Congruential Generators. After discussing the theory behind LCG
algorithms, we will go on to look at built-in support for random numbers
in C and C++, and effective ways to make use of it.

#### Linear Congruential Generators

A linear congruential sequence is a series of numbers based on the
recurrence relation formula:

X<sub>n</sub> = (aX<sub>n-1</sub> + c) mod m

In this formula, m is called the modulus, a is called the multiplier,
and c is called the increment. It is not difficult to imagine the
resulting sequence of this formula when given different values. For
example, with a = 2, c = 3, m = 10, and a starting value of X = 5, the
following sequence would be produced:

    5 3 9 1 5 3 9 1 5 3...

While this particular sequence is simple to calculate by hand, an
algorithm to produce it is a trivial direct translation from the formula
to source code:

```c
    #include <stdio.h>

    int jsw_lcg(int seed)
    {
        return  (2 * seed + 3) % 10;
    }

    int main(void)
    {
        int seed = 5;
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", seed);
            seed = jsw_lcg(seed);
        }

        printf("...\n");

        return 0;
    }
```

Notice that the sequence repeats after four numbers have been generated.
This is due to the modular arithmetic that forces wrapping of values
into the desired range. In random number theory, the length of this
cycle is called the period of the sequence. As you might expect, a
longer period allows for sequences that appear more random because there
is no immediately discernable pattern. The longest period possible for a
linear congruential algorithm is the value of m itself. As such, the
larger m is, the better the chances are of having a long period.

Also notice that despite a modulus of 10, the period of the algorithm
given was only 4. This suggests that both the increment and the
multiplier should be chosen with care. It is possible to obtain a
sequence that covers the full range of m such that the resulting
sequence is a valid permutation of the values in the range. This is
called a Full-Cycle Linear Congruential Generator. There are a few rules
(described by Knuth), that when adhered to, guarantee a full-cycle.
However, these rules only apply to the formula when c is not zero. When
c is not zero, the algorithm is typically called a mixed congruential
generator.

Unfortunately, a long period does not guarantee a random sequence. The
majority of combinations of a, c, and m will either have sufficient
randomness but an unacceptably short period, or a long period with an
obviously non-random sequence. Of the ways to work around this problem,
the most common is to ignore the mixed congruential generator and
instead use the formula for a pure multiplicative generator. The formula
is the same, except for the removal of the variable c:

X<sub>n</sub> = aX<sub>n-1</sub> mod m

The difference between a mixed congruential generator and a pure
multiplicative generator is that the former uses c, where c is not equal
to zero, and the latter uses c, where c is always equal to zero. Because
c is now a constant with no effect, it can be removed from the formula
entirely. The result is a simpler algorithm, so many random number
algorithms use a pure multiplicative generator.

One note should be made before moving on, and that is that pure
multiplicative generators cannot generate 0, or every subsequent value
will be 0 as well. This was not an issue with mixed congruential
generators because if a value of 0 was generated, c would be added to
it.

As already described, pure multiplicative generators have been widely
accepted as random number generators, and studied extensively. Through
trial and error, many acceptable combinations of modulus and multiplier
have been discovered. However, Park and Miller have called one of these
combinations the Minimal Standard Generator. The resulting sequence from
m = 2,147,483,647 and a = 16,807 is recommended as the bare minimum for
random number generators (though Park and Miller now recommend a =
48,271 for better statistical properties). While there are better
combinations of modulus and multiplier, and even other (and better)
categories of algorithm than the linear congruential generator, the
Minimal Standard Generator is the one by which all others should be
judged, and an acceptable algorithm should not be worse.

The Minimal Standard Generator is relatively simple to write (assuming
at least 32-bit integers):

```c
    #include <stdio.h>

    #define M 2147483647
    #define A 16807
    #define Q ( M / A )
    #define R ( M % A )

    static int seed = 1;

    int jsw_rand(void)
    {
        seed = A * (seed % Q) - R * (seed / Q);

        if (seed <= 0)
        {
            seed += M;
        }

        return seed;
    }

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", jsw_rand());
        }

        printf("...\n");

        return 0;
    }
```

This is quite different from the basic LCG algorithm. The reason that
extra work is required, is because, unlike the theoretical world, real
programming languages have limits. If the intermediate values of aX
exceed the limits of a signed integer, the type will overflow and the
algorithm will invoke undefined behavior. Unsigned types could be used
instead, but then the Minimal Standard Generator would not be strictly
correct, as it would produce the wrong sequence. The generally accepted
solution is called Schrage's Method, a clever approach that avoids
overflow through careful use of division and subtraction, however, an
alternative method is to use floating-point for intermediate values.

Because the resulting sequence from just about every general random
number generator may consist of very large numbers, it may not be
practical for use in programs that need a random number within a smaller
range. The need arises to scale random numbers to fit the application,
and a whole new class of problems is created as a result.

There are two potential needs for scaling a random number sequence.
First, you might need random numbers in the range of \[0, n). Once again
turning to modular arithmetic, this is a simple affair. In the above
program, only one line must to be changed to to force the sequence into
the range of, say, \[0, 100):

```c
    printf ( "%d ", jsw_rand() % 100 );
```

To shift this range to \[1, 101), simply add 1 to the result:

```c
    printf ( "%d ", jsw_rand() % 100 + 1 );
```

The second need for scaling a random number sequence is into a range
with specific high and low values. To do this is only slightly more
difficult:

```c
    printf ( "%d ", low + ( jsw_rand() % ( high - low ) ) );
```

However, be aware that the randomness of a random number sequence is
global for the sequence as a whole. If you shrink the range then there
is a higher probability of the sequence appearing less random. For
example, you may have long runs of the same number if the scaled range
is small. This is immediately obvious if you shift the range to \[0,2):

```c
    #include <stdio.h>

    #define M 2147483647
    #define A 16807
    #define Q ( M / A )
    #define R ( M % A )

    static int seed = 1;

    int jsw_rand(void)
    {
        seed = A * (seed % Q) - R * (seed / Q);

        if (seed <= 0)
        {
            seed += M;
        }

        return seed;
    }

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", jsw_rand());
        }

        printf("...\n");

        return 0;
    }
```

It is theoretically possible that in a sequence of one million random
numbers, of any range, a single value will be generated one million
times. However, this is highly unlikely for long ranges, but for short
sequences, apparent non-randomness can be a problem. Also, scaling a
random range through modulus is generally frowned upon with linear
congruential generators. A more reliable approach is to find the deviate
of a range and multiply by N rather than use a given random number and
take the remainder of division:

```c
    #include <stdio.h>

    /* ... */

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            jsw_rand();
            printf("%d ", (int)(uniform_deviate() * 100));
        }

        printf("...\n");

        return 0;
    }
```

The linear congruential generator shown so far is the
bottom-of-the-bucket generator, and it does have problems. In fact,
linear congruential generators as a whole have generally acceptable
performance, but in randomness testing, they still show uncomfortable
patterns. Fortunately, there is a solution to this without turning to
dreadfully complicated algorithms such as the Mersenne Twister (the most
popular random number generator at the time of writing).

The linear congruential generator shown so far is a single-phase
algorithm. This means that it calculates one seed and returns the value
calculated. Another approach uses a two-phase algorithm, where the
results of two linear congruential generators are combined to produce a
significantly more random sequence. Pierre L'Ecuyer studied this brand
of LCG and suggests a combination of two LCG algorithms with m1 =
2,147,483,563, a1 = 40014, and m2 = 2,417,483,399, a2 = 40692.The
implementation is only marginally more difficult than a single-phase
algorithm using either one of those:

```c
    #define M1 2147483647
    #define M2 2147483399
    #define A1 40015
    #define A2 40692
    #define Q1 ( M1 / A1 )
    #define Q2 ( M2 / A2 )
    #define R1 ( M1 % A1 )
    #define R2 ( M2 % A2 )

    static int seed1 = 1, seed2 = 1;

    /* Dual-Phase Linear Congruential Generator */
    int jsw_rand(void)
    {
        int result;

        seed1 = A1 * (seed1 % Q1) - R1 * (seed1 / Q1);
        seed2 = A2 * (seed2 % Q2) - R2 * (seed2 / Q2);

        if (seed1 <= 0)
        {
            seed1 += M1;
        }

        if (seed2 <= 0)
        {
            seed2 += M2;
        }

        result = seed1 - seed2;

        if (result < 1)
        {
            result += M1 - 1;
        }

        return result;
    }
```

The only problem with this approach is that multiple seeds must be
maintained. This is an issue because to avoid repeating the sequence
each time a program is run, client code is often required to specify the
starting seed. While only one such seed needs to be initialized with the
Minimal Standard Generator, two are needed with a dual-phase generator,
three for a tri-phase generator, and so on. This initialization can be
done with a helper function that “seeds” the random number generator:

```c
    #include <stdio.h>

    #define M1 2147483647
    #define M2 2147483399
    #define A1 40015
    #define A2 40692
    #define Q1 ( M1 / A1 )
    #define Q2 ( M2 / A2 )
    #define R1 ( M1 % A1 )
    #define R2 ( M2 % A2 )

    static int seed1 = 1, seed2 = 1;

    void jsw_seed(int s1, int s2)
    {
        seed1 = s1;
        seed2 = s2;
    }

    /* Dual-Phase Linear Congruential Generator */
    int jsw_rand(void)
    {
        int result;

        seed1 = A1 * (seed1 % Q1) - R1 * (seed1 / Q1);
        seed2 = A2 * (seed2 % Q2) - R2 * (seed2 / Q2);

        if (seed1 <= 0)
        {
            seed1 += M1;
        }

        if (seed2 <= 0)
        {
            seed2 += M2;
        }

        result = seed1 - seed2;

        if (result < 1)
        {
            result += M1 - 1;
        }

        return result;
    }

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", jsw_rand() % 100);
        }

        printf("...\n");

        jsw_seed(12345, 23456);

        for (i = 0; i < 10; i++)
        {
            printf("%d ", jsw_rand() % 100);
        }

        printf("...\n");

        return 0;
    }
```

Because **seed1** and **seed2** are statically initialized to 1, that is
the default seed for the generator. However, by calling **jsw\_seed**,
the client code can “re-seed” the generator to produce a different
sequence at will. This method is employed by the standard library
functions rand and srand, which we will cover later in the tutorial.

Sometimes, a number between 0 and N is not what is desired, such as in
simulations, but a random floating-point number between 0 and 1. In its
simplest form, this is called a uniform deviate, and it is easy to
produce from the result of a call to **jsw\_rand**. Simply multiply the
value returned by **jsw\_rand** by 1/m:

```c
    #include <stdio.h>

    #define M 2147483647
    #define A 16807
    #define Q ( M / A )
    #define R ( M % A )

    static int seed = 1;

    /* Single-Phase Linear Congruential Generator */
    int jsw_rand(void)
    {
        seed = A * (seed % Q) - R * (seed / Q);

        if (seed <= 0)
        {
            seed += M;
        }

        return seed;
    }

    double uniform_deviate(void)
    {
        return (double)seed * (1.0 / M);
    }

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            jsw_rand();
            printf("%f\n", uniform_deviate());
        }

        printf("\n");

        return 0;
    }
```

Fortunately, or unfortunately, depending on your opinion of the quality
of common implementations of the standard library, the C standard
library (also inherited by the C++ standard library) supports two
functions for generating random numbers and seeding that generator.
These functions are called **rand**, and **srand**, respectively. Their
use is surprisingly simple, and fairly consistent with the design of
**jsw\_rand** and **jsw\_seed**:

```c
    #include <stdio.h>
    #include <stdlib.h>

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", rand() % 100);
        }

        printf("\n");

        srand(12345U);

        for (i = 0; i < 10; i++)
        {
            printf("%d ", rand() % 100);
        }

        printf("\n");

        return 0;
    }
```

Now, instead of the definition for **jsw\_rand** and **jsw\_seed**, all
you have to do is include **stdlib.h** (or **cstdlib** for C++). To
obtain a random number in a range, the same concepts apply as with
**jsw\_rand**. However, because the implementation of rand is unknown,
it is impossible to tell whether or not using modulus to shrink the
range will result in an acceptably random sequence. The reason that this
might not be the case is due to the very definition of linear
congruential generators. The details are complicated, and I'm not
entirely sure I understand them completely myself, so I will refrain
from embarrassing myself in an attempt to describe the issues. Just be
aware that scaling the range of a linear congruential generator with
modulus may have a detrimental effect on the randomness of the sequence.

The common solution is to use the constant value **RAND\_MAX**, defined
with every standard library, to use the high-order bits of a random
number through division rather than low-order bits with the remainder of
division:

```c
    #include <stdio.h>
    #include <stdlib.h>

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", rand() / (RAND_MAX / 100 + 1));
        }

        printf("...\n");

        return 0;
    }
```

An alternative to dividing **RAND\_MAX** by N is to force the expression
to floating-point and multiply **RAND\_MAX** by N:

```c
    #include <stdio.h>
    #include <stdlib.h>

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", (int)(rand() / (RAND_MAX + 1.0) * 100));
        }

        printf("...\n");

        return 0;
    }
```

Yet another alternative is to utilize the **uniform\_deviate** function
provided not too long ago. The rules still apply, even though we are now
using a library function rather than rolling our own, so modifying
**uniform\_deviate** to work with rand is an exercise in the trivial:

```c
    #include <stdio.h>
    #include <stdlib.h>

    double uniform_deviate(int seed)
    {
        return seed * (1.0 / (RAND_MAX + 1.0));
    }

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", (int)(uniform_deviate(rand()) * 100));
        }

        printf("...\n");

        return 0;
    }
```

However, at the time of writing, implementations have improved such that
modulus will likely work, and if it does not work, chances are good that
the standard **rand** is unsuited to the application anyway. In such a
case, an algorithm known to be good should be used, such as the
dual-phase generator provided in this tutorial, or the Mersenne Twister,
which has an incredibly large period and very good statistical
properties.

Most tutorials will simply recommend the Mersenne Twister and be done
with it, but I will be kind enough to offer a working implementation.
How it works is far beyond the scope of this tutorial. A simple web
search for “Mersenne Twister” will give you several resources including
the original paper, which is an interesting read and provided the
majority of the information required to create the following two
functions:

```c
    #define N 624
    #define M 397
    #define A 0x9908b0dfUL
    #define U 0x80000000UL
    #define L 0x7fffffffUL

    static unsigned long x[N];
    static int next;

    void jsw_seed(unsigned long s)
    {
        int i;

        x[0] = s & 0xffffffffUL;

        for (i = 1; i < N; i++) {
            x[i] = (1812433253UL * (x[i - 1] ^ (x[i - 1] >> 30)) + i);
            x[i] &= 0xffffffffUL;
        }
    }

    unsigned long jsw_rand(void)
    {
        unsigned long y, a;
        int i;

        /* Refill x if exhausted */
        if (next == N) {
            next = 0;

            for (i = 0; i < N - 1; i++) {
                y = (x[i] & U) | x[i + 1] & L;
                a = (y & 0x1UL) ? A : 0x0UL;
                x[i] = x[(i + M) % N] ^ (y >> 1) ^ a;
            }

            y = (x[N - 1] & U) | x[0] & L;
            a = (y & 0x1UL) ? A : 0x0UL;
            x[N - 1] = x[M - 1] ^ (y >> 1) ^ a;
        }

        y = x[next++];

        /* Improve distribution */
        y ^= (y >> 11);
        y ^= (y << 7) & 0x9d2c5680UL;
        y ^= (y << 15) & 0xefc60000UL;
        y ^= (y >> 18);

        return y;
    }
```

The Mersenne Twister is surprisingly short for how good it is, but, like
most effective random number generators, the code is almost completely
opaque. At first glance, it is a mess of magic numbers and bitwise
operations, and comments generally do not help much. Therefore, I highly
recommend reading the paper by Matsumoto and Nishimura if understanding
is your goal. If not, the two functions have passed stringent testing
and have come out clean in every case, so you can be comfortable just
using it if you want.

As a final note, the seed for a random number generator itself should
appear to be random, or at least change with each run of the program. As
a result, most people prefer to seed their random number generator with
the current system time. The code typically looks like this:

```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", rand() % 100);
        }

        printf("...\n");

        srand((unsigned)time(NULL));

        for (i = 0; i < 10; i++)
        {
            printf("%d ", rand() % 100);
        }

        printf("...\n");

        return 0;
    }
```

Now, while I am not aware of a system where this would not work, it is
technically not portable because the C (and C++) standards do not
require that **time\_t**, which is the return value of the **time**
function, be convertable to unsigned int. To be strictly portable, it
has been suggested that a hash can be taken of the bytes of the
**time\_t**, converted to **unsigned**, and passed to **srand**. The
following is based on a function by Lawrence Kirby via Ben Pfaff:

```c
    #include <limits.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>

    unsigned time_seed(void)
    {
        time_t now = time(NULL);
        unsigned char *p = (unsigned char *)&now;
        unsigned seed = 0;
        size_t i;

        for (i = 0; i < sizeof now; i++)
        {
            seed = seed * (UCHAR_MAX + 2U) + p[i];
        }

        return seed;
    }

    int main(void)
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            printf("%d ", rand() % 100);
        }

        printf("\n");

        srand(time_seed());

        for (i = 0; i < 10; i++)
        {
            printf("%d ", rand() % 100);
        }

        printf("\n");

        return 0;
    }
```

Random numbers are a pain in the butt. However, they're terribly
interesting, and very useful in many applications of computer science.
Hopefully this tutorial has laid the foundation necessary for using
random numbers properly and enough interest in the topic to promote
further study. This article has only scratched the surface, as random
number generation is a popular topic of study for computer scientists
and mathematicians, so I encourage you to branch out from here and
continue your research.

-----

© 2018 - Eternally Confuzzled
