//
// Created by admin on 2022/2/10.
//

#ifndef C_REDIS_STUDY_ZMALLOC_H
#define C_REDIS_STUDY_ZMALLOC_H

#include <malloc/malloc.h>

#define HAVE_MALLOC_SIZE 1
#define zmalloc_size(p) malloc_size(p)

void zmall0c_set_oom_handler(void (*omm_handler)(size_t));

void *zmalloc(size_t size);

void *zcalloc(size_t size);

void *zrealloc(void *ptr, size_t size);

void zfree(void *ptr);

char *zstrdup(const char *s);

size_t zmalloc_used_memory(void);

void zmalloc_enable_thread_safeness(void);

float zmalloc_get_fragmentation_ratio(size_t rss);

size_t zmalloc_get_rss(void);

size_t zmalloc_get_provate_dirty(void);

#endif //C_REDIS_STUDY_ZMALLOC_H
