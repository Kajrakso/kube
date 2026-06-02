#include "thread_pool.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

#define QUEUE_CAPACITY 4096

/* Per-worker context passed at thread creation. */
typedef struct {
    int          thread_id;
    ThreadPool*  pool;
    void*        local;
} WorkerContext;

struct ThreadPool {
    int            num_threads;
    pthread_t*     threads;
    WorkerContext* contexts;

    void*  local_buf;
    size_t local_size;

    /* Shared task queue (ring buffer). */
    void*  queue[QUEUE_CAPACITY];
    int    queue_count;
    int    queue_head;

    /* Current batch info. */
    void (*worker_func)(int, void*, void*);
    int    tasks_remaining;

    pthread_mutex_t mutex;
    pthread_cond_t  work_avail; /* signalled when new tasks arrive */
    pthread_cond_t  work_done;  /* signalled when all tasks complete */
    bool            shutdown;
};

static void* worker_loop(void* arg) {
    WorkerContext* ctx = (WorkerContext*)arg;
    ThreadPool*   pool = ctx->pool;

    pthread_mutex_lock(&pool->mutex);

    while (!pool->shutdown) {
        /* Wait until there is work, or we are shutting down. */
        while (pool->queue_count == 0 && !pool->shutdown)
            pthread_cond_wait(&pool->work_avail, &pool->mutex);

        if (pool->shutdown)
            break;

        /* Grab one task from the queue. */
        void* task = pool->queue[pool->queue_head];
        pool->queue_head = (pool->queue_head + 1) % QUEUE_CAPACITY;
        pool->queue_count--;

        pthread_mutex_unlock(&pool->mutex);

        /* Execute the task. */
        pool->worker_func(ctx->thread_id, task, ctx->local);

        /* Mark completion. */
        pthread_mutex_lock(&pool->mutex);
        pool->tasks_remaining--;

        if (pool->tasks_remaining == 0)
            pthread_cond_signal(&pool->work_done);
    }

    pthread_mutex_unlock(&pool->mutex);
    return NULL;
}

ThreadPool* thread_pool_create(int num_threads, size_t local_size) {
    ThreadPool* pool = calloc(1, sizeof(ThreadPool));
    pool->num_threads = num_threads;
    pool->local_size  = local_size;
    pool->local_buf   = calloc((size_t)num_threads, local_size);
    pool->threads     = calloc((size_t)num_threads, sizeof(pthread_t));
    pool->contexts    = calloc((size_t)num_threads, sizeof(WorkerContext));

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->work_avail, NULL);
    pthread_cond_init(&pool->work_done,  NULL);

    for (int i = 0; i < num_threads; i++) {
        pool->contexts[i].thread_id = i;
        pool->contexts[i].pool      = pool;
        pool->contexts[i].local     = (char*)pool->local_buf
                                     + (size_t)i * local_size;
        pthread_create(&pool->threads[i], NULL, worker_loop,
                       &pool->contexts[i]);
    }

    return pool;
}

void thread_pool_destroy(ThreadPool* pool) {
    pthread_mutex_lock(&pool->mutex);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->work_avail);
    pthread_mutex_unlock(&pool->mutex);

    for (int i = 0; i < pool->num_threads; i++)
        pthread_join(pool->threads[i], NULL);

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->work_avail);
    pthread_cond_destroy(&pool->work_done);

    free(pool->threads);
    free(pool->contexts);
    free(pool->local_buf);
    free(pool);
}

void thread_pool_execute(ThreadPool* pool, void** tasks, int num_tasks,
                         void (*worker_func)(int, void*, void*)) {
    pthread_mutex_lock(&pool->mutex);

    pool->worker_func     = worker_func;
    pool->tasks_remaining = num_tasks;
    pool->queue_count     = num_tasks;
    pool->queue_head      = 0;

    for (int i = 0; i < num_tasks; i++)
        pool->queue[i] = tasks[i];

    pthread_cond_broadcast(&pool->work_avail);

    while (pool->tasks_remaining > 0)
        pthread_cond_wait(&pool->work_done, &pool->mutex);

    pthread_mutex_unlock(&pool->mutex);
}

void* thread_pool_get_local_by_id(ThreadPool* pool, int thread_id) {
    return (char*)pool->local_buf + (size_t)thread_id * pool->local_size;
}
