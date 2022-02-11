//
// Created by admin on 2022/2/11.
//

#include "sds.h"
#include "zmalloc.h"
#include "string.h"
#include "stdio.h"

sds sdsnewlen(const void *init, size_t initlen) {

    struct sdshdr *sh;
    if (init) {
        printf("存在初始化内容，不初始化分配内存. zmalloc\n");
        sh = zmalloc(sizeof(struct sdshdr) + initlen + 1);
    } else {
        printf("无初始化内容，初始化内存分配. zcalloc\n");
        sh = zcalloc(sizeof(struct sdshdr) + initlen + 1);
    }

    if (sh == NULL) return NULL;

    sh->len = initlen;
    sh->free = 0;

    if (initlen && init) {
        memcpy(sh->buf, init, initlen);
    }
    sh->buf[initlen] = '\0';
    return sh->buf;
}

sds sdsnew(const char *init) {
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}