/* SDSLib, A C dynamic strings library
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SDS_H
#define __SDS_H

/*
 * 最大预分配长度
 */
#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>

/*
 * 类型别名，用于指向 sdshdr 的 buf 属性，即将char*起个别名shs
 */
typedef char *sds;

/*
 * 保存字符串对象的结构
 */
struct sdshdr {
    
    // buf 中已占用空间的长度
    int len;

    // buf 中剩余可用空间的长度
    int free;

    // 数据空间
    char buf[];
};

/*
 * 返回 sds 实际保存的字符串的长度
 *
 * T = O(1)
 */
static inline size_t sdslen(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

/*
 * 返回 sds 可用空间的长度
 *
 * T = O(1)
 */
static inline size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->free;
}

sds sdsnewlen(const void *init, size_t initlen);//创建一个指定长度的sds，接受一个指定的C字符串作为初始化值
sds sdsnew(const char *init);//根据给定的C字符串，创建一个相应的sds
sds sdsempty(void);//创建一个只包含空字符串””的sds
size_t sdslen(const sds s);//返回 sds 实际保存的字符串的长度
sds sdsdup(const sds s);//复制给定的sds
void sdsfree(sds s);//释放给定的sds
size_t sdsavail(const sds s);//返回 sds 可用空间的长度
sds sdsgrowzero(sds s, size_t len);//将给定的sds扩展到指定的长度，空余的部分用\0进行填充
sds sdscatlen(sds s, const void *t, size_t len);//将一个C字符串追加到给定的sds对应sdshdr的buf
sds sdscat(sds s, const char *t);//将给定字符串 t 追加到 sds 的末尾
sds sdscatsds(sds s, const sds t);//将另一个 sds 追加到一个 sds 的末尾
sds sdscpylen(sds s, const char *t, size_t len);//将一个C字符串复制到sds中，需要依据sds的总长度来判断是否需要扩展
sds sdscpy(sds s, const char *t);//将字符串复制到 sds 当中，覆盖原有的字符。

sds sdscatvprintf(sds s, const char *fmt, va_list ap);//打印函数，被 sdscatprintf 所调用
#ifdef __GNUC__
sds sdscatprintf(sds s, const char *fmt, ...)//通过格式化输出形式，来追加到给定的sds
    __attribute__((format(printf, 2, 3)));
#else
sds sdscatprintf(sds s, const char *fmt, ...);
#endif

sds sdscatfmt(sds s, char const *fmt, ...);//sdscatprintf性能优化版本
sds sdstrim(sds s, const char *cset);//对给定sds，删除前端/后端在给定的C字符串中的字符
void sdsrange(sds s, int start, int end);//截取给定sds，[start,end]字符串
void sdsupdatelen(sds s);//更新sds字符串内部状态数据(字符长度，剩余空间)
void sdsclear(sds s);//清除给定sds的buf，将buf初始化为””，同时修改对应sdshdr的free与len值
int sdscmp(const sds s1, const sds s2);//比较两个sds的大小
sds *sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count);//对给定的字符串s按照给定的sep分隔字符串来进行切割
void sdsfreesplitres(sds *tokens, int count);//释放sdssplitlen，sdssplitargs分割出来的sds字符串数组
void sdstolower(sds s);//sds字符串变更全小写
void sdstoupper(sds s);//sds字符串变更全大写
sds sdsfromlonglong(long long value);//将value转换为sds字符串
sds sdscatrepr(sds s, const char *p, size_t len);//将p指向的字符串转换为人可阅读的追加到sds后面
sds *sdssplitargs(const char *line, int *argc);//参数分割函数，返回arc大小的sds字符串数组
sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen);//sds字符串内按照[from, to]的字符map映射进行转换
sds sdsjoin(char **argv, int argc, char *sep);//将字符串用分隔符连接(sep)，返回sds字符串

/* Low level functions exposed to the user API */
sds sdsMakeRoomFor(sds s, size_t addlen);//对给定sds对应sdshdr的buf进行扩展
void sdsIncrLen(sds s, int incr);//对给定sds的buf的右端进行扩展或缩小
sds sdsRemoveFreeSpace(sds s);//在不改动sds的前提下，将buf的多余空间释放
size_t sdsAllocSize(sds s);//计算给定的sds所占的内存大小

#endif
