# Hash Hash Hash
This CS111 Lab 3 focuses on implementing a thread-safe hash table using pthread mutexes to enable concurrent access while maintaining correctness and performance. Starting with a base hash table that uses separate chaining for collision resolution, two new implementations are developed: V1, which prioritizes correctness using a single global lock, and V2, which optimizes for performance and correctness. The goal is to evaluate the trade-offs between synchronization overhead and parallel efficiency while ensuring data integrity in a multi-threaded environment.

## Building
```shell
make
```

## Running
```shell
./hash-table-tester -t 6 -s 100000
```

## First Implementation
In the `hash_table_v1_add_entry` function, I added a `pthread_mutex_lock()` at the start to ensure exclusive access when modifying the hash table and its linked lists, preventing race conditions in a multi-threaded environment. After determining the appropriate bucket using `bernstein_hash()`, the function checks if the key already exists; if so, it updates the value and releases the lock immediately. Otherwise, it allocates a new entry, inserts it at the head of the linked list, and unlocks the mutex. The mutex itself is a single global lock, declared within the `hash_table_v1` struct, initialized in `hash_table_v1_create()`, and destroyed in `hash_table_v1_destroy()`. While this approach guarantees correctness, it introduces performance bottlenecks, as multiple threads cannot modify different buckets simultaneously, making it slower than a fine-grained locking strategy.

### Performance
```
./hash-table-tester -t 6 -s 100000
```
```shell
Generation: 278,883 usec
Hash table base: 5,392,193 usec
  - 0 missing
Hash table v1: 21,790,918 usec
  - 0 missing

Generation: 253,937 usec
Hash table base: 6,132,884 usec
  - 0 missing
Hash table v1: 21,719,435 usec
  - 0 missing

Generation: 421,432 usec
Hash table base: 6,600,711 usec
  - 0 missing
Hash table v1: 22,394,057 usec
  - 0 missing
```
Version 1 is a little slower than the base version because it uses a single global mutex to control access to the entire hash table, forcing all threads to serialize their access during insertions. This locking mechanism prevents concurrent modifications even when different threads are targeting separate buckets, causing significant contention and reducing parallel efficiency. As a result, threads often spend time waiting for the mutex to become available instead of executing operations concurrently. The increased synchronization overhead introduced by this global lock outweighs the potential speedup from multi-threading, making Version 1 slower compared to the base implementation, which executes operations serially but without any locking overhead.

## Second Implementation
In the `hash_table_v2_add_entry` function, I used a fine-grained locking strategy by associating a separate mutex with each hash bucket to allow concurrent access to different parts of the table. Before modifying a bucket, the function locks the corresponding mutex based on the hash index and releases it immediately after the update is complete. This approach minimizes contention compared to a single global lock by enabling multiple threads to insert entries into different buckets simultaneously. The mutexes are initialized in `hash_table_v2_create()` and properly destroyed in `hash_table_v2_destroy()`, ensuring that all resources are managed correctly.

### Performance
```
./hash-table-tester -t 6 -s 100000
```
```shell
Generation: 465,483 usec
Hash table base: 10,020,504 usec
  - 0 missing
Hash table v1: 39,268,921 usec
  - 0 missing
Hash table v2: 3,861,202 usec
  - 0 missing

Generation: 467,993 usec
Hash table base: 11,361,845 usec
  - 0 missing
Hash table v1: 37,363,888 usec
  - 0 missing
Hash table v2: 3,532,489 usec
  - 0 missing

Generation: 467,993 usec
Hash table base: 11,361,845 usec
  - 0 missing
Hash table v1: 37,363,888 usec
  - 0 missing
Hash table v2: 3,532,489 usec
  - 0 missing

Generation: 467,293 usec
Hash table base: 13,204,154 usec
  - 0 missing
Hash table v1: 39,704,780 usec
  - 0 missing
Hash table v2: 3,480,354 usec
  - 0 missing
```
Missing entries in the hash table happen because of race conditions when multiple threads try to access and modify the same parts of the table at the same time. If two threads attempt to update the linked lists within the hash table simultaneously without proper synchronization, one thread's changes might not be visible to the other. This can cause issues like multiple nodes being added to the head of the list at once, breaking the list's structure and making it impossible to find some entries. Another issue is that two entries with the same key might end up in the same list without one update overwriting the other, leaving duplicate keys. To prevent these problems, each hash table entry has its own lock, ensuring that only one thread can update a specific linked list at a time. This approach is faster than using a single global lock, as it allows multiple threads to update different parts of the hash table simultaneously, as long as they aren't trying to access the same entry. This fine-grained locking strategy reduces wait times and makes Version 2 more efficient than Version 1. 

The results show that the speedup is about 3 to 4 times faster than the base implementation. The results could be slower than expected because many students may be running their code on the server as well. This is with a high number of elements and on 6 threads running on 6 cores. Since it is ran on 6 cores from the linux VM, here are some results where the thread count is smaller than the number of processors. In this case you can see the results are only 2 to 3 times faster than the base implementation.
```
./hash-table-tester -t 4 -s 100000
```
```
Generation: 171,384 usec
Hash table base: 1,972,882 usec
  - 0 missing
Hash table v1: 9,001,599 usec
  - 0 missing
Hash table v2: 921,331 usec
  - 0 missing

Generation: 171,463 usec
Hash table base: 2,312,591 usec
  - 0 missing
Hash table v1: 9,744,062 usec
  - 0 missing
Hash table v2: 1,080,818 usec
  - 0 missing

Generation: 171,246 usec
Hash table base: 2,063,248 usec
  - 0 missing
Hash table v1: 9,068,264 usec
  - 0 missing
Hash table v2: 733,614 usec
  - 0 missing
```
## Cleaning up
```shell
make clean
```