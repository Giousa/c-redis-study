//
// Created by admin on 2022/2/11.
//

#include <malloc/malloc.h>

#ifndef C_REDIS_STUDY_SDS_H
#define C_REDIS_STUDY_SDS_H

typedef char *sds;

struct sdshdr {
    int len;
    int free;
    char buf[];
};

sds sdsnewlen(const void *init, size_t initlen);
sds sdsnew(const char *init);


#endif //C_REDIS_STUDY_SDS_H
