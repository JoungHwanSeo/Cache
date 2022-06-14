# [Lab 4] Cache Simulation

## Introduction

The goal of this lab is to implement a write-back cache with LRU replacement policy. To do so, you will write a cache simulator that reads a memory trace and performs operations to update the metadata in the tag store, depending on a cache hit or a cache miss. You do not need to implement the data store in this lab. Your simulator should keep track of the statistics (e.g., #accesses, #hits, #misses, #writes, #writebacks) and update the statistics for every memory reference.

## Part I: Direct-Mapped Cache

The first step is to implement a direct-mapped cache. Note that a direct-mapped cache *does not* have to do with LRU replacement policy. The initial code is provided for your convenience. You are free to modify most of the code provided throughout this lab.

### Cache Parameters

The cache simulator takes as inputs the following parameters and initiates the cache structure.

- **Cache Size**: Total cache capacity in bytes
- **Associativity**: For the direct-mapped cache, this will be 1.
- **Line Size**: Cache block size in bytes

These parameter values need to be always powers of two. Your cache needs to be able to print out the cache statistics for different combinations of the parameters.

### Write-Allocate & Write-Back

Your cache needs to be a write-allocate, write-back cache. That is,

- A write miss allocates a cacheline in the cache with a **dirty** flag.
- When a line is evicted and the line is dirty, it is written back to memory and the writeback counter increments.

## Part II: Set-Associative Cache

The second step is to extend your direct-mapped cache to a set-associative cache with LRU replacement policy. In an `N`-way set-associative cache, each cache set has `N` cache blocks. A memory address is still mapped to a specific set, but the requested data can be found to any one of the `N` blocks in the set.

### Replacement Policy: True LRU

We need to maintain an LRU status of the blocks in each set.
* **Cache Hit**: The cache block is promoted to the MRU (most recently used) position.
* **Cache Miss**: You need to evict the LRU block, bring the new cacheline, and set it to **MRU**. Note that you first need to check if there exists an invalid cache block. If so, you need to use it first.

Note that the code that is needed for the LRU implementation is not included in the initial code. You are free to modify the code to implement LRU.

## Trace

The sample trace file is located in the **traces** folder. Each line in the trace consists of two fields.

- **1st field**: Indicates whether the memory reference is a data read (0), a data write (1), or an instruction fetch (2)
- **2nd field**: Memory address

Note that you implement a unified (I/D) cache that caches both instructions and data for Part II.

## Compile & Run

### Compile Code
```
$ make
```
or 
```
$ g++ -o cache_sim cache_sim.cc cache.cc
```

Run Simulation
```
./cache_sim <trace> <cache size (in bytes)> <associativity> <line size (in bytes)>
```

```
$ ./cache_sim ./traces/sample.trace 16384 2 64
```

## Tips & Cache Operations & Statistics

* You may want to write your own simple trace for which you can verify the answer by hand, and use it to check the results from the simulator.
* The number of hits/misses includes all READ/WRITE/IF hits/misses.
* The LRU stack also needs to be updated for all the READ/WRITE/IF requests.
* This document may not describe all the straightforward cache operations. Please think carefully what will happen when you write the code and count the statistics.

## Submission

#DUE: **6/19 (SUN) 11:59PM**

### Late Policy (**2 Days**)
* 10% discounted per day (6/20 12:00 AM is a late submission) 
* After 06/22 (WED) 12:00AM, you will get **zero** score for the assignment

### What to Submit
*  Your code that we can compile and run  
  * Do not include the trace file in the zipped file.
* A **1-page** report that describes your implementation
  * You need to include the cache hit rate for the 16KB direct-mapped cache and 2-/4-/8-way set associative caches with the 64B linesize.

### How to Submit
* Upload your compressed file (zip) to eTL
* Format: **YourStudentID_YOURLASTNAME_lab#**
  * e.g., 2020-12345_KIM_lab4.zip
* Please make sure you follow the format (10% penalty for the wrong format)


## Part III: Multi-Level Cache Hierarchy (Extra Credits:+30 Points)

> * **Disclaimer**: This will be graded only if you have completed the SA implementation
> * No Late Submission for Extra Credits

Now, consider a two-level cache hierarchy where there exist

- **L1 Instruction Cache (L1I)**: 1KB, Direct-Mapped, 64B Line Size
- **L1 Data Cache (L1D)**: 1KB, Direct-Mapped, 64B Line Size
- **Unified L2 Cache (L2)**: 8KB, 4-Way SA, **Inclusive**, LRU, 64B Line Size

All the caches are write-back caches; of course, there will be no writebacks from `L1I` though.

### Inclusive L2

Inclusive L2 means that all the cache blocks that reside in L1 (L1I & L1D) are also in L2. That is, L2 is **inclusive** of L1. To implement inclusive L2, you need to implement the following.

#### L2 Cache Miss

If a memory request misses in L2, you need to bring the new block from memory and place it in **both L1 and L2**. Note that an L2 miss naturally implies that L1 does not have the requested block either. 
>For the L1/L2 write miss, it is first treated as a read miss from the core. And, we bring the data from memory into L1/L2, then the pending write is committed. This makes the L1 block dirty and L2 block clean.

#### L1 Cache Miss + L2 Cache Hit

You can simply bring the cache block from L2 to L1 in this case. Even if the L2 block is dirty, the newly installed block in L1 will be clean; because the L1 and L2 blocks are the same, you do not have to write back the L1 block to L2 on its eviction later. Once there is a write on the L1 block, the L1 block becomes dirty, and it should be written back to L2 on its eviction.

#### L1 Eviction

* If the evicted L1 block is **dirty**,  the victim block needs to be moved to L2. In this case, you do not update the LRU stack of the L2 cache; because that is not a cache access due to the memory reference. 
* Writeback does not contribute the number of accesses/hits/misses. Note that the number of L2 accesses is the same as the number of L1 misses.

#### L2 Eviction

On an L2 eviction, if there is the same cache block in L1, the L1 block needs to be invalidated, in order not to violate inclusion. This is called **back-invalidation**. Note that the invalidated L1 block could be dirty. If that is the case, the invalidated L1 block needs to be written back to **memory** (not to L2), and you need to increment writebacks.

### Submission for Extra Credits

Please make another submission with ID_NAME_lab4_extra.zip in `[Lab 4] Cache Simulation (Extra Credits)`.

The zipped file needs to include the following:

* Your code that we can compile and run
* A half-page document with (1) the following numbers and (2) how we can run your simulator.
  * Hit rates for L1I, L1D, L2
  * The number of cache blocks that are invalidated in L1I and L1D due to back invalidation.
  * The number of writebacks from L1D to memory due to back invalidation

Note that you need to modify the lab code a bit by yourself. We will only test your code for the cache parameters above.
