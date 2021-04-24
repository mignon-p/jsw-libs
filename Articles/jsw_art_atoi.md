# `atoi()` is evil\!

  

In this article I'll bash the `atoi` function. :) What could possibly be
the problem with a simple function that takes a string representing an
integer and converts it to `int`? Well, quite a number of things are
wrong, ranging from the practical to the theoretical. But let's start
with an example of usage to truly highlight the issues:

```c
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char buf[BUFSIZ];

    fputs("Enter an integer: ", stdout);
    fflush(stdout);

    if (fgets(buf, sizeof buf, stdin) != NULL)
    {
        int value = atoi(buf);

        printf("%d\n", value);
    }

    return 0;
}
```

"So what's the problem?", you might ask. The problem is that this code
is subtly broken. `atoi` makes two very big assumptions indeed:

1.  The string represents an integer.
2.  The integer can fit in an `int`.

If the string does not represent an integer at all, `atoi` will return
0. Yes, that's right. If `atoi` cannot perform a conversion, it will
return a valid result. Which means that if `atoi` ever returns 0, you
have no idea whether it was because the string is actually "0", or the
string was invalid. That's about as robust as a library can get...not\!

If the string does represent an integer but the integer fails to fit in
the range of `int`, `atoi` silently invokes undefined behavior. No
warning, no error, no recovery. Do not collect $200 and go straight to
jail, your program is completely undefined from that point forward.

By the way, don't expect any support from `errno` if `atoi` fails for
any reason; `atoi` isn't required to set `errno` under any
circumstances.

`atoi` falls under a class of truly heinous library functions that exist
solely due to backward compatibility of existing code. Another notable
member of this hall of shame is `gets`. Unlike `gets`, which cannot be
made safe, `atoi` can be used safely by thoroughly validating the string
before passing it in:

```c
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_valid_int(const char *s);

int main(void)
{
    char buf[BUFSIZ];

    fputs("Enter an integer: ", stdout);
    fflush(stdout);

    if (fgets(buf, sizeof buf, stdin) != NULL)
    {
        buf[strcspn(buf, "\n")] = '\0'; // Conditionally trim any newline

        if (is_valid_int(buf))
        {
            int value = atoi(buf);

            printf("%d\n", value);
        }
    }

    return 0;
}

bool is_valid_int(const char *s)
{
    long long temp = 0;
    bool negative = false;

    if (*s != '\0' && (*s == '-' || *s == '+'))
    {
        negative = *s++ == '-';
    }

    while (*s != '\0')
    {
        if (!isdigit((unsigned char)*s))
        {
            return false;
        }

        temp = 10 * temp + (*s - '0');

        if ((!negative && temp > INT_MAX) || (negative && -temp < INT_MIN))
        {
            return false;
        }

        ++s;
    }

    return true;
}
```

Aside from being a pain in the butt and easy to get wrong, you'll notice
that `is_valid_int` performs a string to integer conversion. If you're
already doing it manually, why do you then subsequently need `atoi` to
do exactly the same thing in a less safe way? Obviously something is
wrong here.

Now, I strongly recommend against writing your own manual conversions if
you can avoid it, because it's a pain in the butt and easy to get wrong,
as mentioned before. So what is a C programmer to do? The answer is
`strtol`. `atoi` is defined as behaving the same as `strtol(s,
NULL, 10)`, except for the behavior on error. This suggests that
`strtol` can handle the errors that `atoi` can't, and that's true.
Here's a safe replacement of `atoi` in the first program using `strtol`:

```c
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char buf[BUFSIZ];

    fputs("Enter an integer: ", stdout);
    fflush(stdout);

    if (fgets(buf, sizeof buf, stdin) != NULL)
    {
        char *end = NULL;

        errno = 0;

        long temp = strtol(buf, &end, 10);

        if (end != buf && errno != ERANGE && temp >= INT_MIN && temp <= INT_MAX)
        {
            int value = (int)temp;

            printf("%d\n", value);
        }
    }

    return 0;
}
```

The first part of the test after `strtol` returns is to see if any
characters were converted. If not, the string isn't a valid integer.
Then `errno` is checked for a range error, which `strtol` will assign if
the value is out of range for `long`. If `errno` remains 0 as you
initialized it, the value is in the range of `long`. Finally, the value
is verified to be within the range of `int` and you're good to go. This
code does the same thing that the manual code above did, except now
you're letting the standard library do the heavy lifting.

Note that checking `errno` for `ERANGE` and a range check against `int`
are required for full safety. The reason for this is that `long` and
`int` can have the same range. `strtol` will return `LONG_MIN` or
`LONG_MAX` if there's an overflow situation, at which point only the
value of `errno` will help you diagnose an error versus a legitimate
boundary value.

"But wait\!", you might say, "`atoi` just calls `strtol`, so it's
already safe\!" That may be true. In fact, in [my standard
library](https://storage.googleapis.com/google-code-archive-source/v2/code.google.com/c-standard-library/source-archive.zip),
that's exactly what `atoi` does:

```c
/*
    @description:
        Converts the initial portion of the string pointed to by s to an integer.
            * Behavior is undefined if the string cannot be represented.
*/
int atoi(const char *s)
{
    return (int)strtol(s, NULL, 10);
}
```

`atoi` defers to `strtol`, which in turn defers to `strtoull`:

```c
/*
    @description:
        Converts the initial portion of the string pointed to by s to an integer.
*/
long strtol(const char * restrict s, char ** restrict end, int base)
{
    unsigned long long temp;
    const char *it = s;

    /* Skip leading whitespace (looking for a sign) */
    while (isspace(*it))
        ++it;

    /* Using the original pointer because we must return it on failure */
    temp = strtoull(s, end, base);

    /* Handle underflow/overflow */
    if (*it == '-' && temp <= LONG_MAX) {
        errno = ERANGE;
        return LONG_MIN;
    }
    else if (*it != '-' && temp >= LONG_MAX) {
        errno = ERANGE;
        return LONG_MAX;
    }

    return (long)temp;
}

/*
    @description:
        Converts the initial portion of the string pointed to by s to an integer.
*/
unsigned long long strtoull(const char * restrict s, char ** restrict end, int base)
{
    const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    unsigned long long temp = 0, prev = 0;
    const char *it = s, *match, *num_end;
    char *sep = localeconv()->thousands_sep;
    size_t sep_len = strlen(sep);
    int sign = 0, n;

    /* Skip leading whitespace */
    while (isspace(*it))
        ++it;

    /* Check for and skip over any sign */
    if (*it == '-' || *it == '+')
        sign = (*it++ == '-');

    /* Handle unrealistic bases (excluding 0 because it has special meaning) */
    if (base < 0 || base == 2 || base > _BASE_MAX) {
        if (end)
            *end = (char*)s;

        return 0;
    }

    /* Extrapolate the base if it's 0 */
    if (base == 0) {
        if (*it == '0' && (it[1] && tolower(it[1]) == 'x')) {
            base = 16;

            /* Skip a leading 0x because it'll interfere later */
            it += 2;
        }
        else if (*it == '0') {
            base = 8;
        }
        else {
            base = 10;
        }
    }
    else {
        if (base == 16 && *it == '0' && (it[1] && tolower(it[1]) == 'x')) {
            /* Skip a leading 0x because it'll interfere later */
            it += 2;
        }
    }

    /* Skip leading zeros */
    while (*it == '0')
        ++it;

    /* Find the end of the first locale-friendly numeric string */
    num_end = integer_end(it, it + strlen(it), base);

    if (num_end == it) {
        /* There are no valid groups */
        if (end)
            *end = (char*)s;

        return 0;
    }

    /* Build the value */
    for (n = 0; it != num_end; ++n, ++it) {
        if (memcmp(it, sep, sep_len) == 0)
            it += sep_len; /* Skip over a thousands separator */

        match = (const char*)memchr(digits, tolower(*it), base);
        prev = temp;
        temp = base * temp + (match - digits);
    }

    if (n == 0) {
        /* No valid digits in the string */
        if (end)
            *end = (char*)s;

        return 0;
    }
    else {
        match = (const char*)memchr(digits, tolower(it[-1]), base);

        if (end)
            *end = (char*)it;

        /* Check for overflow */
        if (n >= max_digits(sizeof(unsigned long long) * CHAR_BIT, base) || (temp - (match - digits)) / base != prev)
        {
            errno = ERANGE;
            return ULLONG_MAX;
        }

        if (sign)
            temp = -temp;

        return temp;
    }
}

/*
    @description:
        Locates the end of the first valid integer string starting at s.
        This function is aware of the current locale's LC_NUMERIC setting.
*/
const char *integer_end(const char *first, const char *last, int base)
{
    char *grouping = localeconv()->grouping;
    int group_len = 0, group_size = *grouping;

    const char *end = last;
    const char *it = end - 1;

    for (;;) {
        if (it == first && group_size && _digitvalue(*end, base) != -1)
            end = first;
        else if (it != first && group_size && ++group_len == group_size) {
            /* Check for a group separator */
            if (it - 1 == first || it[-1] != *localeconv()->thousands_sep) {
                /* Invalid group: reset grouping, mark the end and proceed */
                grouping = localeconv()->grouping;
                group_size = *grouping;
                group_len = 0;
                end = it; /* Save 1 past the last valid character */
            }
            else {
                /* Valid group: move to the next grouping level */
                if (*grouping && *++grouping)
                    group_size = *grouping;

                group_len = 0;

                /* Skip over the separator so we don't error on the next iteration */
                --it;
            }
        }
        else if ((*it == '-' || *it == '+') && it != first) {
            /* Invalid sign: reset grouping, mark the end and proceed */
            grouping = localeconv()->grouping;
            group_size = *grouping;
            group_len = 0;
            end = it; /* Save 1 past the last valid character */
        }
        else if (!(*it == '-' || *it == '+') && _digitvalue(*it, base) == -1) {
            /* Invalid digit: reset grouping, mark the end and proceed */
            grouping = localeconv()->grouping;
            group_size = *grouping;
            group_len = 0;
            end = it; /* Save 1 past the last valid character */
        }

        if (it == first)
            break;

        --it;
    }

    return end;
}

/*
    @description:
        Calculates the maximum number of decimal digits that can be stored
        in a value of up to the specified bits in the specified base.
*/
int max_digits(int bits, int base)
{
    return (base > 1) ? 1 + (int)(log(pow(2.0, bits)) / log((double)base)) : 0;
}
```

I included the full code to highlight that a string to integer
conversion is far more complex than the sub-20 line function most people
show as an example. However, to answer the complaint above, this is an
implementation detail. Implementations are not required to implement
`atoi` in terms of `strtol`. So while your implementation may do it,
others may not, and good code must be written with all possible
implementations in mind.

But let's assume all possible implementations do this, so undefined
behavior isn't an issue. However, we still have the not insignificant
problem of returning 0. Since there is no way to properly check the
result of `strtol` as in the original fixed code because it's hidden by
and under control of the implementation, there is subsequently *still*
no way to determine whether 0 means the string was not a valid integer
or the string represented "0". Now what, smarty pants? :)

Hopefully this article is enlightening that `atoi` is either extremely
dangerous, impossible to use without confounding results, or both. I
also tried to exhibit how string to integer conversions are actually
somewhat difficult to implement properly (by showing a full
standard-conforming implementation of `strtol`) and naive
implementations are severely lacking.

-----

© 2018 - Eternally Confuzzled
