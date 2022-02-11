//
// Created by admin on 2022/2/11.
//

#include <malloc/malloc.h>

#ifndef C_REDIS_STUDY_SDS_H
#define C_REDIS_STUDY_SDS_H

#define SDS_MAX_PREALLOC (1024*1024)


typedef char *sds;

struct sdshdr {
    int len;
    int free;
    char buf[];
};

sds sdsnewlen(const void *init, size_t initlen);

sds sdsnew(const char *init);

sds sdsempty(void);

size_t sdslen(const sds s);

sds sdsup(const sds s);

void sdsfree(sds s);

size_t sdsavail(const sds s);

sds sdsMakeRoomFor(sds s, size_t addlen);

sds sdsgrowzero(sds s, size_t len);

sds sdscatlen(sds s, const void *t, size_t len);

sds sdscat(sds s,const void *t);

sds sdscatsds(sds s,const sds t);

sds sdscpylen(sds s,const char *t,size_t len);

#endif //C_REDIS_STUDY_SDS_H
