# Shared Semaphores

The code in this section illustrates the usage of shared semaphores using two different methods:

* Shared memory. The example in `anonymous_semaphores/` places a semaphore in shared memory using `mmap(2)` with the `MAP_SHARED` flag. Note that the processes sharing the semaphore in this case must be somehow related; independent processes could never get a reference to the shared memory segment unless `shm_open(3)` was used instead.

* Named semaphores. The interface is significantly more friendly, and there is no need to explicitly set up shared memory, deal with allocation and manually initialize the semaphore. The example in `named_semaphores/` sets up a pair of processes that synchronizes with named semaphores. A named semaphore is created and fetched with `sem_open(3)`. This code was developed as part of answering this question on StackOverflow: [Share POSIX semaphore among multiple processes](http://stackoverflow.com/q/32205396/2793118). For further details, check it out.
