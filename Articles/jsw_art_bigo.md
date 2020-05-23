Asymptotic Notation
===================

I try to minimize things that might not be well known to my target audience because I'm reasonably sure that when a lot of people reach some funky notation that they don't understand, they stop reading. Now, while I try also to explain the good, the bad, and the ugly without using those notations as well, it's hard to realize this if you don't get to that point. ;-) The most important measures that I use are for asymptotic growth of algorithms. This article will attempt to explain the use of these notations without requiring you to have a degree in computer science.

Because I'm simplifying reality, it should be noted that none of the information in this article will be completely accurate in a formal mathematical sense, so please refrain from emailing me with complaints that I'm not entirely correct. The information in this article is more than sufficient to know what I mean when I say O(N<sup>2</sup>), or Ω(Nlog<sub>2</sub> N) and which one is better. And to be perfectly frank, once you get the basic concept, that's more than enough to live your life unless you have a burning desire to formally analyze and prove the performance properties of an algorithm. I don't really care about that because I can get close enough with my simplified reality to know which algorithms to choose.

The very word “asymptotic” scares the bejeezus out of people because it sounds complicated. The definition doesn't serve to aleviate that fear. Something that's asymptotic relates to a asymptote, which is defined as “A line whose distance to a given curve tends toward zero”. That's damn near worthless, so let's say that something asymptotic refers to a limiting behavior based on a single variable and a desired measure.

For example, we can use the number of steps that it takes for an algorithm that works with N items to complete (aka. time complexity) as the desired measure, and derive an asymptotic bound on the time complexity by increasing N toward infinity. In real person terms, we're just figuring out how much longer the algorithm takes when we add more items. The most common way to go about this is to double the number of items and see how much longer the algorithm takes.

Now, we could actually test this by writing the algorithm, profiling it to see how long it takes for N, then profiling it again after doubling N. The time difference is a rough estimate of the growth. This is called an empirical test. However, we can also do a theoretical test by measuring the steps that rely on the size of N and get a reasonably useful measure of how the time complexity grows. Because the steps that don't rely on N won't grow, we can remove them from the measure because at a certain point, they become so small as to be worthless. In other words, we pretend that they don't matter in all cases.

This is the idea behind asymptotic notation. By removing the constants (variables that have a fixed but unknown value), we can focus on the part of the measure that grows and derive a simplified asymptotic bound on the algorithm. A common notation that removes constants is called Big O Notation, where the O means “order of” (there are variants that do something similar that we'll look at shortly). Let's look at an example:

    void f (int a[], int n)
    {
        int i;

        printf("N = %d\n", n);

        for (i = 0; i < n; i++)
        {
            printf("%d ", a[i]);
        }

        printf("\n");
    }

In this function, the only part that takes longer as the size of the array grows is the loop. Therefore, the two printf calls outside of the loop are said to have a constant time complexity, or O(1), as they don't rely on N. The loop itself has a number of steps equal to the size of the array, so we can say that the loop has a linear time complexity, or O(N). The entire function f has a time complexity of 2 \* O(1) + O(N), and because constants are removed, it's simplified to O(1) + O(N).

Now, asymptotic notation also typically ignores the measures that grow more slowly because eventually the measure that grows more quickly will dominate the time complexity as N moves toward infinity. So by ignoring the constant time complexity because it grows more slowly than the linear time complexity, we can simplify the asymptotic bound of the function to O(N), so the conclusion is that f has linear time complexity.

Okay, but what does O really mean? Big O notation refers to the asymptotic upper bound, which means that it's a cap on how much the time complexity will grow. If we say that a function is O(1), then there's no growth and the function will always take a fixed amount of time to complete. If we say that a function is O(N) then if N doubles, the function's time complexity at most will double. It may be less, but never more. That's the upper bound of an algorithm, and it's the most common notation.

Now, even though O notation is the most common, it's not always the most accurate measure. For example, let's say we have a sequential search of an unordered array where the items are randomly distributed and we want both the average case growth and the worst case growth:

    int find(int a[], int n, int x)
    {
        int i;

        for (i = 0; i < n; i++)
        {
            if (a[i] == x)
            {
                return 1;
            }
        }

        return 0;
    }

This algorithm is clearly O(N) because it only has one loop that relies on the size of the array, and the time complexity of the loop doubles as the size of the array doubles. However, that's the worst case upper bound. We know (because smarter people than I figured it out) that on average, only half of the array is searched before the item is found due to the random distribution. So while the time complexity could reach O(N), it's usually less even though we don't really know how much less.

Okay, how about a binary search instead of a sequential search? If the array is sorted, we can make the search a lot faster by splitting the array in half at each comparison and only searching the half where the item might be. That's common knowledge, but why is it faster? Here's the code for a binary search:

    int find(int a[], int n, int x)
    {
        int i = 0;

        while (i < n)
        {
            int mid = (n + i) / 2;

            if (a[mid] < x)
            {
                n = mid;
            }
            else if (a[mid] > x)
            {
                i = mid + 1;
            }
            else
            {
                return 1;
            }
        }

        return 0;
    }

We can call this an O(N) algorithm and not be wrong because the time complexity will never exceed O(N). But because the array is split in half each time, the number of steps is always going to be equal to the base-2 logarithm of N, which is considerably less than O(N). So an even better choice would be to set the upper bound to log N, which is the upper limit that we know we're guaranteed never to cross. Therefore, a more accurate claim is that binary search is a logarithmic, or O(log<sub>2</sub> N), algorithm.

Sometimes we're interested not in an upper bound, but in a lower bound. What's the smallest time complexity that we can expect? For example, what if we want to know the lower bound for the binary search we just found the upper bound for? Well, since a correct binary search is guaranteed to only take log N steps to complete, we can say that the lower bound is also logarithmic. There's a notation for the lower bound too, called Omega, so we can say that the lower bound for binary search is Ω(log<sub>2</sub> N).

Wait a second. The upper and lower bounds are the same! That's good, now we can have a very accurate bound on the time complexity of a binary search. There's a notation for the asyptotically tight bound too, called Theta, and since we know the O and Ω for binary search and they're the same, we can say that binary search is Θ(log N). O(log<sub>2</sub> N) is still correct, and you'll see it more often than any of the other notations, but Θ(log<sub>2</sub> N) is a much stronger claim.

Note that in the best case scenario, the first item we look at would be the one we're looking for and the search would effectively be O(1), so why is the lower bound Ω(log<sub>2</sub> N)? Remember that we're only using one variable, the size of the array, to derive our measure. If we use other variables, such as the contents of the array and the item being searched for, we can easily say that the lower bound is O(1) because the best possible case is an immediate match. However, lacking those extra variables, we can't make an assumption. Therefore, the longest time complexity possible is logarithmic for both the upper and lower bounds.

Okay, what about a sorting algorithm? Let's start with selection sort. The algorithm is simple: find the largest item and move it to the back of the array. When you move an item to the back, decrease the size of the array so that you don't continually choose from the items that have already been selected:

    void jsw_selection(int a[], int n)
    {
        while (--n > 0)
        {
            int i, max = n;

            for (i = 0; i < n; i++)
            {
                if (a[i] > a[max])
                {
                    max = i;
                }
            }

            if (max != n)
            {
                jsw_swap (&a;[n], &a;[max]);
            }
      }
    }

This algorithm has two loops, one inside of the other. Both rely on the size of the array, so the algorithm is clearly O(N \* N), more commonly shown as O(N<sup>2</sup>) and referred to as quadratic. The fact that N decreases with each step of the outer loop is irrelevant unless you want a tight bound, and even then it's difficult to analyze. But that doesn't matter much because the upper bound is really all we care about for an existing sorting algorithm.

Let's look at a faster sort. The heap sort algorithm uses a tree based structure to make the selection process faster. Because the selection part of a selection sort is Θ(N), and the outer loop that it's nested in is O(N), selection sort is O(N<sup>2</sup>). But by using a heap where selection is O(1) and fixing the heap is Θ(log<sub>2</sub> N), we can turn the whole process of selection into a Θ(log<sub>2</sub> N) process:

    void jsw_do_heap(int a[], int i, int n)
    {
        int k = i * 2 + 1;
        int save = a[i];

        while (k < n)
        {
            if (k + 1 < n && a[k] < a[k + 1])
            {
                ++k;
            }

            if (save >= a[k])
            {
                break;
            }

            a[i] = a[k];
            i = k;
            k = i * 2 + 1;
        }

        a[i] = save;
    }

    void jsw_heapsort(int a[], int n)
    {
        int i = n / 2;

        while (i-- > 0)
        {
            jsw_do_heap(a, i, n);
        }

        while (--n > 0)
        {
            jsw_swap(&a;[0], &a;[n]);
            jsw_do_heap(a, 0, n);
        }
    }

Because the heap is structured like a tree, **jsw\_do\_heap** is Θ(log<sub>2</sub> N). The first loop in **jsw\_heapsort** is O(N / 2), but because the second loop is O(N) and dominates the first loop, we can toss the complexity of the first loop. So we have an O(N) loop that calls a Θ(log<sub>2</sub> N) function. We conclude that the upper bound of heap sort is O(Nlog<sub>2</sub> N), which doesn't have a set descriptive name, but it's often shown as O(N \* log<sub>2</sub> N). However, because the lower bound of heap sort is also Ω(Nlog<sub>2</sub> N) for the same reasons as binary search, we can safely say that heap sort has Θ(Nlog<sub>2</sub> N) time complexity.

We've looked at the most common time complexities: O(1) for constant time, O(N) for linear time, O(log<sub>2</sub> N) for logarithmic time, O(Nlog<sub>2</sub> N), and O(N<sup>2</sup>) for quadratic time. Others exist, such as O(N!) for a ridiculous factorial growth, but you won't see them often. Here are the upper bound time complexities in order of growth from least to greatest that you're most likely to see:

-   O(1) - No growth
-   O(log<sub>2</sub> N) - Grows by the logarithm of N when N doubles
-   O(N) - Grows with N when N doubles
-   O(Nlog<sub>2</sub> N) - Grows by the product of N and the logarithm of N when N doubles
-   O(N<sup>2</sup>) - Grows twice as fast as N when N doubles
-   O(N!) - Grows by the factorial of N when N doubles

Hopefully this article clears up any confusion without delving too far into the theory. Unfortunately, algorithm analysis can only be simplified so much before it becomes useless, but I think I covered all of the foundations that would be useful in figuring out a basic time complexity for your own algorithms as well as understanding many of the time complexities given for existing algorithms.

------------------------------------------------------------------------

© 2018 - Eternally Confuzzled
