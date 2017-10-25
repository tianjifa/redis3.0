/* anet.c -- Basic TCP socket stuff made a bit less boring
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef ANET_H
#define ANET_H

#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

/* Flags used with certain functions. */
#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

#if defined(__sun)
#define AF_LOCAL AF_UNIX
#endif

int anetTcpConnect(char *err, char *addr, int port);//创建阻塞 TCP 连接
int anetTcpNonBlockConnect(char *err, char *addr, int port);//创建非阻塞 TCP 连接
int anetTcpNonBlockBindConnect(char *err, char *addr, int port, char *source_addr);//创建非阻塞 TCP 连接
int anetUnixConnect(char *err, char *path);//创建阻塞本地连接
int anetUnixNonBlockConnect(char *err, char *path);//创建非阻塞本地连接
int anetRead(int fd, char *buf, int count);//带 short count 处理的读取函数
int anetResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len);//解析所有的东西
int anetResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len);//单单解析IP的地址
int anetTcpServer(char *err, int port, char *bindaddr, int backlog);//创建socket的server
int anetTcp6Server(char *err, int port, char *bindaddr, int backlog);//创建只能发送和接收ipv6的socket的server
int anetUnixServer(char *err, char *path, mode_t perm, int backlog);//创建unix域的的socket的server
int anetTcpAccept(char *err, int serversock, char *ip, size_t ip_len, int *port);//获得tcp连接请求并建立连接
int anetUnixAccept(char *err, int serversock);//获得unix域连接请求并建立连接
int anetWrite(int fd, char *buf, int count);//对write()的简单封装
int anetNonBlock(char *err, int fd);//将 fd 设置为非阻塞模式（O_NONBLOCK）
int anetEnableTcpNoDelay(char *err, int fd);//禁用 Nagle 算法
int anetDisableTcpNoDelay(char *err, int fd);//启用 Nagle 算法
int anetTcpKeepAlive(char *err, int fd);//开启 TCP 的 keep alive 选项
int anetPeerToString(int fd, char *ip, size_t ip_len, int *port);//获取连接客户端的 IP 和端口号
int anetKeepAlive(char *err, int fd, int interval);//修改 TCP 连接的 keep alive 选项
int anetSockName(int fd, char *ip, size_t ip_len, int *port);//获取服务器本机的 IP 和端口号

#endif
