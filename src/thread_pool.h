#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stddef.h>
#include <pthread.h>

typedef struct ThreadPool ThreadPool;

/* Create a pool with `num_threads` worker threads.
   `local_size` bytes are allocated for each thread's private storage. */
ThreadPool* thread_pool_create(int num_threads, size_t local_size);

/* Destroy the pool. Waits for workers to finish. */
void thread_pool_destroy(ThreadPool* pool);

/* Execute `num_tasks` tasks in parallel.
   `worker_func(thread_id, task_ptr, local_data)` is called for each task.
   Blocks until all tasks finish. */
void thread_pool_execute(ThreadPool* pool, void** tasks, int num_tasks,
                         void (*worker_func)(int thread_id, void* task, void* local));

/* Return the local storage for a specific thread (0 .. num_threads-1). */
void* thread_pool_get_local_by_id(ThreadPool* pool, int thread_id);

#endif /* THREAD_POOL_H */
