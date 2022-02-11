//
// Created by admin on 2022/2/11.
//

#include "sds.h"
#include "zmalloc.h"
#include "string.h"
#include "stdio.h"

/**
 * 创建字符串
 */
sds sdsnewlen(const void *init, size_t initlen) {

    struct sdshdr *sh;
    if (init) {
        printf("存在初始化内容，不初始化分配内存. zmalloc\n");
        //这个1，是为了结尾\0准备的。
        //结构： len free buf
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
    printf("创建新的sds. sds地址：%p,sh地址：%p\n", sh->buf, sh);
    return sh->buf;
}

/**
 * 创建一个新的sds字符型
 */
sds sdsnew(const char *init) {
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}

/**
 * 创建一个空的字符串
 */
sds sdsempty(void) {
    return sdsnewlen("", 0);
}

/**
 * 参考sdsnewlen
 * sdshdr结构体的内存分配：len free buf --> int int buf[x]
 * sds的地址，是buf[x]首位地址
 * 当减去sizeof(struct sdshdr)，主要是int int 占用的地址
 * 就是sh首位地址
 */
size_t sdslen(const sds s) {
    printf("s1 len = %lu\n", strlen(s));
    printf("s1 = %p\n", s);
    printf("s2 = %p\n", s - (sizeof(struct sdshdr)));
    struct sdshdr *sh = (void *) (s - (sizeof(struct sdshdr)));
    //sh len: 11,free: 0,buf: hello world
    printf("sh shp: %p,len: %d,free: %d,buf: %s \n", sh, sh->len, sh->free, sh->buf);
    return sh->len;
}

/**
 * 复制sds
 */
sds sdsup(const sds s) {
    return sdsnewlen(s, sdslen(s));
}

/**
 * 释放sds
 */
void sdsfree(sds s) {
    if (s == NULL) return;
    zfree(s - sizeof(struct sdshdr));
}

/**
 * 返回sds可用空间长度
 * @param s
 * @return
 */
size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void *) (s - sizeof(struct sdshdr));
    return sh->free;
}

/**
 * 对 sds 中 buf 的长度进行扩展，确保在函数执行之后，
 * buf 至少会有 addlen + 1 长度的空余空间
 * （额外的 1 字节是为 \0 准备的）
 */
sds sdsMakeRoomFor(sds s, size_t addlen) {
    struct sdshdr *sh, *newsh;

    size_t free = sdsavail(s);
    //空余空间已足够，无需拓展
    if (free > addlen) return s;

    size_t len, newlen;
    //s当前占用空间
    len = sdslen(s);
    sh = (void *) (s - sizeof(struct sdshdr));
    //s 至少需要的长度
    newlen = len + addlen;

    if (newlen < SDS_MAX_PREALLOC) {
        newlen *= 2;
    } else {
        newlen += SDS_MAX_PREALLOC;
    }

    newsh = zrealloc(sh, sizeof(struct sdshdr) + newlen + 1);
    //内存不足，分配失败，返回
    if (newsh == NULL) return NULL;

    //剩余空间，就是新的长度，减去占用长度
    newsh->free = newlen - len;

    return newsh->buf;
}

/**
 * 将 sds 扩充至指定长度，未使用的空间以 0 字节填充。
 * @param s
 * @param len
 * @return
 */
sds sdsgrowzero(sds s, size_t len) {
    struct sdshdr *sh = (void *) (s - sizeof(struct sdshdr));
    size_t totlen, curlen = sh->len;

    //如果len比字符串长度小，不做操作
    if (len <= curlen) return s;

    //扩展sds
    s = sdsMakeRoomFor(s, len - curlen);
    if (s == NULL) return NULL;

    sh = (void *) (s - (sizeof(struct sdshdr)));
    memset(s + curlen, 0, (len - curlen + 1));

    //更新属性
    totlen = sh->len + sh->free;
    sh->len = len;
    sh->free = totlen - sh->len;

    return s;
}


/**
 * 字符串后面追加数据
 */
sds sdscatlen(sds s, const void *t, size_t len) {
    struct sdshdr *sh;

    //原有长度
    size_t curlen = sdslen(s);

    //拓展sds
    s = sdsMakeRoomFor(s, len);
    if (s == NULL) return NULL;

    //追加数据
    sh = (void *) (s - (sizeof(struct sdshdr)));
    memcpy(s + curlen, t, len);

    //更新属性
    sh->len = curlen + len;
    sh->free = sh->free - len;
    //末尾追加\0
    s[curlen + len] = '\0';
    return s;
}

/**
 * 追加数据
 */
sds sdscat(sds s, const void *t) {
    return sdscatlen(s, t, strlen(t));
}

/**
 * 追加sds
 */
sds sdscatsds(sds s, const sds t) {
    return sdscatlen(s, t, sdslen(t));
}

/**
 * 将字符串 t 的前 len 个字符复制到 sds s 当中，并在字符串的最后添加终结符。
 * 注意：会直接替换字符串，不是追加
 */
sds sdscpylen(sds s, const char *t, size_t len) {
    struct sdshdr *sh = (void *) (s - (sizeof(struct sdshdr)));

    size_t totlen = sh->free + sh->len;
    if (totlen < len) {
        s = sdsMakeRoomFor(s, len - sh->len);
        if (s == NULL) return NULL;
        sh = (void *) (s - (sizeof(struct sdshdr)));
        totlen = sh->free + sh->len;
    }

    memcpy(s, t, len);
    s[len] = '\0';

    sh->len = len;
    sh->free = totlen - len;

    return s;
}