# Quick Sort (Optimized)

## Table of Contents

- [Why Optimize?](#why-optimize)
- [Core Idea](#core-idea)
- [Function Reference](#function-reference)
- [ASCII Flowchart](#ascii-flowchart)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [Where It Gets Tricky](#where-it-gets-tricky)
- [Complexity Analysis](#complexity-analysis)
- [Comparison with Original](#comparison-with-original)
- [Summary](#summary)

---

## Why Optimize?

The original quick sort has three problems that hurt performance:

1. **Left recursion bug**: `quick_sort(array, 0, pivot - 1)` re-sorts the entire prefix from index 0 on every recursive call, instead of only sorting the current sub-range starting at `low`. This turns O(n log n) average into O(n^2) in practice.

2. **Fixed pivot choice**: Always picking the last element as pivot causes O(n^2) on sorted or reverse-sorted input — one of the most common real-world patterns.

3. **No small-array optimization**: Recursion overhead dominates for tiny sub-arrays where a simpler algorithm would be faster.

---

## Core Idea

Think of the original approach as always picking the person standing at the end of the line as the "divider." If the line is already sorted, this person is always the biggest (or smallest), and you barely split the line at all — it takes forever.

The optimized version makes three improvements:

1. **Median-of-three pivot**: Instead of blindly picking the last element, look at the first, middle, and last elements, and pick the middle value. This avoids the worst case on sorted input and usually gives a more balanced partition.

2. **Insertion sort for small groups**: When a sub-array has 10 or fewer elements, stop recursing and use insertion sort instead. Insertion sort has less overhead and is actually faster than quick sort for tiny arrays.

3. **Tail call elimination**: Instead of two recursive calls, use a while loop: recurse on the smaller half and iterate on the larger half. This guarantees the recursion stack never exceeds O(log n), even in the worst case.

And of course, the left recursion now correctly uses `low` instead of `0`.

---

## Function Reference

| Function | Signature | Purpose |
|----------|-----------|---------|
| `swap` | `void swap(int *a, int *b)` | Swap two integers via a temporary variable. |
| `insertion_sort` | `void insertion_sort(int *array, int low, int high)` | Sort `array[low..high]` using insertion sort. Used for sub-arrays with 10 or fewer elements to avoid recursion overhead. |
| `median_of_three` | `int median_of_three(int *array, int low, int high)` | Sort `array[low]`, `array[mid]`, `array[high]` among themselves, then move the median to `array[high-1]` as the pivot. Returns the pivot value. This ensures `array[low] <= pivot <= array[high]`, providing sentinels for the partition scan. |
| `partition` | `int partition(int *array, int low, int high)` | Partition `array[low..high]` around the median-of-three pivot. Uses Hoare-style two-pointer scanning: `i` moves right past elements < pivot, `j` moves left past elements > pivot, swapping when they both stop. Returns the final pivot index. |
| `quick_sort` | `void quick_sort(int *array, int low, int high)` | Main sorting function. Uses a while loop (tail call elimination) to always recurse on the smaller partition and iterate on the larger one. Falls back to insertion sort for sub-arrays of size <= 10. |
| `print_array` | `void print_array(const char *label, int *array, int len)` | Print an array with a label prefix, e.g. `Input:  [1 7 3 8 4]`. |
| `check_sorted` | `int check_sorted(int *array, int len)` | Verify an array is sorted in non-decreasing order. Returns 1 if sorted, 0 otherwise. |
| `run_test` | `void run_test(const char *name, int *src, int len)` | Run a single test: copy input, sort it, verify with `check_sorted` and `assert`, print PASS. |
| `main` | `int main(int argc, char *argv[])` | Entry point. Runs all 10 test cases (identical to the original) and prints "All tests passed!" on success. |

---

## ASCII Flowchart

```
+-----------------------------------+
| quick_sort(array, low, high)      |
+-----------------------------------+
                |
                v
        +---------------+
        | array == NULL? |--yes--> Return
        +---------------+
                | no
                v
        +---------------+
   +--->| low < high ?  |--no---> Return
   |    +---------------+
   |            | yes
   |            v
   |    +-------------------+
   |    | high-low+1 <= 10? |--yes--> insertion_sort(low,high)
   |    +-------------------+         Return
   |            | no
   |            v
   |    +-------------------+
   |    | median_of_three   |
   |    | sort arr[low],    |
   |    | arr[mid],arr[high]|
   |    | pivot -> high-1   |
   |    +-------------------+
   |            |
   |            v
   |    +-------------------+
   |    | partition:        |
   |    | i scans -->       |
   |    | j scans <--       |
   |    | swap when both    |
   |    | stop, until i>=j  |
   |    | place pivot at i  |
   |    +-------------------+
   |            |
   |            v
   |    +-------------------+
   |    | left half smaller?|
   |    +-------------------+
   |      /  yes      \  no
   |     v              v
   | +-----------+ +-----------+
   | | recurse   | | recurse   |
   | | (low,p-1) | | (p+1,high)|
   | | low=p+1   | | high=p-1  |
   | +-----------+ +-----------+
   |      |              |
   +------+--------------+
       (loop back)
```

---

## Step-by-Step Walkthrough

Let's trace `[3, 1, 4, 1, 5, 9, 2, 6, 5, 3]` (10 elements, so it hits insertion sort cutoff):

```
quick_sort(arr, 0, 9): size=10 <= INSERTION_CUTOFF
  -> insertion_sort(arr, 0, 9)

Insertion sort trace:
  i=1: key=1, shift 3 right -> [1, 3, 4, 1, 5, 9, 2, 6, 5, 3]
  i=2: key=4, already in place -> [1, 3, 4, 1, 5, 9, 2, 6, 5, 3]
  i=3: key=1, shift 4,3 right -> [1, 1, 3, 4, 5, 9, 2, 6, 5, 3]
  i=4: key=5, already in place -> [1, 1, 3, 4, 5, 9, 2, 6, 5, 3]
  i=5: key=9, already in place -> [1, 1, 3, 4, 5, 9, 2, 6, 5, 3]
  i=6: key=2, shift 9,5,4,3 right -> [1, 1, 2, 3, 4, 5, 9, 6, 5, 3]
  i=7: key=6, shift 9 right -> [1, 1, 2, 3, 4, 5, 6, 9, 5, 3]
  i=8: key=5, shift 9,6 right -> [1, 1, 2, 3, 4, 5, 5, 6, 9, 3]
  i=9: key=3, shift 9,6,5,5,4 right -> [1, 1, 2, 3, 3, 4, 5, 5, 6, 9]

Result: [1, 1, 2, 3, 3, 4, 5, 5, 6, 9]
```

Now let's trace a larger example where partitioning kicks in. Consider an 11-element array `[8, 3, 7, 1, 5, 9, 2, 6, 4, 10, 0]`:

```
quick_sort(arr, 0, 10): size=11 > 10

median_of_three: low=0(8), mid=5(9), high=10(0)
  Sort: 0, 8, 9 -> arr[0]=0, arr[5]=8, arr[10]=9
  Swap arr[5] with arr[9]: pivot_val=8, pivot at index 9
  Array: [0, 3, 7, 1, 5, 8, 2, 6, 4, 8, 9]
                                        ^ pivot at index 9

partition(0, 10):
  i=0, j=9 (start scanning from i+1=1, j-1=8)
  i scans right: arr[1]=3<8, arr[2]=7<8, arr[3]=1<8, arr[4]=5<8,
                 arr[5]=8 not <8, stop at i=5
  j scans left:  arr[8]=4 not >8, stop at j=8
  i(5) < j(8): swap arr[5] & arr[8] -> [..., 4, 2, 6, 8, ...]
  Continue: i scans: arr[6]=2<8, arr[7]=6<8, arr[8]=8 not <8, i=8
            j scans: arr[7]=6 not >8, j=7
  i(8) >= j(7): break
  Swap arr[8] with arr[9] (pivot): pivot now at index 8

Left half (0..7), right half (9..10)
Right half smaller -> recurse(9,10), set high=7

  recurse(9,10): size=2 <= 10 -> insertion_sort
  Back in while loop with low=0, high=7: size=8 <= 10 -> insertion_sort
```

---

## Where It Gets Tricky

1. **Median-of-three needs at least 3 elements.** The code only calls `partition` (which calls `median_of_three`) when the sub-array has more than 10 elements, so this is always safe. If you lower `INSERTION_CUTOFF` below 3, the median-of-three logic would need a guard.

2. **Sentinel elements in the partition scan.** After `median_of_three`, `array[low] <= pivot <= array[high]`. This means the left scan (`while array[++i] < pivot`) will stop at or before `high-1`, and the right scan (`while array[--j] > pivot`) will stop at or after `low`. No bounds checking is needed — the sorted endpoints act as natural sentinels.

3. **Equal elements.** When many elements equal the pivot, both scans stop and swap — this keeps the partition balanced even with duplicates. Unlike Lomuto partition (which degrades to O(n^2) on all-equal input), this Hoare-style approach handles equal elements gracefully.

4. **Tail call elimination correctness.** Always recursing on the smaller half and iterating on the larger guarantees O(log n) maximum stack depth, because the smaller half is at most n/2 elements. Without this optimization, adversarial input can cause O(n) stack depth and potential stack overflow.

---

## Complexity Analysis

| Metric | Value | Reason |
|--------|-------|--------|
| **Time (best/average)** | O(n log n) | Median-of-three gives balanced partitions on most inputs |
| **Time (worst case)** | O(n^2) | Still possible with adversarial input, but median-of-three makes this extremely unlikely for natural data |
| **Space (stack)** | O(log n) guaranteed | Tail call elimination ensures we always recurse on the smaller half |

---

## Comparison with Original

| | Original | Optimized |
|--|----------|-----------|
| **Left recursion** | `quick_sort(array, 0, pivot-1)` — re-sorts from index 0 every time | `quick_sort(array, low, pivot-1)` — only sorts the current sub-range |
| **Pivot selection** | Always last element — O(n^2) on sorted input | Median-of-three — avoids worst case on sorted/reverse input |
| **Small arrays** | Full recursion down to 1 element | Insertion sort for size <= 10 — less overhead |
| **Stack depth** | O(n) worst case | O(log n) guaranteed via tail call elimination |
| **Partition style** | Pivot-walking (pivot swaps into position) | Hoare-style two-pointer with sentinel elements |

---

## Summary

The optimized quick sort fixes the original's `0` vs `low` bug and adds three standard improvements: median-of-three pivot selection to avoid degenerate O(n^2) on sorted input, insertion sort for small sub-arrays to reduce recursion overhead, and tail call elimination to guarantee O(log n) stack depth. The Hoare-style partition scans from both ends and uses the median-of-three endpoints as natural sentinels, avoiding bounds checks. Together these changes make the algorithm robust against common worst-case patterns while keeping the same O(n log n) average-case performance.
