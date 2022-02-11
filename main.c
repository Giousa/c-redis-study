#include <stdio.h>
#include "string.h"
#include "src/zmalloc.h"
#include "src/sds.h"


int main() {
    printf("Hello, World!\n");

    char *p = "helloworld";
    sds s = sdsnew(p);
    char *t = "123";
    sds s2 = sdscpylen(s, t, 3);
//    printf("s2 len=%d,free=%d,buf=%s\n",s2)
    printf("s=%s\n", s);
    printf("s2=%s\n", s2);

    return 0;
}

void zmalloc_test() {

    char *ptr = zmalloc(10);
    strcpy(ptr, "helloworld");
    printf("打印获取分配内存地址：ptr = %p,pvalue = %s\n", ptr, ptr);
    zfree(ptr);
    strcat(ptr, "1234567890");
    printf("end：ptr = %p,pvalue = %s\n", ptr, ptr);
    printf("长度: %d\n", strlen(ptr));
    char *newptr = zstrdup(ptr);
    printf("newptr = %p,newvalue = %s,ptr = %p,pvalue = %s.\n", newptr, newptr, ptr, ptr);

    printf("当前使用内存：%zu\n", zmalloc_used_memory());

    zmalloc_enable_thread_safeness();
    printf("打开安全性能，当前使用内存：%zu\n", zmalloc_used_memory());

    printf("内存使用碎片：%zu\n", zmalloc_get_rss());

    printf("内存使用率：%f\n", zmalloc_get_fragmentation_ratio(zmalloc_get_rss()));
}
