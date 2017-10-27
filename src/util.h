/*
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef __REDIS_UTIL_H
#define __REDIS_UTIL_H

#include "sds.h"

int stringmatchlen(const char *p, int plen, const char *s, int slen, int nocase);//支持glob-style的通配符格式
int stringmatch(const char *p, const char *s, int nocase);
long long memtoll(const char *p, int *err);//将表示内存数量的字符串转换成数字*字节
int ll2string(char *s, size_t len, long long value);//将长时间转换成字符串。返回的数量需要代表数字的字符，如果通过，可以更短缓冲区长度不足以存储整个数字。
int string2ll(const char *s, size_t slen, long long *value);//将一个字符串转换成很长的时间。如果字符串可以被解析，则返回1进入(非溢出)的长，否则为0。该值将被设置为适当时的解析值。
int string2l(const char *s, size_t slen, long *value);//将一个字符串转换成long。如果字符串可以被解析为(非溢出)long，否则为0。该值将被设置为解析在适当的价值。
int d2string(char *buf, size_t len, double value);//将double转换为字符串表示形式。返回所需的字节数。这个表示法应该始终可以用stdtod(3)来表示。
sds getAbsolutePath(char *filename);//给定文件名，将绝对路径作为SDS字符串返回，如果由于某种原因失败，则返回NULL。
int pathIsBaseName(char *path);//检查在指定路径中是否存在/或字符。

#endif
