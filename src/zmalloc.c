//
// Created by admin on 2022/2/10.
//

#include <mach/mach_traps.h>
#include <mach/mach_init.h>
#include <unistd.h>
#include <mach/task.h>
#include "zmalloc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"

#define PREFIX_SIZE (0)
#define PREFIX_SIZE2 (1)

#define update_zmalloc_stat_add(__n) __sync_add_and_fetch(&used_memory, (__n))
#define update_zmalloc_stat_sub(__n) __sync_sub_and_fetch(&used_memory, (__n))


static int zmalloc_thread_safe = 0;
static size_t used_memory = 0;

/**
 * 分配内存
 */
#define update_zmalloc_stat_alloc(__n) do { \
    printf("update_zmalloc_stat_alloc __n = %d\n",__n);                                        \
    size_t _n = (__n); \
    if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); \
    printf("update_zmalloc_stat_alloc _n = %d\n",_n);                                        \
    if (zmalloc_thread_safe) {              \
        printf("update_zmalloc_stat_alloc update_zmalloc_stat_add.\n");                                 \
        update_zmalloc_stat_add(_n); \
    } else {                                \
        printf("update_zmalloc_stat_alloc used_memory add.\n");                                    \
        used_memory += _n; \
    } \
} while(0)


/**
 * 释放内存
 */
#define update_zmalloc_stat_free(__n) do { \
    size_t _n = (__n); \
    if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); \
    if (zmalloc_thread_safe) { \
        update_zmalloc_stat_sub(_n); \
    } else { \
        used_memory -= _n; \
    } \
} while(0)

static void zmalloc_default_oom(size_t size) {
    fprintf(stderr, "zmalloc: Out of memory trying to allocate %zu bytes\n", size);
    fflush(stderr);
    abort();
}

/**
 * 函数指针
 * typedef int (*fun_ptr)(int,int); 声明一个指向同样参数、返回值的函数指针类型
 * void (*p)(int)
 */
static void (*zmalloc_oom_handler)(size_t) = zmalloc_default_oom;

void zmall0c_set_oom_handler(void (*omm_handler)(size_t)) {
    zmalloc_oom_handler = omm_handler;
    printf("zmalloc_oom_handler = %d\n", zmalloc_oom_handler);
}

void *zmalloc(size_t size) {
    printf("zmalloc wait size = %d\n", size);
//    printf("PREFIX_SIZE = %d\n", PREFIX_SIZE);
//    printf("PREFIX_SIZE2 = %d\n", PREFIX_SIZE2);
    void *ptr = malloc(size);
//    void *ptr = NULL;
//    printf("zmalloc ptr = %p\n", ptr);
    if (!ptr) {
//        printf("指针p为空\n");
        zmalloc_oom_handler(size);
    } else {
//        printf("指针p不为空\n");
    }
#ifdef HAVE_MALLOC_SIZE
//    malloc_size(ptr);
//    printf("HAVE_MALLOC_SIZE has defiend. \n");
//    size_t __n = zmalloc_size(ptr);
//    printf("__n = %d\n", __n);
//    printf("sizeof(long) -1 = %d\n", (sizeof(long) - 1));
//    printf("222 = %d\n", __n & (sizeof(long) - 1));
//    printf("used_memory_pre = %d\n", used_memory);
    update_zmalloc_stat_alloc(zmalloc_size(ptr));
//    printf("used_memory_now = %d\n", used_memory);
    return ptr;
#else
    printf("没有定义:HAVE_MALLOC_SIZE \n");
#endif
}

void *zcalloc(size_t size) {
    printf("zcalloc wait size = %d\n", size);
//    printf("zcalloc real size = %lu\n", (size + PREFIX_SIZE));
    void *ptr = calloc(1, size + PREFIX_SIZE);
    if (!ptr) {
//        printf("zcalloc ptr指针为空");
        zmalloc_oom_handler(size);
    }
//    printf("实际分配内存：%d\n", malloc_size(ptr));
#ifdef HAVE_MALLOC_SIZE
    update_zmalloc_stat_alloc(zmalloc_size(ptr));
#else
    printf("HAVE_MALLOC_SIZE 未定义....");
#endif
}

void *zrealloc(void *ptr, size_t size) {
#ifndef HAVE_MALLOC_SIZE
    void *realptr;
#endif
    size_t oldsize;
    void *newptr;
    if (ptr == NULL) return zmalloc(size);

#ifdef HAVE_MALLOC_SIZE
    oldsize = zmalloc_size(ptr);
    newptr = realloc(ptr, size);
    if (!newptr) {
        zmalloc_oom_handler(size);
    }
    update_zmalloc_stat_free(oldsize);
    update_zmalloc_stat_alloc(zmalloc_size(newptr));
    return newptr;
#endif
}

void zfree(void *ptr) {
    if (ptr == NULL) return;
    update_zmalloc_stat_free(zmalloc_size(ptr));
    free(ptr);
}

char *zstrdup(const char *s) {
    size_t l = strlen(s) + 1;
    char *p = zmalloc(l);
    memcpy(p, s, l);
    return p;
}

size_t zmalloc_used_memory(void) {
    if (zmalloc_thread_safe) {
        printf("zmalloc_used_memory zmalloc_thread_safe is 1.\n");
        return __sync_add_and_fetch(&used_memory, 0);
    }
    printf("zmalloc_used_memory zmalloc_thread_safe is 0.\n");
    return used_memory;
}

void zmalloc_enable_thread_safeness(void) {
    zmalloc_thread_safe = 1;
}

size_t zmalloc_get_rss(void) {
    task_t task = MACH_PORT_NULL;
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    if (task_for_pid(current_task(), getpid(), &task) != KERN_SUCCESS) {
        return 0;
    }
    task_info(task, TASK_BASIC_INFO, (task_info_t) &t_info, &t_info_count);
    return t_info.resident_size;
}

float zmalloc_get_fragmentation_ratio(size_t rss) {
    return (float) rss / zmalloc_used_memory();
}

size_t zmalloc_get_provate_dirty(void) {
    return 0;
}