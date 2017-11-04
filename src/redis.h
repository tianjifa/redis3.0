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

#ifndef __REDIS_H
#define __REDIS_H

#include "fmacros.h"
#include "config.h"

#if defined(__sun)
#include "solarisfixes.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <syslog.h>
#include <netinet/in.h>
#include <lua.h>
#include <signal.h>

#include "ae.h"      /* Event driven programming library */
#include "sds.h"     /* Dynamic safe strings */
#include "dict.h"    /* Hash tables */
#include "adlist.h"  /* Linked lists */
#include "zmalloc.h" /* total memory usage aware version of malloc/free */
#include "anet.h"    /* Networking the easy way */
#include "ziplist.h" /* Compact list data structure */
#include "intset.h"  /* Compact integer set structure */
#include "version.h" /* Version macro */
#include "util.h"    /* Misc functions useful in many places */

/* Error codes */
#define REDIS_OK                0
#define REDIS_ERR               -1

/* Static server configuration */
/* 默认的服务器配置值 */
#define REDIS_DEFAULT_HZ        10      /* Time interrupt calls/sec. */
#define REDIS_MIN_HZ            1
#define REDIS_MAX_HZ            500 
#define REDIS_SERVERPORT        6379    /* TCP port */
#define REDIS_TCP_BACKLOG       511     /* TCP listen backlog */
#define REDIS_MAXIDLETIME       0       /* default client timeout: infinite */
#define REDIS_DEFAULT_DBNUM     16
#define REDIS_CONFIGLINE_MAX    1024
#define REDIS_DBCRON_DBS_PER_CALL 16
#define REDIS_MAX_WRITE_PER_EVENT (1024*64)
#define REDIS_SHARED_SELECT_CMDS 10
#define REDIS_SHARED_INTEGERS 10000
#define REDIS_SHARED_BULKHDR_LEN 32
#define REDIS_MAX_LOGMSG_LEN    1024 /* Default maximum length of syslog messages */
#define REDIS_AOF_REWRITE_PERC  100
#define REDIS_AOF_REWRITE_MIN_SIZE (64*1024*1024)
#define REDIS_AOF_REWRITE_ITEMS_PER_CMD 64
#define REDIS_SLOWLOG_LOG_SLOWER_THAN 10000
#define REDIS_SLOWLOG_MAX_LEN 128
#define REDIS_MAX_CLIENTS 10000
#define REDIS_AUTHPASS_MAX_LEN 512
#define REDIS_DEFAULT_SLAVE_PRIORITY 100
#define REDIS_REPL_TIMEOUT 60
#define REDIS_REPL_PING_SLAVE_PERIOD 10
#define REDIS_RUN_ID_SIZE 40
#define REDIS_OPS_SEC_SAMPLES 16
#define REDIS_DEFAULT_REPL_BACKLOG_SIZE (1024*1024)    /* 1mb */
#define REDIS_DEFAULT_REPL_BACKLOG_TIME_LIMIT (60*60)  /* 1 hour */
#define REDIS_REPL_BACKLOG_MIN_SIZE (1024*16)          /* 16k */
#define REDIS_BGSAVE_RETRY_DELAY 5 /* Wait a few secs before trying again. */
#define REDIS_DEFAULT_PID_FILE "/var/run/redis.pid"
#define REDIS_DEFAULT_SYSLOG_IDENT "redis"
#define REDIS_DEFAULT_CLUSTER_CONFIG_FILE "nodes.conf"
#define REDIS_DEFAULT_DAEMONIZE 0
#define REDIS_DEFAULT_UNIX_SOCKET_PERM 0
#define REDIS_DEFAULT_TCP_KEEPALIVE 0
#define REDIS_DEFAULT_LOGFILE ""
#define REDIS_DEFAULT_SYSLOG_ENABLED 0
#define REDIS_DEFAULT_STOP_WRITES_ON_BGSAVE_ERROR 1
#define REDIS_DEFAULT_RDB_COMPRESSION 1
#define REDIS_DEFAULT_RDB_CHECKSUM 1
#define REDIS_DEFAULT_RDB_FILENAME "dump.rdb"
#define REDIS_DEFAULT_SLAVE_SERVE_STALE_DATA 1
#define REDIS_DEFAULT_SLAVE_READ_ONLY 1
#define REDIS_DEFAULT_REPL_DISABLE_TCP_NODELAY 0
#define REDIS_DEFAULT_MAXMEMORY 0
#define REDIS_DEFAULT_MAXMEMORY_SAMPLES 5
#define REDIS_DEFAULT_AOF_FILENAME "appendonly.aof"
#define REDIS_DEFAULT_AOF_NO_FSYNC_ON_REWRITE 0
#define REDIS_DEFAULT_ACTIVE_REHASHING 1
#define REDIS_DEFAULT_AOF_REWRITE_INCREMENTAL_FSYNC 1
#define REDIS_DEFAULT_MIN_SLAVES_TO_WRITE 0
#define REDIS_DEFAULT_MIN_SLAVES_MAX_LAG 10
#define REDIS_IP_STR_LEN INET6_ADDRSTRLEN
#define REDIS_PEER_ID_LEN (REDIS_IP_STR_LEN+32) /* Must be enough for ip:port */
#define REDIS_BINDADDR_MAX 16
#define REDIS_MIN_RESERVED_FDS 32

#define ACTIVE_EXPIRE_CYCLE_LOOKUPS_PER_LOOP 20 /* Loopkups per loop. */
#define ACTIVE_EXPIRE_CYCLE_FAST_DURATION 1000 /* Microseconds */
#define ACTIVE_EXPIRE_CYCLE_SLOW_TIME_PERC 25 /* CPU max % for keys collection */
#define ACTIVE_EXPIRE_CYCLE_SLOW 0
#define ACTIVE_EXPIRE_CYCLE_FAST 1

/* Protocol and I/O related defines */
#define REDIS_MAX_QUERYBUF_LEN  (1024*1024*1024) /* 1GB max query buffer. */
#define REDIS_IOBUF_LEN         (1024*16)  /* Generic I/O buffer size */
#define REDIS_REPLY_CHUNK_BYTES (16*1024) /* 16k output buffer */
#define REDIS_INLINE_MAX_SIZE   (1024*64) /* Max size of inline reads */
#define REDIS_MBULK_BIG_ARG     (1024*32)
#define REDIS_LONGSTR_SIZE      21          /* Bytes needed for long -> str */
// 指示 AOF 程序每累积这个量的写入数据
// 就执行一次显式的 fsync
#define REDIS_AOF_AUTOSYNC_BYTES (1024*1024*32) /* fdatasync every 32MB */
/* When configuring the Redis eventloop, we setup it so that the total number
 * of file descriptors we can handle are server.maxclients + RESERVED_FDS + FDSET_INCR
 * that is our safety margin. */
#define REDIS_EVENTLOOP_FDSET_INCR (REDIS_MIN_RESERVED_FDS+96)

/* Hash table parameters */
#define REDIS_HT_MINFILL        10      /* Minimal hash table fill 10% */

/* Command flags. Please check the command table defined in the redis.c file
 * for more information about the meaning of every flag. */
// 命令标志
#define REDIS_CMD_WRITE 1                   /* "w" flag */
#define REDIS_CMD_READONLY 2                /* "r" flag */
#define REDIS_CMD_DENYOOM 4                 /* "m" flag */
#define REDIS_CMD_NOT_USED_1 8              /* no longer used flag */
#define REDIS_CMD_ADMIN 16                  /* "a" flag */
#define REDIS_CMD_PUBSUB 32                 /* "p" flag */
#define REDIS_CMD_NOSCRIPT  64              /* "s" flag */
#define REDIS_CMD_RANDOM 128                /* "R" flag */
#define REDIS_CMD_SORT_FOR_SCRIPT 256       /* "S" flag */
#define REDIS_CMD_LOADING 512               /* "l" flag */
#define REDIS_CMD_STALE 1024                /* "t" flag */
#define REDIS_CMD_SKIP_MONITOR 2048         /* "M" flag */
#define REDIS_CMD_ASKING 4096               /* "k" flag */

/* Object types */
// 对象类型
#define REDIS_STRING 0
#define REDIS_LIST 1
#define REDIS_SET 2
#define REDIS_ZSET 3
#define REDIS_HASH 4

/* Objects encoding. Some kind of objects like Strings and Hashes can be
 * internally represented in multiple ways. The 'encoding' field of the object
 * is set to one of this fields for this object. */
// 对象编码
#define REDIS_ENCODING_RAW 0     /* Raw representation */
#define REDIS_ENCODING_INT 1     /* Encoded as integer */
#define REDIS_ENCODING_HT 2      /* Encoded as hash table */
#define REDIS_ENCODING_ZIPMAP 3  /* Encoded as zipmap */
#define REDIS_ENCODING_LINKEDLIST 4 /* Encoded as regular linked list */
#define REDIS_ENCODING_ZIPLIST 5 /* Encoded as ziplist */
#define REDIS_ENCODING_INTSET 6  /* Encoded as intset */
#define REDIS_ENCODING_SKIPLIST 7  /* Encoded as skiplist */
#define REDIS_ENCODING_EMBSTR 8  /* Embedded sds string encoding */

/* Defines related to the dump file format. To store 32 bits lengths for short
 * keys requires a lot of space, so we check the most significant 2 bits of
 * the first byte to interpreter the length:
 *
 * 00|000000 => if the two MSB are 00 the len is the 6 bits of this byte
 * 01|000000 00000000 =>  01, the len is 14 byes, 6 bits + 8 bits of next byte
 * 10|000000 [32 bit integer] => if it's 10, a full 32 bit len will follow
 * 11|000000 this means: specially encoded object will follow. The six bits
 *           number specify the kind of object that follows.
 *           See the REDIS_RDB_ENC_* defines.
 *
 * Lengths up to 63 are stored using a single byte, most DB keys, and may
 * values, will fit inside. */
#define REDIS_RDB_6BITLEN 0
#define REDIS_RDB_14BITLEN 1
#define REDIS_RDB_32BITLEN 2
#define REDIS_RDB_ENCVAL 3
#define REDIS_RDB_LENERR UINT_MAX

/* When a length of a string object stored on disk has the first two bits
 * set, the remaining two bits specify a special encoding for the object
 * accordingly to the following defines: */
#define REDIS_RDB_ENC_INT8 0        /* 8 bit signed integer */
#define REDIS_RDB_ENC_INT16 1       /* 16 bit signed integer */
#define REDIS_RDB_ENC_INT32 2       /* 32 bit signed integer */
#define REDIS_RDB_ENC_LZF 3         /* string compressed with FASTLZ */

/* AOF states */
#define REDIS_AOF_OFF 0             /* AOF is off */
#define REDIS_AOF_ON 1              /* AOF is on */
#define REDIS_AOF_WAIT_REWRITE 2    /* AOF waits rewrite to start appending */

/* Client flags */
#define REDIS_SLAVE (1<<0)   /* This client is a slave server */
#define REDIS_MASTER (1<<1)  /* This client is a master server */
#define REDIS_MONITOR (1<<2) /* This client is a slave monitor, see MONITOR */
#define REDIS_MULTI (1<<3)   /* This client is in a MULTI context */
#define REDIS_BLOCKED (1<<4) /* The client is waiting in a blocking operation */
#define REDIS_DIRTY_CAS (1<<5) /* Watched keys modified. EXEC will fail. */
#define REDIS_CLOSE_AFTER_REPLY (1<<6) /* Close after writing entire reply. */
#define REDIS_UNBLOCKED (1<<7) /* This client was unblocked and is stored in
                                  server.unblocked_clients */
#define REDIS_LUA_CLIENT (1<<8) /* This is a non connected client used by Lua */
#define REDIS_ASKING (1<<9)     /* Client issued the ASKING command */
#define REDIS_CLOSE_ASAP (1<<10)/* Close this client ASAP */
#define REDIS_UNIX_SOCKET (1<<11) /* Client connected via Unix domain socket */
#define REDIS_DIRTY_EXEC (1<<12)  /* EXEC will fail for errors while queueing */
#define REDIS_MASTER_FORCE_REPLY (1<<13)  /* Queue replies even if is master */
#define REDIS_FORCE_AOF (1<<14)   /* Force AOF propagation of current cmd. */
#define REDIS_FORCE_REPL (1<<15)  /* Force replication of current cmd. */
#define REDIS_PRE_PSYNC (1<<16)   /* Instance don't understand PSYNC. */
#define REDIS_READONLY (1<<17)    /* Cluster client is in read-only state. */

/* Client block type (btype field in client structure)
 * if REDIS_BLOCKED flag is set. */
#define REDIS_BLOCKED_NONE 0    /* Not blocked, no REDIS_BLOCKED flag set. */
#define REDIS_BLOCKED_LIST 1    /* BLPOP & co. */
#define REDIS_BLOCKED_WAIT 2    /* WAIT for synchronous replication. */

/* Client request types */
#define REDIS_REQ_INLINE 1
#define REDIS_REQ_MULTIBULK 2

/* Client classes for client limits, currently used only for
 * the max-client-output-buffer limit implementation. */
#define REDIS_CLIENT_LIMIT_CLASS_NORMAL 0
#define REDIS_CLIENT_LIMIT_CLASS_SLAVE 1
#define REDIS_CLIENT_LIMIT_CLASS_PUBSUB 2
#define REDIS_CLIENT_LIMIT_NUM_CLASSES 3

/* Slave replication state - from the point of view of the slave. */
#define REDIS_REPL_NONE 0 /* No active replication */
#define REDIS_REPL_CONNECT 1 /* Must connect to master */
#define REDIS_REPL_CONNECTING 2 /* Connecting to master */
#define REDIS_REPL_RECEIVE_PONG 3 /* Wait for PING reply */
#define REDIS_REPL_TRANSFER 4 /* Receiving .rdb from master */
#define REDIS_REPL_CONNECTED 5 /* Connected to master */

/* Slave replication state - from the point of view of the master.
 * In SEND_BULK and ONLINE state the slave receives new updates
 * in its output queue. In the WAIT_BGSAVE state instead the server is waiting
 * to start the next background saving in order to send updates to it. */
#define REDIS_REPL_WAIT_BGSAVE_START 6 /* We need to produce a new RDB file. */
#define REDIS_REPL_WAIT_BGSAVE_END 7 /* Waiting RDB file creation to finish. */
#define REDIS_REPL_SEND_BULK 8 /* Sending RDB file to slave. */
#define REDIS_REPL_ONLINE 9 /* RDB file transmitted, sending just updates. */

/* Synchronous read timeout - slave side */
#define REDIS_REPL_SYNCIO_TIMEOUT 5

/* List related stuff */
#define REDIS_HEAD 0
#define REDIS_TAIL 1

/* Sort operations */
#define REDIS_SORT_GET 0
#define REDIS_SORT_ASC 1
#define REDIS_SORT_DESC 2
#define REDIS_SORTKEY_MAX 1024

/* Log levels */
#define REDIS_DEBUG 0
#define REDIS_VERBOSE 1
#define REDIS_NOTICE 2
#define REDIS_WARNING 3
#define REDIS_LOG_RAW (1<<10) /* Modifier to log without timestamp */
#define REDIS_DEFAULT_VERBOSITY REDIS_NOTICE

/* Anti-warning macro... */
#define REDIS_NOTUSED(V) ((void) V)

#define ZSKIPLIST_MAXLEVEL 32 /* Should be enough for 2^32 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */

/* Append only defines */
#define AOF_FSYNC_NO 0
#define AOF_FSYNC_ALWAYS 1
#define AOF_FSYNC_EVERYSEC 2
#define REDIS_DEFAULT_AOF_FSYNC AOF_FSYNC_EVERYSEC

/* Zip structure related defaults */
#define REDIS_HASH_MAX_ZIPLIST_ENTRIES 512
#define REDIS_HASH_MAX_ZIPLIST_VALUE 64
#define REDIS_LIST_MAX_ZIPLIST_ENTRIES 512
#define REDIS_LIST_MAX_ZIPLIST_VALUE 64
#define REDIS_SET_MAX_INTSET_ENTRIES 512
#define REDIS_ZSET_MAX_ZIPLIST_ENTRIES 128
#define REDIS_ZSET_MAX_ZIPLIST_VALUE 64

/* HyperLogLog defines */
#define REDIS_DEFAULT_HLL_SPARSE_MAX_BYTES 3000

/* Sets operations codes */
#define REDIS_OP_UNION 0
#define REDIS_OP_DIFF 1
#define REDIS_OP_INTER 2

/* Redis maxmemory strategies */
#define REDIS_MAXMEMORY_VOLATILE_LRU 0
#define REDIS_MAXMEMORY_VOLATILE_TTL 1
#define REDIS_MAXMEMORY_VOLATILE_RANDOM 2
#define REDIS_MAXMEMORY_ALLKEYS_LRU 3
#define REDIS_MAXMEMORY_ALLKEYS_RANDOM 4
#define REDIS_MAXMEMORY_NO_EVICTION 5
#define REDIS_DEFAULT_MAXMEMORY_POLICY REDIS_MAXMEMORY_NO_EVICTION

/* Scripting */
#define REDIS_LUA_TIME_LIMIT 5000 /* milliseconds */

/* Units */
#define UNIT_SECONDS 0
#define UNIT_MILLISECONDS 1

/* SHUTDOWN flags */
#define REDIS_SHUTDOWN_SAVE 1       /* Force SAVE on SHUTDOWN even if no save
                                       points are configured. */
#define REDIS_SHUTDOWN_NOSAVE 2     /* Don't SAVE on SHUTDOWN. */

/* Command call flags, see call() function */
#define REDIS_CALL_NONE 0
#define REDIS_CALL_SLOWLOG 1
#define REDIS_CALL_STATS 2
#define REDIS_CALL_PROPAGATE 4
#define REDIS_CALL_FULL (REDIS_CALL_SLOWLOG | REDIS_CALL_STATS | REDIS_CALL_PROPAGATE)

/* Command propagation flags, see propagate() function */
#define REDIS_PROPAGATE_NONE 0
#define REDIS_PROPAGATE_AOF 1
#define REDIS_PROPAGATE_REPL 2

/* Keyspace changes notification classes. Every class is associated with a
 * character for configuration purposes. */
#define REDIS_NOTIFY_KEYSPACE (1<<0)    /* K */
#define REDIS_NOTIFY_KEYEVENT (1<<1)    /* E */
#define REDIS_NOTIFY_GENERIC (1<<2)     /* g */
#define REDIS_NOTIFY_STRING (1<<3)      /* $ */
#define REDIS_NOTIFY_LIST (1<<4)        /* l */
#define REDIS_NOTIFY_SET (1<<5)         /* s */
#define REDIS_NOTIFY_HASH (1<<6)        /* h */
#define REDIS_NOTIFY_ZSET (1<<7)        /* z */
#define REDIS_NOTIFY_EXPIRED (1<<8)     /* x */
#define REDIS_NOTIFY_EVICTED (1<<9)     /* e */
#define REDIS_NOTIFY_ALL (REDIS_NOTIFY_GENERIC | REDIS_NOTIFY_STRING | REDIS_NOTIFY_LIST | REDIS_NOTIFY_SET | REDIS_NOTIFY_HASH | REDIS_NOTIFY_ZSET | REDIS_NOTIFY_EXPIRED | REDIS_NOTIFY_EVICTED)      /* A */

/* Using the following macro you can run code inside serverCron() with the
 * specified period, specified in milliseconds.
 * The actual resolution depends on server.hz. */
#define run_with_period(_ms_) if ((_ms_ <= 1000/server.hz) || !(server.cronloops%((_ms_)/(1000/server.hz))))

/* We can print the stacktrace, so our assert is defined this way: */
#define redisAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (_redisAssertWithInfo(_c,_o,#_e,__FILE__,__LINE__),_exit(1)))
#define redisAssert(_e) ((_e)?(void)0 : (_redisAssert(#_e,__FILE__,__LINE__),_exit(1)))
#define redisPanic(_e) _redisPanic(#_e,__FILE__,__LINE__),_exit(1)

/*-----------------------------------------------------------------------------
 * Data types
 *----------------------------------------------------------------------------*/

typedef long long mstime_t; /* millisecond time type. */

/* A redis object, that is a type able to hold a string / list / set */

/* The actual Redis Object */
/*
 * Redis 对象
 */
#define REDIS_LRU_BITS 24
#define REDIS_LRU_CLOCK_MAX ((1<<REDIS_LRU_BITS)-1) /* Max value of obj->lru */
#define REDIS_LRU_CLOCK_RESOLUTION 1000 /* LRU clock resolution in ms */
typedef struct redisObject {

    // 类型
    unsigned type:4;

    // 编码
    unsigned encoding:4;

    // 对象最后一次被访问的时间
    unsigned lru:REDIS_LRU_BITS; /* lru time (relative to server.lruclock) */

    // 引用计数
    int refcount;

    // 指向实际值的指针
    void *ptr;

} robj;

/* Macro used to obtain the current LRU clock.
 * If the current resolution is lower than the frequency we refresh the
 * LRU clock (as it should be in production servers) we return the
 * precomputed value, otherwise we need to resort to a function call. */
#define LRU_CLOCK() ((1000/server.hz <= REDIS_LRU_CLOCK_RESOLUTION) ? server.lruclock : getLRUClock())

/* Macro used to initialize a Redis object allocated on the stack.
 * Note that this macro is taken near the structure definition to make sure
 * we'll update it when the structure is changed, to avoid bugs like
 * bug #85 introduced exactly in this way. */
#define initStaticStringObject(_var,_ptr) do { \
    _var.refcount = 1; \
    _var.type = REDIS_STRING; \
    _var.encoding = REDIS_ENCODING_RAW; \
    _var.ptr = _ptr; \
} while(0);

/* To improve the quality of the LRU approximation we take a set of keys
 * that are good candidate for eviction across freeMemoryIfNeeded() calls.
 *
 * Entries inside the eviciton pool are taken ordered by idle time, putting
 * greater idle times to the right (ascending order).
 *
 * Empty entries have the key pointer set to NULL. */
#define REDIS_EVICTION_POOL_SIZE 16
struct evictionPoolEntry {
    unsigned long long idle;    /* Object idle time. */
    sds key;                    /* Key name. */
};

/* Redis database representation. There are multiple databases identified
 * by integers from 0 (the default database) up to the max configured
 * database. The database number is the 'id' field in the structure. */
typedef struct redisDb {

    // 数据库键空间，保存着数据库中的所有键值对
    dict *dict;                 /* The keyspace for this DB */

    // 键的过期时间，字典的键为键，字典的值为过期事件 UNIX 时间戳
    dict *expires;              /* Timeout of keys with a timeout set */

    // 正处于阻塞状态的键
    dict *blocking_keys;        /* Keys with clients waiting for data (BLPOP) */

    // 可以解除阻塞的键
    dict *ready_keys;           /* Blocked keys that received a PUSH */

    // 正在被 WATCH 命令监视的键
    dict *watched_keys;         /* WATCHED keys for MULTI/EXEC CAS */

    struct evictionPoolEntry *eviction_pool;    /* Eviction pool of keys */

    // 数据库号码
    int id;                     /* Database ID */

    // 数据库的键的平均 TTL ，统计信息
    long long avg_ttl;          /* Average TTL, just for stats */

} redisDb;

/* Client MULTI/EXEC state */

/*
 * 事务命令
 */
typedef struct multiCmd {

    // 参数
    robj **argv;

    // 参数数量
    int argc;

    // 命令指针
    struct redisCommand *cmd;

} multiCmd;

/*
 * 事务状态
 */
typedef struct multiState {

    // 事务队列，FIFO 顺序
    multiCmd *commands;     /* Array of MULTI commands */

    // 已入队命令计数
    int count;              /* Total number of MULTI commands */
    int minreplicas;        /* MINREPLICAS for synchronous replication */
    time_t minreplicas_timeout; /* MINREPLICAS timeout as unixtime. */
} multiState;

/* This structure holds the blocking operation state for a client.
 * The fields used depend on client->btype. */
// 阻塞状态
typedef struct blockingState {

    /* Generic fields. */
    // 阻塞时限
    mstime_t timeout;       /* Blocking operation timeout. If UNIX current time
                             * is > timeout then the operation timed out. */

    /* REDIS_BLOCK_LIST */
    // 造成阻塞的键
    dict *keys;             /* The keys we are waiting to terminate a blocking
                             * operation such as BLPOP. Otherwise NULL. */
    // 在被阻塞的键有新元素进入时，需要将这些新元素添加到哪里的目标键
    // 用于 BRPOPLPUSH 命令
    robj *target;           /* The key that should receive the element,
                             * for BRPOPLPUSH. */

    /* REDIS_BLOCK_WAIT */
    // 等待 ACK 的复制节点数量
    int numreplicas;        /* Number of replicas we are waiting for ACK. */
    // 复制偏移量
    long long reploffset;   /* Replication offset to reach. */

} blockingState;

/* The following structure represents a node in the server.ready_keys list,
 * where we accumulate all the keys that had clients blocked with a blocking
 * operation such as B[LR]POP, but received new data in the context of the
 * last executed command.
 *
 * After the execution of every command or script, we run this list to check
 * if as a result we should serve data to clients blocked, unblocking them.
 * Note that server.ready_keys will not have duplicates as there dictionary
 * also called ready_keys in every structure representing a Redis database,
 * where we make sure to remember if a given key was already added in the
 * server.ready_keys list. */
// 记录解除了客户端的阻塞状态的键，以及键所在的数据库。
typedef struct readyList {
    redisDb *db;
    robj *key;
} readyList;

/* With multiplexing we need to take per-client state.
 * Clients are taken in a liked list.
 *
 * 因为 I/O 复用的缘故，需要为每个客户端维持一个状态。
 *
 * 多个客户端状态被服务器用链表连接起来。
 */
typedef struct redisClient {

    // 套接字描述符
    int fd;

    // 当前正在使用的数据库
    redisDb *db;

    // 当前正在使用的数据库的 id （号码）
    int dictid;

    // 客户端的名字
    robj *name;             /* As set by CLIENT SETNAME */

    // 查询缓冲区
    sds querybuf;

    // 查询缓冲区长度峰值
    size_t querybuf_peak;   /* Recent (100ms or more) peak of querybuf size */

    // 参数数量
    int argc;

    // 参数对象数组
    robj **argv;

    // 记录被客户端执行的命令
    struct redisCommand *cmd, *lastcmd;

    // 请求的类型：内联命令还是多条命令
    int reqtype;

    // 剩余未读取的命令内容数量
    int multibulklen;       /* number of multi bulk arguments left to read */

    // 命令内容的长度
    long bulklen;           /* length of bulk argument in multi bulk request */

    // 回复链表
    list *reply;

    // 回复链表中对象的总大小
    unsigned long reply_bytes; /* Tot bytes of objects in reply list */

    // 已发送字节，处理 short write 用
    int sentlen;            /* Amount of bytes already sent in the current
                               buffer or object being sent. */

    // 创建客户端的时间
    time_t ctime;           /* Client creation time */

    // 客户端最后一次和服务器互动的时间
    time_t lastinteraction; /* time of the last interaction, used for timeout */

    // 客户端的输出缓冲区超过软性限制的时间
    time_t obuf_soft_limit_reached_time;

    // 客户端状态标志
    int flags;              /* REDIS_SLAVE | REDIS_MONITOR | REDIS_MULTI ... */

    // 当 server.requirepass 不为 NULL 时
    // 代表认证的状态
    // 0 代表未认证， 1 代表已认证
    int authenticated;      /* when requirepass is non-NULL */

    // 复制状态
    int replstate;          /* replication state if this is a slave */
    // 用于保存主服务器传来的 RDB 文件的文件描述符
    int repldbfd;           /* replication DB file descriptor */

    // 读取主服务器传来的 RDB 文件的偏移量
    off_t repldboff;        /* replication DB file offset */
    // 主服务器传来的 RDB 文件的大小
    off_t repldbsize;       /* replication DB file size */
    
    sds replpreamble;       /* replication DB preamble. */

    // 主服务器的复制偏移量
    long long reploff;      /* replication offset if this is our master */
    // 从服务器最后一次发送 REPLCONF ACK 时的偏移量
    long long repl_ack_off; /* replication ack offset, if this is a slave */
    // 从服务器最后一次发送 REPLCONF ACK 的时间
    long long repl_ack_time;/* replication ack time, if this is a slave */
    // 主服务器的 master run ID
    // 保存在客户端，用于执行部分重同步
    char replrunid[REDIS_RUN_ID_SIZE+1]; /* master run id if this is a master */
    // 从服务器的监听端口号
    int slave_listening_port; /* As configured with: SLAVECONF listening-port */

    // 事务状态
    multiState mstate;      /* MULTI/EXEC state */

    // 阻塞类型
    int btype;              /* Type of blocking op if REDIS_BLOCKED. */
    // 阻塞状态
    blockingState bpop;     /* blocking state */

    // 最后被写入的全局复制偏移量
    long long woff;         /* Last write global replication offset. */

    // 被监视的键
    list *watched_keys;     /* Keys WATCHED for MULTI/EXEC CAS */

    // 这个字典记录了客户端所有订阅的频道
    // 键为频道名字，值为 NULL
    // 也即是，一个频道的集合
    dict *pubsub_channels;  /* channels a client is interested in (SUBSCRIBE) */

    // 链表，包含多个 pubsubPattern 结构
    // 记录了所有订阅频道的客户端的信息
    // 新 pubsubPattern 结构总是被添加到表尾
    list *pubsub_patterns;  /* patterns a client is interested in (SUBSCRIBE) */
    sds peerid;             /* Cached peer ID. */

    /* Response buffer */
    // 回复偏移量
    int bufpos;
    // 回复缓冲区
    char buf[REDIS_REPLY_CHUNK_BYTES];

} redisClient;

// 服务器的保存条件（BGSAVE 自动执行的条件）
struct saveparam {

    // 多少秒之内
    time_t seconds;

    // 发生多少次修改
    int changes;

};

// 通过复用来减少内存碎片，以及减少操作耗时的共享对象
struct sharedObjectsStruct {
    robj *crlf, *ok, *err, *emptybulk, *czero, *cone, *cnegone, *pong, *space,
    *colon, *nullbulk, *nullmultibulk, *queued,
    *emptymultibulk, *wrongtypeerr, *nokeyerr, *syntaxerr, *sameobjecterr,
    *outofrangeerr, *noscripterr, *loadingerr, *slowscripterr, *bgsaveerr,
    *masterdownerr, *roslaveerr, *execaborterr, *noautherr, *noreplicaserr,
    *busykeyerr, *oomerr, *plus, *messagebulk, *pmessagebulk, *subscribebulk,
    *unsubscribebulk, *psubscribebulk, *punsubscribebulk, *del, *rpop, *lpop,
    *lpush, *emptyscan, *minstring, *maxstring,
    *select[REDIS_SHARED_SELECT_CMDS],
    *integers[REDIS_SHARED_INTEGERS],
    *mbulkhdr[REDIS_SHARED_BULKHDR_LEN], /* "*<value>\r\n" */
    *bulkhdr[REDIS_SHARED_BULKHDR_LEN];  /* "$<value>\r\n" */
};

/* ZSETs use a specialized version of Skiplists */
/*
 * 跳跃表节点
 */
typedef struct zskiplistNode {

    // 成员对象
    robj *obj;

    // 分值
    double score;

    // 后退指针
    struct zskiplistNode *backward;

    // 层
    struct zskiplistLevel {

        // 前进指针
        struct zskiplistNode *forward;

        // 跨度
        unsigned int span;

    } level[];

} zskiplistNode;

/*
 * 跳跃表
 */
typedef struct zskiplist {

    // 表头节点和表尾节点
    struct zskiplistNode *header, *tail;

    // 表中节点的数量
    unsigned long length;

    // 表中层数最大的节点的层数
    int level;

} zskiplist;

/*
 * 有序集合
 */
typedef struct zset {

    // 字典，键为成员，值为分值
    // 用于支持 O(1) 复杂度的按成员取分值操作
    dict *dict;

    // 跳跃表，按分值排序成员
    // 用于支持平均复杂度为 O(log N) 的按分值定位成员操作
    // 以及范围操作
    zskiplist *zsl;

} zset;

// 客户端缓冲区限制
typedef struct clientBufferLimitsConfig {
    // 硬限制
    unsigned long long hard_limit_bytes;
    // 软限制
    unsigned long long soft_limit_bytes;
    // 软限制时限
    time_t soft_limit_seconds;
} clientBufferLimitsConfig;

// 限制可以有多个
extern clientBufferLimitsConfig clientBufferLimitsDefaults[REDIS_CLIENT_LIMIT_NUM_CLASSES];

/* The redisOp structure defines a Redis Operation, that is an instance of
 * a command with an argument vector, database ID, propagation target
 * (REDIS_PROPAGATE_*), and command pointer.
 *
 * redisOp 结构定义了一个 Redis 操作，
 * 它包含指向被执行命令的指针、命令的参数、数据库 ID 、传播目标（REDIS_PROPAGATE_*）。
 *
 * Currently only used to additionally propagate more commands to AOF/Replication
 * after the propagation of the executed command. 
 *
 * 目前只用于在传播被执行命令之后，传播附加的其他命令到 AOF 或 Replication 中。
 */
typedef struct redisOp {

    // 参数
    robj **argv;

    // 参数数量、数据库 ID 、传播目标
    int argc, dbid, target;

    // 被执行命令的指针
    struct redisCommand *cmd;

} redisOp;

/* Defines an array of Redis operations. There is an API to add to this
 * structure in a easy way.
 *
 * redisOpArrayInit();
 * redisOpArrayAppend();
 * redisOpArrayFree();
 */
typedef struct redisOpArray {
    redisOp *ops;
    int numops;
} redisOpArray;

/*-----------------------------------------------------------------------------
 * Global server state
 *----------------------------------------------------------------------------*/

struct clusterState;

struct redisServer {

    /* General */

    // 配置文件的绝对路径
    char *configfile;           /* Absolute config file path, or NULL */

    // serverCron() 每秒调用的次数
    int hz;                     /* serverCron() calls frequency in hertz */

    // 数据库
    redisDb *db;

    // 命令表（受到 rename 配置选项的作用）
    dict *commands;             /* Command table */
    // 命令表（无 rename 配置选项的作用）
    dict *orig_commands;        /* Command table before command renaming. */

    // 事件状态
    aeEventLoop *el;

    // 最近一次使用时钟
    unsigned lruclock:REDIS_LRU_BITS; /* Clock for LRU eviction */

    // 关闭服务器的标识
    int shutdown_asap;          /* SHUTDOWN needed ASAP */

    // 在执行 serverCron() 时进行渐进式 rehash
    int activerehashing;        /* Incremental rehash in serverCron() */

    // 是否设置了密码
    char *requirepass;          /* Pass for AUTH command, or NULL */

    // PID 文件
    char *pidfile;              /* PID file path */

    // 架构类型
    int arch_bits;              /* 32 or 64 depending on sizeof(long) */

    // serverCron() 函数的运行次数计数器
    int cronloops;              /* Number of times the cron function run */

    // 本服务器的 RUN ID
    char runid[REDIS_RUN_ID_SIZE+1];  /* ID always different at every exec. */

    // 服务器是否运行在 SENTINEL 模式
    int sentinel_mode;          /* True if this instance is a Sentinel. */


    /* Networking */

    // TCP 监听端口
    int port;                   /* TCP listening port */

    int tcp_backlog;            /* TCP listen() backlog */

    // 地址
    char *bindaddr[REDIS_BINDADDR_MAX]; /* Addresses we should bind to */
    // 地址数量
    int bindaddr_count;         /* Number of addresses in server.bindaddr[] */

    // UNIX 套接字
    char *unixsocket;           /* UNIX socket path */
    mode_t unixsocketperm;      /* UNIX socket permission */

    // 描述符
    int ipfd[REDIS_BINDADDR_MAX]; /* TCP socket file descriptors */
    // 描述符数量
    int ipfd_count;             /* Used slots in ipfd[] */

    // UNIX 套接字文件描述符
    int sofd;                   /* Unix socket file descriptor */

    int cfd[REDIS_BINDADDR_MAX];/* Cluster bus listening socket */
    int cfd_count;              /* Used slots in cfd[] */

    // 一个链表，保存了所有客户端状态结构
    list *clients;              /* List of active clients */
    // 链表，保存了所有待关闭的客户端
    list *clients_to_close;     /* Clients to close asynchronously */

    // 链表，保存了所有从服务器，以及所有监视器
    list *slaves, *monitors;    /* List of slaves and MONITORs */

    // 服务器的当前客户端，仅用于崩溃报告
    redisClient *current_client; /* Current client, only used on crash report */

    int clients_paused;         /* True if clients are currently paused */
    mstime_t clients_pause_end_time; /* Time when we undo clients_paused */

    // 网络错误
    char neterr[ANET_ERR_LEN];   /* Error buffer for anet.c */

    // MIGRATE 缓存
    dict *migrate_cached_sockets;/* MIGRATE cached sockets */


    /* RDB / AOF loading information */

    // 这个值为真时，表示服务器正在进行载入
    int loading;                /* We are loading data from disk if true */

    // 正在载入的数据的大小
    off_t loading_total_bytes;

    // 已载入数据的大小
    off_t loading_loaded_bytes;

    // 开始进行载入的时间
    time_t loading_start_time;
    off_t loading_process_events_interval_bytes;

    /* Fast pointers to often looked up command */
    // 常用命令的快捷连接
    struct redisCommand *delCommand, *multiCommand, *lpushCommand, *lpopCommand,
                        *rpopCommand;


    /* Fields used only for stats */

    // 服务器启动时间
    time_t stat_starttime;          /* Server start time */

    // 已处理命令的数量
    long long stat_numcommands;     /* Number of processed commands */

    // 服务器接到的连接请求数量
    long long stat_numconnections;  /* Number of connections received */

    // 已过期的键数量
    long long stat_expiredkeys;     /* Number of expired keys */

    // 因为回收内存而被释放的过期键的数量
    long long stat_evictedkeys;     /* Number of evicted keys (maxmemory) */

    // 成功查找键的次数
    long long stat_keyspace_hits;   /* Number of successful lookups of keys */

    // 查找键失败的次数
    long long stat_keyspace_misses; /* Number of failed lookups of keys */

    // 已使用内存峰值
    size_t stat_peak_memory;        /* Max used memory record */

    // 最后一次执行 fork() 时消耗的时间
    long long stat_fork_time;       /* Time needed to perform latest fork() */

    // 服务器因为客户端数量过多而拒绝客户端连接的次数
    long long stat_rejected_conn;   /* Clients rejected because of maxclients */

    // 执行 full sync 的次数
    long long stat_sync_full;       /* Number of full resyncs with slaves. */

    // PSYNC 成功执行的次数
    long long stat_sync_partial_ok; /* Number of accepted PSYNC requests. */

    // PSYNC 执行失败的次数
    long long stat_sync_partial_err;/* Number of unaccepted PSYNC requests. */


    /* slowlog */

    // 保存了所有慢查询日志的链表
    list *slowlog;                  /* SLOWLOG list of commands */

    // 下一条慢查询日志的 ID
    long long slowlog_entry_id;     /* SLOWLOG current entry ID */

    // 服务器配置 slowlog-log-slower-than 选项的值
    long long slowlog_log_slower_than; /* SLOWLOG time limit (to get logged) */

    // 服务器配置 slowlog-max-len 选项的值
    unsigned long slowlog_max_len;     /* SLOWLOG max number of items logged */
    size_t resident_set_size;       /* RSS sampled in serverCron(). */
    /* The following two are used to track instantaneous "load" in terms
     * of operations per second. */
    // 最后一次进行抽样的时间
    long long ops_sec_last_sample_time; /* Timestamp of last sample (in ms) */
    // 最后一次抽样时，服务器已执行命令的数量
    long long ops_sec_last_sample_ops;  /* numcommands in last sample */
    // 抽样结果
    long long ops_sec_samples[REDIS_OPS_SEC_SAMPLES];
    // 数组索引，用于保存抽样结果，并在需要时回绕到 0
    int ops_sec_idx;


    /* Configuration */

    // 日志可见性
    int verbosity;                  /* Loglevel in redis.conf */

    // 客户端最大空转时间
    int maxidletime;                /* Client timeout in seconds */

    // 是否开启 SO_KEEPALIVE 选项
    int tcpkeepalive;               /* Set SO_KEEPALIVE if non-zero. */
    int active_expire_enabled;      /* Can be disabled for testing purposes. */
    size_t client_max_querybuf_len; /* Limit for client query buffer length */
    int dbnum;                      /* Total number of configured DBs */
    int daemonize;                  /* True if running as a daemon */
    // 客户端输出缓冲区大小限制
    // 数组的元素有 REDIS_CLIENT_LIMIT_NUM_CLASSES 个
    // 每个代表一类客户端：普通、从服务器、pubsub，诸如此类
    clientBufferLimitsConfig client_obuf_limits[REDIS_CLIENT_LIMIT_NUM_CLASSES];


    /* AOF persistence */

    // AOF 状态（开启/关闭/可写）
    int aof_state;                  /* REDIS_AOF_(ON|OFF|WAIT_REWRITE) */

    // 所使用的 fsync 策略（每个写入/每秒/从不）
    int aof_fsync;                  /* Kind of fsync() policy */
    char *aof_filename;             /* Name of the AOF file */
    int aof_no_fsync_on_rewrite;    /* Don't fsync if a rewrite is in prog. */
    int aof_rewrite_perc;           /* Rewrite AOF if % growth is > M and... */
    off_t aof_rewrite_min_size;     /* the AOF file is at least N bytes. */

    // 最后一次执行 BGREWRITEAOF 时， AOF 文件的大小
    off_t aof_rewrite_base_size;    /* AOF size on latest startup or rewrite. */

    // AOF 文件的当前字节大小
    off_t aof_current_size;         /* AOF current size. */
    int aof_rewrite_scheduled;      /* Rewrite once BGSAVE terminates. */

    // 负责进行 AOF 重写的子进程 ID
    pid_t aof_child_pid;            /* PID if rewriting process */

    // AOF 重写缓存链表，链接着多个缓存块
    list *aof_rewrite_buf_blocks;   /* Hold changes during an AOF rewrite. */

    // AOF 缓冲区
    sds aof_buf;      /* AOF buffer, written before entering the event loop */

    // AOF 文件的描述符
    int aof_fd;       /* File descriptor of currently selected AOF file */

    // AOF 的当前目标数据库
    int aof_selected_db; /* Currently selected DB in AOF */

    // 推迟 write 操作的时间
    time_t aof_flush_postponed_start; /* UNIX time of postponed AOF flush */

    // 最后一直执行 fsync 的时间
    time_t aof_last_fsync;            /* UNIX time of last fsync() */
    time_t aof_rewrite_time_last;   /* Time used by last AOF rewrite run. */

    // AOF 重写的开始时间
    time_t aof_rewrite_time_start;  /* Current AOF rewrite start time. */

    // 最后一次执行 BGREWRITEAOF 的结果
    int aof_lastbgrewrite_status;   /* REDIS_OK or REDIS_ERR */

    // 记录 AOF 的 write 操作被推迟了多少次
    unsigned long aof_delayed_fsync;  /* delayed AOF fsync() counter */

    // 指示是否需要每写入一定量的数据，就主动执行一次 fsync()
    int aof_rewrite_incremental_fsync;/* fsync incrementally while rewriting? */
    int aof_last_write_status;      /* REDIS_OK or REDIS_ERR */
    int aof_last_write_errno;       /* Valid if aof_last_write_status is ERR */
    /* RDB persistence */

    // 自从上次 SAVE 执行以来，数据库被修改的次数
    long long dirty;                /* Changes to DB from the last save */

    // BGSAVE 执行前的数据库被修改次数
    long long dirty_before_bgsave;  /* Used to restore dirty on failed BGSAVE */

    // 负责执行 BGSAVE 的子进程的 ID
    // 没在执行 BGSAVE 时，设为 -1
    pid_t rdb_child_pid;            /* PID of RDB saving child */
    struct saveparam *saveparams;   /* Save points array for RDB */
    int saveparamslen;              /* Number of saving points */
    char *rdb_filename;             /* Name of RDB file */
    int rdb_compression;            /* Use compression in RDB? */
    int rdb_checksum;               /* Use RDB checksum? */

    // 最后一次完成 SAVE 的时间
    time_t lastsave;                /* Unix time of last successful save */

    // 最后一次尝试执行 BGSAVE 的时间
    time_t lastbgsave_try;          /* Unix time of last attempted bgsave */

    // 最近一次 BGSAVE 执行耗费的时间
    time_t rdb_save_time_last;      /* Time used by last RDB save run. */

    // 数据库最近一次开始执行 BGSAVE 的时间
    time_t rdb_save_time_start;     /* Current RDB save start time. */

    // 最后一次执行 SAVE 的状态
    int lastbgsave_status;          /* REDIS_OK or REDIS_ERR */
    int stop_writes_on_bgsave_err;  /* Don't allow writes if can't BGSAVE */


    /* Propagation of commands in AOF / replication */
    redisOpArray also_propagate;    /* Additional command to propagate. */


    /* Logging */
    char *logfile;                  /* Path of log file */
    int syslog_enabled;             /* Is syslog enabled? */
    char *syslog_ident;             /* Syslog ident */
    int syslog_facility;            /* Syslog facility */


    /* Replication (master) */
    int slaveseldb;                 /* Last SELECTed DB in replication output */
    // 全局复制偏移量（一个累计值）
    long long master_repl_offset;   /* Global replication offset */
    // 主服务器发送 PING 的频率
    int repl_ping_slave_period;     /* Master pings the slave every N seconds */

    // backlog 本身
    char *repl_backlog;             /* Replication backlog for partial syncs */
    // backlog 的长度
    long long repl_backlog_size;    /* Backlog circular buffer size */
    // backlog 中数据的长度
    long long repl_backlog_histlen; /* Backlog actual data length */
    // backlog 的当前索引
    long long repl_backlog_idx;     /* Backlog circular buffer current offset */
    // backlog 中可以被还原的第一个字节的偏移量
    long long repl_backlog_off;     /* Replication offset of first byte in the
                                       backlog buffer. */
    // backlog 的过期时间
    time_t repl_backlog_time_limit; /* Time without slaves after the backlog
                                       gets released. */

    // 距离上一次有从服务器的时间
    time_t repl_no_slaves_since;    /* We have no slaves since that time.
                                       Only valid if server.slaves len is 0. */

    // 是否开启最小数量从服务器写入功能
    int repl_min_slaves_to_write;   /* Min number of slaves to write. */
    // 定义最小数量从服务器的最大延迟值
    int repl_min_slaves_max_lag;    /* Max lag of <count> slaves to write. */
    // 延迟良好的从服务器的数量
    int repl_good_slaves_count;     /* Number of slaves with lag <= max_lag. */


    /* Replication (slave) */
    // 主服务器的验证密码
    char *masterauth;               /* AUTH with this password with master */
    // 主服务器的地址
    char *masterhost;               /* Hostname of master */
    // 主服务器的端口
    int masterport;                 /* Port of master */
    // 超时时间
    int repl_timeout;               /* Timeout after N seconds of master idle */
    // 主服务器所对应的客户端
    redisClient *master;     /* Client that is master for this slave */
    // 被缓存的主服务器，PSYNC 时使用
    redisClient *cached_master; /* Cached master to be reused for PSYNC. */
    int repl_syncio_timeout; /* Timeout for synchronous I/O calls */
    // 复制的状态（服务器是从服务器时使用）
    int repl_state;          /* Replication status if the instance is a slave */
    // RDB 文件的大小
    off_t repl_transfer_size; /* Size of RDB to read from master during sync. */
    // 已读 RDB 文件内容的字节数
    off_t repl_transfer_read; /* Amount of RDB read from master during sync. */
    // 最近一次执行 fsync 时的偏移量
    // 用于 sync_file_range 函数
    off_t repl_transfer_last_fsync_off; /* Offset when we fsync-ed last time. */
    // 主服务器的套接字
    int repl_transfer_s;     /* Slave -> Master SYNC socket */
    // 保存 RDB 文件的临时文件的描述符
    int repl_transfer_fd;    /* Slave -> Master SYNC temp file descriptor */
    // 保存 RDB 文件的临时文件名字
    char *repl_transfer_tmpfile; /* Slave-> master SYNC temp file name */
    // 最近一次读入 RDB 内容的时间
    time_t repl_transfer_lastio; /* Unix time of the latest read, for timeout */
    int repl_serve_stale_data; /* Serve stale data when link is down? */
    // 是否只读从服务器？
    int repl_slave_ro;          /* Slave is read only? */
    // 连接断开的时长
    time_t repl_down_since; /* Unix time at which link with master went down */
    // 是否要在 SYNC 之后关闭 NODELAY ？
    int repl_disable_tcp_nodelay;   /* Disable TCP_NODELAY after SYNC? */
    // 从服务器优先级
    int slave_priority;             /* Reported in INFO and used by Sentinel. */
    // 本服务器（从服务器）当前主服务器的 RUN ID
    char repl_master_runid[REDIS_RUN_ID_SIZE+1];  /* Master run id for PSYNC. */
    // 初始化偏移量
    long long repl_master_initial_offset;         /* Master PSYNC offset. */


    /* Replication script cache. */
    // 复制脚本缓存
    // 字典
    dict *repl_scriptcache_dict;        /* SHA1 all slaves are aware of. */
    // FIFO 队列
    list *repl_scriptcache_fifo;        /* First in, first out LRU eviction. */
    // 缓存的大小
    int repl_scriptcache_size;          /* Max number of elements. */

    /* Synchronous replication. */
    list *clients_waiting_acks;         /* Clients waiting in WAIT command. */
    int get_ack_from_slaves;            /* If true we send REPLCONF GETACK. */
    /* Limits */
    int maxclients;                 /* Max number of simultaneous clients */
    unsigned long long maxmemory;   /* Max number of memory bytes to use */
    int maxmemory_policy;           /* Policy for key eviction */
    int maxmemory_samples;          /* Pricision of random sampling */


    /* Blocked clients */
    unsigned int bpop_blocked_clients; /* Number of clients blocked by lists */
    list *unblocked_clients; /* list of clients to unblock before next loop */
    list *ready_keys;        /* List of readyList structures for BLPOP & co */


    /* Sort parameters - qsort_r() is only available under BSD so we
     * have to take this state global, in order to pass it to sortCompare() */
    int sort_desc;
    int sort_alpha;
    int sort_bypattern;
    int sort_store;


    /* Zip structure config, see redis.conf for more information  */
    size_t hash_max_ziplist_entries;
    size_t hash_max_ziplist_value;
    size_t list_max_ziplist_entries;
    size_t list_max_ziplist_value;
    size_t set_max_intset_entries;
    size_t zset_max_ziplist_entries;
    size_t zset_max_ziplist_value;
    size_t hll_sparse_max_bytes;
    time_t unixtime;        /* Unix time sampled every cron cycle. */
    long long mstime;       /* Like 'unixtime' but with milliseconds resolution. */


    /* Pubsub */
    // 字典，键为频道，值为链表
    // 链表中保存了所有订阅某个频道的客户端
    // 新客户端总是被添加到链表的表尾
    dict *pubsub_channels;  /* Map channels to list of subscribed clients */

    // 这个链表记录了客户端订阅的所有模式的名字
    list *pubsub_patterns;  /* A list of pubsub_patterns */

    int notify_keyspace_events; /* Events to propagate via Pub/Sub. This is an
                                   xor of REDIS_NOTIFY... flags. */


    /* Cluster */

    int cluster_enabled;      /* Is cluster enabled? */
    mstime_t cluster_node_timeout; /* Cluster node timeout. */
    char *cluster_configfile; /* Cluster auto-generated config file name. */
    struct clusterState *cluster;  /* State of the cluster */

    int cluster_migration_barrier; /* Cluster replicas migration barrier. */
    /* Scripting */

    // Lua 环境
    lua_State *lua; /* The Lua interpreter. We use just one for all clients */
    
    // 复制执行 Lua 脚本中的 Redis 命令的伪客户端
    redisClient *lua_client;   /* The "fake client" to query Redis from Lua */

    // 当前正在执行 EVAL 命令的客户端，如果没有就是 NULL
    redisClient *lua_caller;   /* The client running EVAL right now, or NULL */

    // 一个字典，值为 Lua 脚本，键为脚本的 SHA1 校验和
    dict *lua_scripts;         /* A dictionary of SHA1 -> Lua scripts */
    // Lua 脚本的执行时限
    mstime_t lua_time_limit;  /* Script timeout in milliseconds */
    // 脚本开始执行的时间
    mstime_t lua_time_start;  /* Start time of script, milliseconds time */

    // 脚本是否执行过写命令
    int lua_write_dirty;  /* True if a write command was called during the
                             execution of the current script. */

    // 脚本是否执行过带有随机性质的命令
    int lua_random_dirty; /* True if a random command was called during the
                             execution of the current script. */

    // 脚本是否超时
    int lua_timedout;     /* True if we reached the time limit for script
                             execution. */

    // 是否要杀死脚本
    int lua_kill;         /* Kill the script if true. */


    /* Assert & bug reporting */

    char *assert_failed;
    char *assert_file;
    int assert_line;
    int bug_report_start; /* True if bug report header was already logged. */
    int watchdog_period;  /* Software watchdog period in ms. 0 = off */
};

/*
 * 记录订阅模式的结构
 */
typedef struct pubsubPattern {

    // 订阅模式的客户端
    redisClient *client;

    // 被订阅的模式
    robj *pattern;

} pubsubPattern;

typedef void redisCommandProc(redisClient *c);
typedef int *redisGetKeysProc(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);

/*
 * Redis 命令
 */
struct redisCommand {

    // 命令名字
    char *name;

    // 实现函数
    redisCommandProc *proc;

    // 参数个数
    int arity;

    // 字符串表示的 FLAG
    char *sflags; /* Flags as string representation, one char per flag. */

    // 实际 FLAG
    int flags;    /* The actual flags, obtained from the 'sflags' field. */

    /* Use a function to determine keys arguments in a command line.
     * Used for Redis Cluster redirect. */
    // 从命令中判断命令的键参数。在 Redis 集群转向时使用。
    redisGetKeysProc *getkeys_proc;

    /* What keys should be loaded in background when calling this command? */
    // 指定哪些参数是 key
    int firstkey; /* The first argument that's a key (0 = no keys) */
    int lastkey;  /* The last argument that's a key */
    int keystep;  /* The step between first and last key */

    // 统计信息
    // microseconds 记录了命令执行耗费的总毫微秒数
    // calls 是命令被执行的总次数
    long long microseconds, calls;
};

struct redisFunctionSym {
    char *name;
    unsigned long pointer;
};

// 用于保存被排序值及其权重的结构
typedef struct _redisSortObject {

    // 被排序键的值
    robj *obj;

    // 权重
    union {

        // 排序数字值时使用
        double score;

        // 排序字符串时使用
        robj *cmpobj;

    } u;

} redisSortObject;

// 排序操作
typedef struct _redisSortOperation {

    // 操作的类型，可以是 GET 、 DEL 、INCR 或者 DECR
    // 目前只实现了 GET
    int type;

    // 用户给定的模式
    robj *pattern;

} redisSortOperation;

/* Structure to hold list iteration abstraction.
 *
 * 列表迭代器对象
 */
typedef struct {

    // 列表对象
    robj *subject;

    // 对象所使用的编码
    unsigned char encoding;

    // 迭代的方向
    unsigned char direction; /* Iteration direction */

    // ziplist 索引，迭代 ziplist 编码的列表时使用
    unsigned char *zi;

    // 链表节点的指针，迭代双端链表编码的列表时使用
    listNode *ln;

} listTypeIterator;

/* Structure for an entry while iterating over a list.
 *
 * 迭代列表时使用的记录结构，
 * 用于保存迭代器，以及迭代器返回的列表节点。
 */
typedef struct {

    // 列表迭代器
    listTypeIterator *li;

    // ziplist 节点索引
    unsigned char *zi;  /* Entry in ziplist */

    // 双端链表节点指针
    listNode *ln;       /* Entry in linked list */

} listTypeEntry;

/* Structure to hold set iteration abstraction. */
/*
 * 多态集合迭代器
 */
typedef struct {

    // 被迭代的对象
    robj *subject;

    // 对象的编码
    int encoding;

    // 索引值，编码为 intset 时使用
    int ii; /* intset iterator */

    // 字典迭代器，编码为 HT 时使用
    dictIterator *di;

} setTypeIterator;

/* Structure to hold hash iteration abstraction. Note that iteration over
 * hashes involves both fields and values. Because it is possible that
 * not both are required, store pointers in the iterator to avoid
 * unnecessary memory allocation for fields/values. */
/*
 * 哈希对象的迭代器
 */
typedef struct {

    // 被迭代的哈希对象
    robj *subject;

    // 哈希对象的编码
    int encoding;

    // 域指针和值指针
    // 在迭代 ZIPLIST 编码的哈希对象时使用
    unsigned char *fptr, *vptr;

    // 字典迭代器和指向当前迭代字典节点的指针
    // 在迭代 HT 编码的哈希对象时使用
    dictIterator *di;
    dictEntry *de;
} hashTypeIterator;

#define REDIS_HASH_KEY 1
#define REDIS_HASH_VALUE 2

/*-----------------------------------------------------------------------------
 * Extern declarations
 *----------------------------------------------------------------------------*/

extern struct redisServer server;
extern struct sharedObjectsStruct shared;
extern dictType setDictType;
extern dictType zsetDictType;
extern dictType clusterNodesDictType;
extern dictType clusterNodesBlackListDictType;
extern dictType dbDictType;
extern dictType shaScriptObjectDictType;
extern double R_Zero, R_PosInf, R_NegInf, R_Nan;
extern dictType hashDictType;
extern dictType replScriptCacheDictType;

/*-----------------------------------------------------------------------------
 * Functions prototypes
 *----------------------------------------------------------------------------*/

/* Utils */
long long ustime(void);//返回微秒格式的 UNIX 时间,1 秒 = 1 000 000 微秒
long long mstime(void);//返回毫秒格式的 UNIX 时间,1 秒 = 1 000 毫秒
void getRandomHexChars(char *p, unsigned int len);//设置服务器的运行 ID
uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l);//64位CRC校验码的算法
void exitFromChild(int retcode);//用_exit()或exit()退出，获得正确的覆盖率信息。
size_t redisPopcount(void *s, long count);//计算长度为 count 的二进制数组指针 s 被设置为 1 的位的数量。
void redisSetProcTitle(char *title);//为服务器进程设置名字

/* networking.c -- Networking and Client related operations */
redisClient *createClient(int fd);//为主服务器绑定事件
void closeTimedoutClients(void);//???????????????
void freeClient(redisClient *c);//关闭超时客户端
void freeClientAsync(redisClient *c);//异步地释放给定的客户端
void resetClient(redisClient *c);//在客户端执行完命令之后执行：重置客户端以准备执行下个命令
void sendReplyToClient(aeEventLoop *el, int fd, void *privdata, int mask);//负责传送命令回复的写处理器
void addReply(redisClient *c, robj *obj);//在redisClient的buffer中写入数据，数据存在obj->ptr的指针中
void *addDeferredMultiBulkLength(redisClient *c);//当发送 Multi Bulk 回复时，先创建一个空的链表，之后再用实际的回复填充它
void setDeferredMultiBulkLength(redisClient *c, void *node, long length);//设置 Multi Bulk 回复的长度
void addReplySds(redisClient *c, sds s);//将 SDS 中的内容复制到回复缓冲区
void processInputBuffer(redisClient *c);//处理客户端输入的命令内容
void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask);//创建一个 TCP 连接处理器
void acceptUnixHandler(aeEventLoop *el, int fd, void *privdata, int mask);//创建一个本地连接处理器
void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask);//读取客户端的查询缓冲区内容
void addReplyBulk(redisClient *c, robj *obj);//返回一个 Redis 对象作为回复
void addReplyBulkCString(redisClient *c, char *s);//返回一个 C 字符串作为回复
void addReplyBulkCBuffer(redisClient *c, void *p, size_t len);//返回一个 C 缓冲区作为回复
void addReplyBulkLongLong(redisClient *c, long long ll);//返回一个 long long 值作为回复
void acceptHandler(aeEventLoop *el, int fd, void *privdata, int mask);//??????????????
void addReply(redisClient *c, robj *obj);//在redisClient的buffer中写入数据，数据存在obj->ptr的指针中//??????与以上重复？？？
void addReplySds(redisClient *c, sds s);//在回复中添加Sds字符串,下面的额addReply()系列方法原理基本类似
void addReplyError(redisClient *c, char *err);//往Reply中添加error类的信息
void addReplyStatus(redisClient *c, char *status);//创建一次 SORT 操作
void addReplyDouble(redisClient *c, double d);//在bulk reply中添加一个double类型值，bulk的意思为大块的，bulk reply的意思为大数据量的回复
void addReplyLongLong(redisClient *c, long long ll);//返回一个整数回复,格式为 :10086\r\n
void addReplyMultiBulkLen(redisClient *c, long length);//???
void copyClientOutputBuffer(redisClient *dst, redisClient *src);//将源Client的输出buffer复制给目标Client
void *dupClientReplyValue(void *o);//复制value一份
void getClientsMaxBuffers(unsigned long *longest_output_list,
                          unsigned long *biggest_input_buffer);//获取Client中输入buffer和输出buffer的最大长度值
void formatPeerId(char *peerid, size_t peerid_len, char *ip, int port);//格式化ip,port端口号的输出，ip:port 
char *getClientPeerId(redisClient *client);//获取c->peerid客户端的地址信息
sds catClientInfoString(sds s, redisClient *client);//格式化的输出客户端的属性信息，直接返回一个拼接好的字符串
sds getAllClientsInfoString(void);//获取所有Client客户端的属性信息，并连接成一个总的字符串并输出
void rewriteClientCommandVector(redisClient *c, int argc, ...);//重写客户端的命令集合，旧的命令集合的应用计数减1，新的Command  Vector的命令集合增1 
void rewriteClientCommandArgument(redisClient *c, int i, robj *newval);//重写Client中的第i个参数
unsigned long getClientOutputBufferMemoryUsage(redisClient *c);//获取Client中已经用去的输出buffer的大小
void freeClientsInAsyncFreeQueue(void);//异步的free客户端
void asyncCloseClientOnOutputBufferLimitReached(redisClient *c);//异步的关闭Client，如果缓冲区中的软限制或是硬限制已经到达的时候，缓冲区超出限制的结果会导致释放不安全，
int getClientLimitClassByName(char *name);//根据名字，获取客户端的类型常量
char *getClientLimitClassName(int class);//根据客户端的类型，获取名字
void flushSlavesOutputBuffers(void);//函数的辅助函数，用于在不进入事件循环的情况下，冲洗所有从服务器的输出缓冲区。
void disconnectSlaves(void);//断开所有从服务器的连接，强制所有从服务器执行重同步
int listenToPort(int port, int *fds, int *count);//绑定监听端口
void pauseClients(mstime_t duration);//暂停客户端，让服务器在指定的时间内不再接受被暂停客户端发来的命令可以用于系统更新，并在内部由 CLUSTER FAILOVER 命令使用。
int clientsArePaused(void);//判断服务器目前被暂停客户端的数量，没有任何客户端被暂停时，返回 0 。
int processEventsWhileBlocked(void);//让服务器在被阻塞的情况下，仍然处理某些事件。

#ifdef __GNUC__
void addReplyErrorFormat(redisClient *c, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));//????
void addReplyStatusFormat(redisClient *c, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));//????
#else
void addReplyErrorFormat(redisClient *c, const char *fmt, ...);
void addReplyStatusFormat(redisClient *c, const char *fmt, ...);
#endif

/* List data type */
void listTypeTryConversion(robj *subject, robj *value);//对输入值 value 进行检查，看是否需要将 subject 从 ziplist 转换为双端链表，以便保存值 value 。
void listTypePush(robj *subject, robj *value, int where);//调用者无须担心 value 的引用计数，因为这个函数会负责这方面的工作。
robj *listTypePop(robj *subject, int where);//从列表的表头或表尾中弹出一个元素。
unsigned long listTypeLength(robj *subject);//返回列表的节点数量
listTypeIterator *listTypeInitIterator(robj *subject, long index, unsigned char direction);//创建并返回一个列表迭代器。
void listTypeReleaseIterator(listTypeIterator *li);//释放迭代器
int listTypeNext(listTypeIterator *li, listTypeEntry *entry);//使用 entry 结构记录迭代器当前指向的节点，并将迭代器的指针移动到下一个元素。
robj *listTypeGet(listTypeEntry *entry);//返回 entry 结构当前所保存的列表节点。
void listTypeInsert(listTypeEntry *entry, robj *value, int where);//将对象 value 插入到列表节点的之前或之后。
int listTypeEqual(listTypeEntry *entry, robj *o);//将当前节点的值和对象 o 进行对比,函数在两值相等时返回 1 ，不相等时返回 0 。
void listTypeDelete(listTypeEntry *entry);//删除 entry 所指向的节点
void listTypeConvert(robj *subject, int enc);//将列表的底层编码从 ziplist 转换成双端链表
void unblockClientWaitingData(redisClient *c);//打开一个在阻塞操作(如BLPOP)中等待的客户端。
void handleClientsBlockedOnLists(void);//这个函数会遍历整个 serve.ready_keys 链表，并将里面的 key 的元素弹出给被阻塞客户端，从而解除客户端的阻塞状态。
void popGenericCommand(redisClient *c, int where);//????

/* MULTI/EXEC/WATCH... */
void unwatchAllKeys(redisClient *c);//取消客户端对所有键的监视。
void initClientMultiState(redisClient *c);//初始化客户端的事务状态
void freeClientMultiState(redisClient *c);//释放所有事务状态相关的资源
void queueMultiCommand(redisClient *c);//将一个新命令添加到事务队列中
void touchWatchedKey(redisDb *db, robj *key);//“触碰”一个键，如果这个键正在被某个/某些客户端监视着，那么这个/这些客户端在执行 EXEC 时事务将失败。
void touchWatchedKeysOnFlush(int dbid);//根据key所在的的db，把此db下的watched-key统统touch一遍
void discardTransaction(redisClient *c);//撤销事务操作
void flagTransaction(redisClient *c);//标记一个事物为DIRTY_EXEC状态，最后这个事物会执行失败，，此方法调用于插入命令的时候

/* Redis object implementation */
void decrRefCount(robj *o);//为对象的引用计数减一,当对象的引用计数降为 0 时，释放对象。
void decrRefCountVoid(void *o);//作用于特定数据结构的释放函数包装
void incrRefCount(robj *o);//为对象的引用计数增一
robj *resetRefCount(robj *obj);//这个函数将对象的引用计数设为 0 ，但并不释放对象。
void freeStringObject(robj *o);//释放字符串对象
void freeListObject(robj *o);//释放列表对象
void freeSetObject(robj *o);//释放集合对象
void freeZsetObject(robj *o);//释放有序集合对象
void freeHashObject(robj *o);//释放哈希对象
robj *createObject(int type, void *ptr);//创建一个新 robj 对象
robj *createStringObject(char *ptr, size_t len);//递减robj中的引用计数，引用到0后，释放对象
robj *createRawStringObject(char *ptr, size_t len);//创建一个 REDIS_ENCODING_RAW 编码的字符对象,对象的指针指向一个 sds 结构
robj *createEmbeddedStringObject(char *ptr, size_t len);//创建一个 REDIS_ENCODING_EMBSTR 编码的字符对象,这个字符串对象中的 sds 会和字符串对象的 redisObject 结构一起分配,因此这个字符也是不可修改的
robj *dupStringObject(robj *o);//这个函数在复制一个包含整数值的字符串对象时，总是产生一个非共享的对象。
int isObjectRepresentableAsLongLong(robj *o, long long *llongval);//检查对象 o 中的值能否表示为 long long 类型：
robj *tryObjectEncoding(robj *o);//尝试对字符串对象进行编码，以节约内存。
robj *getDecodedObject(robj *o);//如果对象已经是 RAW 编码的，那么对输入对象的引用计数增一，然后返回输入对象。
size_t stringObjectLen(robj *o);//返回字符串对象中字符串值的长度
robj *createStringObjectFromLongLong(long long value);//根据传入的整数值，创建一个字符串对象
robj *createStringObjectFromLongDouble(long double value);//根据传入的 long double 值，为它创建一个字符串对象,对象将 long double 转换为字符串来保存
robj *createListObject(void);//创建一个 LINKEDLIST 编码的列表对象
robj *createZiplistObject(void);//创建一个 ZIPLIST 编码的列表对象
robj *createSetObject(void);//创建一个 SET 编码的集合对象
robj *createIntsetObject(void);//创建一个 INTSET 编码的集合对象
robj *createHashObject(void);//创建一个 ZIPLIST 编码的哈希对象
robj *createZsetObject(void);//创建一个 SKIPLIST 编码的有序集合
robj *createZsetZiplistObject(void);//创建一个 ZIPLIST 编码的有序集合
int getLongFromObjectOrReply(redisClient *c, robj *o, long *target, const char *msg);//尝试从对象 o 中取出 long 类型值，或者尝试将对象 o 中的值转换为 long 类型值，并将这个得出的整数值保存到 *target 。
int checkType(redisClient *c, robj *o, int type);//检查对象 o 的类型是否和 type 相同
int getLongLongFromObjectOrReply(redisClient *c, robj *o, long long *target, const char *msg);//尝试从对象 o 中取出整数值，或者尝试将对象 o 中的值转换为整数值，并将这个得出的整数值保存到 *target 。
int getDoubleFromObjectOrReply(redisClient *c, robj *o, double *target, const char *msg);//尝试从对象 o 中取出 double 值
int getLongLongFromObject(robj *o, long long *target);//如果 o 为 NULL ，那么将 *target 设为 0 。
int getLongDoubleFromObject(robj *o, long double *target);//尝试从对象中取出 long double 值
int getLongDoubleFromObjectOrReply(redisClient *c, robj *o, long double *target, const char *msg);//尝试从对象 o 中取出 long double 值：
char *strEncoding(int encoding);//返回编码的字符串表示
int compareStringObjects(robj *a, robj *b);//以REDIS_COMPARE_BINARY的方式比较两字符串对象
int collateStringObjects(robj *a, robj *b);//以REDIS_COMPARE_COLL的方式比较两字符串对象
int equalStringObjects(robj *a, robj *b);//字符串比较
unsigned long long estimateObjectIdleTime(robj *o);//获取LRU clock，用于LRU算法 
#define sdsEncodedObject(objptr) (objptr->encoding == REDIS_ENCODING_RAW || objptr->encoding == REDIS_ENCODING_EMBSTR)

/* Synchronous I/O with timeout */
ssize_t syncWrite(int fd, char *ptr, ssize_t size, long long timeout);//将指定的负载写入“fd”。如果写入整个有效负载，将在“timeout”毫秒内完成操作，操作成功并返回“size”。否则，操作失败，返回- 1，并且可以对文件描述符执行未指定的部分写入。
ssize_t syncRead(int fd, char *ptr, ssize_t size, long long timeout);//从“fd”读取指定的字节数。如果所有的字节都在“timeout”毫秒内读取，那么将返回操作成功和“size”。否则，操作失败，返回- 1，并且可以从文件描述符读取未指定的数据量。
ssize_t syncReadLine(int fd, char *ptr, ssize_t size, long long timeout);//读取一行，确保每个字符都不需要超过“超时”毫秒。

/* Replication */
void replicationFeedSlaves(list *slaves, int dictid, robj **argv, int argc);//将传入的参数发送给从服务器
void replicationFeedMonitors(redisClient *c, list *monitors, int dictid, robj **argv, int argc);//将协议发给 Monitor
void updateSlavesWaitingBgsave(int bgsaveerr);//这个函数是在 BGSAVE 完成之后的异步回调函数，它指导该怎么执行和 slave 相关的 RDB 下一步工作。
void replicationCron(void);//复制 cron 函数，每秒调用一次
void replicationHandleMasterDisconnection(void);//这个函数在从服务器以外地和主服务器失去联系时调用
void replicationCacheMaster(redisClient *c);//它将当前的 master 记录到 master cache 里面，然后返回。
void resizeReplicationBacklog(long long newsize);//动态调整 backlog 大小,当 backlog 是被扩大时，原有的数据会被保留，因为分配空间使用的是 realloc
void replicationSetMaster(char *ip, int port);//将服务器设为指定地址的从服务器
void replicationUnsetMaster(void);//取消复制，将服务器设置为主服务器
void refreshGoodSlavesCount(void);//计算那些延迟值少于等于 min-slaves-max-lag 的从服务器数量。
void replicationScriptCacheInit(void);//初始化缓存，只在服务器启动时调用
void replicationScriptCacheFlush(void);//清空脚本缓存。
void replicationScriptCacheAdd(sds sha1);//将脚本的 SHA1 添加到缓存中，如果缓存的数量已达到最大值，那么删除最旧的那个脚本（FIFO）
int replicationScriptCacheExists(sds sha1);//如果脚本存在于脚本，那么返回 1 ；否则，返回 0 。
void processClientsWaitingReplicas(void);//检查是否有客户在等待中被阻塞，因为我们从奴隶那里收到了足够的东西。
void unblockClientWaitingReplicas(redisClient *c);//这由unblockClient()调用，以执行阻塞op类型的特定清理。我们只是将客户从等待复制的客户列表中删除。不要直接调用它，而是调用unblockClient()。
int replicationCountAcksByOffset(long long offset);//得到当前已经发送来确认的从节点个数ackreplicas
void replicationSendNewlineToMaster(void);//从客户端发送空行给主客户端，破坏了原本的协议格式，避免让主客户端检测出从客户端超时的情况
long long replicationGetSlaveOffset(void);//得到当前从节点的复制偏移量myoffset

/* Generic persistence functions */
void startLoading(FILE *fp);//在全局状态中标记程序正在进行载入，并设置相应的载入状态。
void loadingProgress(off_t pos);//刷新载入进度信息
void stopLoading(void);//关闭服务器载入状态

/* RDB persistence */
#include "rdb.h"

/* AOF persistence */
void flushAppendOnlyFile(int force);//将 AOF 缓存写入到文件中。
void feedAppendOnlyFile(struct redisCommand *cmd, int dictid, robj **argv, int argc);//将命令追加到 AOF 文件中，如果 AOF 重写正在进行，那么也将命令追加到 AOF 重写缓存中。
void aofRemoveTempFile(pid_t childpid);//删除 AOF 重写所产生的临时文件
int rewriteAppendOnlyFileBackground(void);//刷新缓存区的内容到磁盘中
int loadAppendOnlyFile(char *filename);//执行 AOF 文件中的命令。
void stopAppendOnly(void);//在用户通过 CONFIG 命令在运行时关闭 AOF 持久化时调用
int startAppendOnly(void);//当用户在运行时使用 CONFIG 命令，从 appendonly no 切换到 appendonly yes 时执行
void backgroundRewriteDoneHandler(int exitcode, int bysignal);//当子线程完成 AOF 重写时，父进程调用这个函数。
void aofRewriteBufferReset(void);//清空 AOF 缓冲区
unsigned long aofRewriteBufferSize(void);//返回 AOF 重写缓存当前的大小

/* Sorted sets data type */

/* Struct to hold a inclusive/exclusive range spec by score comparison. */
// 表示开区间/闭区间范围的结构
typedef struct {

    // 最小值和最大值
    double min, max;

    // 指示最小值和最大值是否*不*包含在范围之内
    // 值为 1 表示不包含，值为 0 表示包含
    int minex, maxex; /* are min or max exclusive? */
} zrangespec;

/* Struct to hold an inclusive/exclusive range spec by lexicographic comparison. */
typedef struct {
    robj *min, *max;  /* May be set to shared.(minstring|maxstring) */
    int minex, maxex; /* are min or max exclusive? */
} zlexrangespec;

zskiplist *zslCreate(void);//创建一个层数为 level 的跳跃表节点，并将节点的成员对象设置为 obj ，分值设置为 score 。
void zslFree(zskiplist *zsl);//释放给定的跳跃表节点
zskiplistNode *zslInsert(zskiplist *zsl, double score, robj *obj);//创建一个成员为 obj ，分值为 score 的新节点，并将这个新节点插入到跳跃表 zsl 中。
unsigned char *zzlInsert(unsigned char *zl, robj *ele, double score);//将带有给定成员和分值的新节点插入到 eptr 所指向的节点的前面，如果 eptr 为 NULL ，那么将新节点插入到 ziplist 的末端。
int zslDelete(zskiplist *zsl, double score, robj *obj);//内部删除函数
zskiplistNode *zslFirstInRange(zskiplist *zsl, zrangespec *range);//返回 zsl 中第一个分值符合 range 中指定范围的节点。
zskiplistNode *zslLastInRange(zskiplist *zsl, zrangespec *range);//返回 zsl 中最后一个分值符合 range 中指定范围的节点。
double zzlGetScore(unsigned char *sptr);//取出 sptr 指向节点所保存的有序集合元素的分值
void zzlNext(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);//根据 eptr 和 sptr ，移动它们分别指向下个成员和下个分值。
void zzlPrev(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);//根据 eptr 和 sptr 的值，移动指针指向前一个节点。
unsigned int zsetLength(robj *zobj);//通用排序集API
void zsetConvert(robj *zobj, int encoding);//将跳跃表对象 zobj 的底层编码转换为 encoding 。
unsigned long zslGetRank(zskiplist *zsl, double score, robj *o);//查找包含给定分值和成员对象的节点在跳跃表中的排位。

/* Core functions */
int freeMemoryIfNeeded(void);//淘汰机制触发的函数
int processCommand(redisClient *c);//这个函数执行时，我们已经读入了一个完整的命令到客户端，这个函数负责执行这个命令，或者服务器准备从客户端中进行一次读取。
void setupSignalHandlers(void);//建立信号处理函数
struct redisCommand *lookupCommand(sds name);//根据给定命令名字（SDS），查找命令
struct redisCommand *lookupCommandByCString(char *s);//根据给定命令名字（C 字符串），查找命令
struct redisCommand *lookupCommandOrOriginal(sds name);//从当前命令表 server.commands 中查找给定名字，如果没找到的话，就尝试从 server.orig_commands 中查找未被改名的原始名字,原始表中的命令名不受 redis.conf 中命令改名的影响
void call(redisClient *c, int flags);//调用命令的实现函数，执行命令
void propagate(struct redisCommand *cmd, int dbid, robj **argv, int argc, int flags);//将过期时间传播到附属节点和 AOF 文件
void alsoPropagate(struct redisCommand *cmd, int dbid, robj **argv, int argc, int target);//使用内部命令在当前命令传播到AOF / Replication后调度附加命令的传播。
void forceCommandPropagation(redisClient *c, int flags);//强制将特定命令执行的传播引入到AOF / Replication中。
int prepareForShutdown();//????
#ifdef __GNUC__
void redisLog(int level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));//将给定的数据写入日志文件，和常用的printf 函数用法差不多
#else
void redisLog(int level, const char *fmt, ...);
#endif
void redisLogRaw(int level, const char *msg);//低水level志记录。仅用于非常大的消息，否则redisLog()就是喜欢。
void redisLogFromHandler(int level, const char *msg);//以一种安全的方式从一个信号处理程序中调用一个固定的消息，而不需要任何类似于打印的功能。
void usage();//???
void updateDictResizePolicy(void);//这个函数被称为某个类型的后台进程，因为我们希望避免在有子的时候调整哈希表，以便在编写时能够很好地发挥作用(否则，当有大量内存分页被复制时)。这个函数的目标是更新dict . c的能力来调整哈希表的大小，从而使我们没有运行childs。
int htNeedsResize(dict *dict);//判断是否可以需要进行dict缩小的条件判断,填充率必须>10%，否则会进行缩小
void oom(const char *msg);//???
void populateCommandTable(void);//根据 redis.c 文件顶部的命令列表，创建命令表
void resetCommandTableStats(void);//重置命令表中的统计信息
void adjustOpenFilesLimit(void);//此函数将尝试根据客户端配置最大数量的打开文件。它还保留了一些文件描述符(REDIS_MIN_RESERVED_FDS)，用于额外的持久性、监听套接字、日志文件等等。
void closeListeningSockets(int unlink_unix_socket);//关闭监听套接字
void updateCachedTime(void);//我们在全局状态中获取了unix时间的缓存值，因为有了虚拟内存和老化，就可以将当前时间存储在每个对象访问的对象中，并且不需要精度。访问全局变量比调用时间(NULL)要快得多。
void resetServerStats(void);//重新设置通过INFO或其他方式公开的数据，我们希望通过配置RESETSTAT来重置。该函数还用于在服务器启动时在initServer()中初始化这些字段。
unsigned int getLRUClock(void);//获取当前系统的毫秒数

/* Set data type */
robj *setTypeCreate(robj *value);//返回一个可以保存值 value 的集合。
int setTypeAdd(robj *subject, robj *value);//多态 add 操作
int setTypeRemove(robj *subject, robj *value);//多态 remove 操作
int setTypeIsMember(robj *subject, robj *value);//多态 ismember 操作
setTypeIterator *setTypeInitIterator(robj *subject);//创建并返回一个多态集合迭代器
void setTypeReleaseIterator(setTypeIterator *si);//释放迭代器
int setTypeNext(setTypeIterator *si, robj **objele, int64_t *llele);//取出被迭代器指向的当前集合元素。
robj *setTypeNextObject(setTypeIterator *si);//在非 copy-on-write 时调用
int setTypeRandomElement(robj *setobj, robj **objele, int64_t *llele);//从非空集合中随机取出一个元素。
unsigned long setTypeSize(robj *subject);//集合多态 size 函数
void setTypeConvert(robj *subject, int enc);//将集合对象 setobj 的编码转换为 REDIS_ENCODING_HT 。

/* Hash data type */
void hashTypeConvert(robj *o, int enc);//将一个 ziplist 编码的哈希对象 o 转换成其他编码
void hashTypeTryConversion(robj *subject, robj **argv, int start, int end);//对 argv 数组中的多个对象进行检查，看是否需要将对象的编码从 REDIS_ENCODING_ZIPLIST 转换成 REDIS_ENCODING_HT
void hashTypeTryObjectEncoding(robj *subject, robj **o1, robj **o2);//当 subject 的编码为 REDIS_ENCODING_HT 时，尝试对对象 o1 和 o2 进行编码，以节省更多内存。
robj *hashTypeGetObject(robj *o, robj *key);//多态 GET 函数，从 hash 中取出域 field 的值，并返回一个值对象。
int hashTypeExists(robj *o, robj *key);//检查给定域 feild 是否存在于 hash 对象 o 中。
int hashTypeSet(robj *o, robj *key, robj *value);//这个函数负责对 field 和 value 参数进行引用计数自增。
int hashTypeDelete(robj *o, robj *key);//将给定 field 及其 value 从哈希表中删除
unsigned long hashTypeLength(robj *o);//返回哈希表的 field-value 对数量
hashTypeIterator *hashTypeInitIterator(robj *subject);//创建一个哈希类型的迭代器
void hashTypeReleaseIterator(hashTypeIterator *hi);//释放迭代器
int hashTypeNext(hashTypeIterator *hi);//获取哈希中的下一个节点，并将它保存到迭代器。
void hashTypeCurrentFromZiplist(hashTypeIterator *hi, int what,
                                unsigned char **vstr,
                                unsigned int *vlen,
                                long long *vll);//从 ziplist 编码的哈希中，取出迭代器指针当前指向节点的域或值。
void hashTypeCurrentFromHashTable(hashTypeIterator *hi, int what, robj **dst);//根据迭代器的指针，从字典编码的哈希中取出所指向节点的 field 或者 value 。
robj *hashTypeCurrentObject(hashTypeIterator *hi, int what);//一个非 copy-on-write 友好，但是层次更高的 hashTypeCurrent() 函数，这个函数返回一个增加了引用计数的对象，或者一个新对象。
robj *hashTypeLookupWriteOrCreate(redisClient *c, robj *key);//按 key 在数据库中查找并返回相应的哈希对象，如果对象不存在，那么创建一个新哈希对象并返回。

/* Pub / Sub */
int pubsubUnsubscribeAllChannels(redisClient *c, int notify);//退订客户端 c 订阅的所有频道。
int pubsubUnsubscribeAllPatterns(redisClient *c, int notify);//退订客户端 c 订阅的所有模式。
void freePubsubPattern(void *p);//释放给定的模式 p
int listMatchPubsubPattern(void *a, void *b);//对比模式 a 和 b 是否相同，相同返回 1 ，不相同返回 0 。
int pubsubPublishMessage(robj *channel, robj *message);//将 message 发送到所有订阅频道 channel 的客户端，以及所有订阅了和 channel 频道匹配的模式的客户端。

/* Keyspace events notification */
void notifyKeyspaceEvent(int type, char *event, robj *key, int dbid);//向Redis核心提供一个简单的API函数
int keyspaceEventsStringToFlags(char *classes);//对传入的字符串参数进行分析， 给出相应的 flags 值
sds keyspaceEventsFlagsToString(int flags);//根据 flags 值还原设置这个 flags 所需的字符串

/* Configuration */
void loadServerConfig(char *filename, char *options);//从给定文件中载入服务器配置。
void appendServerSaveParams(time_t seconds, int changes);//追加server save参数
void resetServerSaveParams();//重置server的save参数，即释放server的serverParams
struct rewriteConfigState; /* Forward declaration to export API. 存了属性配置的字符串数组，一个字符串数组代表一种属性设置*/
void rewriteConfigRewriteLine(struct rewriteConfigState *state, char *option, sds line, int force);//是否覆盖configline
int rewriteConfig(char *path);//将当前的属性读入到文件中，步骤:(1).将当前server属性读入configstate(2).configstate属性变为字符串(3).将字符串写入文件

/* db.c -- Keyspace access API */
int removeExpire(redisDb *db, robj *key);//移除键 key 的过期时间
void propagateExpire(redisDb *db, robj *key);//将过期时间传播到附属节点和 AOF 文件。
int expireIfNeeded(redisDb *db, robj *key);//检查 key 是否已经过期，如果是的话，将它从数据库中删除。
long long getExpire(redisDb *db, robj *key);//返回给定 key 的过期时间。
void setExpire(redisDb *db, robj *key, long long when);//将键 key 的过期时间设为 when
robj *lookupKey(redisDb *db, robj *key);//从数据库 db 中取出键 key 的值（对象）,如果 key 的值存在，那么返回该值；否则，返回 NULL 。
robj *lookupKeyRead(redisDb *db, robj *key);//为执行读取操作而取出键 key 在数据库 db 中的值。
robj *lookupKeyWrite(redisDb *db, robj *key);//为执行写入操作而取出键 key 在数据库 db 中的值。
robj *lookupKeyReadOrReply(redisClient *c, robj *key, robj *reply);//为执行读取操作而从数据库中查找返回 key 的值。
robj *lookupKeyWriteOrReply(redisClient *c, robj *key, robj *reply);//为执行写入操作而从数据库中查找返回 key 的值。
void dbAdd(redisDb *db, robj *key, robj *val);//尝试将键值对 key 和 val 添加到数据库中。
void dbOverwrite(redisDb *db, robj *key, robj *val);//为已存在的键关联一个新值。
void setKey(redisDb *db, robj *key, robj *val);//高层次的 SET 操作函数。这个函数可以在不管键 key 是否存在的情况下，将它和 val 关联起来。
int dbExists(redisDb *db, robj *key);//检查键 key 是否存在于数据库中，存在返回 1 ，不存在返回 0 。
robj *dbRandomKey(redisDb *db);//随机从数据库中取出一个键，并以字符串对象的方式返回这个键。
int dbDelete(redisDb *db, robj *key);//从数据库中删除给定的键，键的值，以及键的过期时间。
robj *dbUnshareStringValue(redisDb *db, robj *key, robj *o);//将string对象的内部编码转成OBJ_ENCODING_RAW的（只有这种编码的robj对象，其内部的sds 才能在后面自由追加新的内容），并解除可能存在的对象共享状态。
long long emptyDb(void(callback)(void*));//清空服务器的所有数据。
int selectDb(redisClient *c, int id);//将客户端的目标数据库切换为 id 所指定的数据库
void signalModifiedKey(redisDb *db, robj *key);//键空间改动的钩子。
void signalFlushedDb(int dbid);//把dbid中的key都touch一遍
unsigned int getKeysInSlot(unsigned int hashslot, robj **keys, unsigned int count);//记录 count 个属于 hashslot 槽的键到 keys 数组，并返回被记录键的数量
unsigned int countKeysInSlot(unsigned int hashslot);//返回指定 slot 包含的键数量
unsigned int delKeysInSlot(unsigned int hashslot);//删除指定哈希槽中的所有键。已删除的项的数量被返回。
int verifyClusterConfigWithData(void);//检查当前节点的节点配置是否正确，包含的数据是否正确,在启动集群时被调用（看 redis.c ）
void scanGenericCommand(redisClient *c, robj *o, unsigned long cursor);//这是 SCAN 、 HSCAN 、 SSCAN 命令的实现函数。
int parseScanCursorOrReply(redisClient *c, robj *o, unsigned long *cursor);//尝试解析存储在对象“o”中的扫描光标:如果光标是有效的，将它存储为无符号整数，然后返回redisok。否则返回REDIS_ERR并向客户机发送错误。

/* API to get key arguments from commands */
int *getKeysFromCommand(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);//如果不需要特定于命令的助手函数，则该函数使用命令表，否则它将调用特定于命令的函数。
void getKeysFreeResult(int *result);//释放getKeysFromCommand的结果
int *zunionInterGetKeys(struct redisCommand *cmd,robj **argv, int argc, int *numkeys);//帮助函数从命令中提取键
int *evalGetKeys(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);//帮助函数从命令中提取键
int *sortGetKeys(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);//帮助函数从排序命令中提取密钥

/* Cluster */
void clusterInit(void);//初始化集群
unsigned short crc16(const char *buf, int len);//???
unsigned int keyHashSlot(char *key, int keylen);//计算给定键应该被分配到那个槽
void clusterCron(void);//集群常规操作函数，默认每秒执行 10 次（每间隔 100 毫秒执行一次）
void clusterPropagatePublish(robj *channel, robj *message);//向整个集群的 channel 频道中广播消息 messages
void migrateCloseTimedoutSockets(void);//移除过期的连接，由 redis.c/serverCron() 调用
void clusterBeforeSleep(void);//在进入下个事件循环时调用。这个函数做的事都是需要尽快执行，但是不能在执行文件事件期间做的事情。

/* Sentinel */
void initSentinelConfig(void);//这个函数会用 Sentinel 所属的属性覆盖服务器默认的属性
void initSentinel(void);//以 Sentinel 模式初始化服务器
void sentinelTimer(void);//sentinel 模式的主函数，由 redis.c/serverCron 函数调用
char *sentinelHandleConfiguration(char **argv, int argc);//Sentinel 配置文件分析器
void sentinelIsRunning(void);//这个函数在 Sentinel 准备就绪，可以执行操作时执行

/* Scripting */
void scriptingInit(void);//初始化环境

/* Blocked clients */
void processUnblockedClients(void);//取消所有在 unblocked_clients 链表中的客户端的阻塞状态
void blockClient(redisClient *c, int btype);//对给定的客户端进行阻塞
void unblockClient(redisClient *c);//取消给定的客户端的阻塞状态
void replyToBlockedClientTimedOut(redisClient *c);//等待超时，向被阻塞的客户端返回通知
int getTimeoutFromObjectOrReply(redisClient *c, robj *object, mstime_t *timeout, int unit);//根据输入参数，取出最大等待时间

/* Git SHA1 */
char *redisGitSHA1(void);//??
char *redisGitDirty(void);//??
uint64_t redisBuildId(void);//??

/* Commands prototypes */
void authCommand(redisClient *c);//密码验证信息
void pingCommand(redisClient *c);//ping 命令处理函数
void echoCommand(redisClient *c);
void setCommand(redisClient *c);
void setnxCommand(redisClient *c);
void setexCommand(redisClient *c);
void psetexCommand(redisClient *c);
void getCommand(redisClient *c);
void delCommand(redisClient *c);
void existsCommand(redisClient *c);
void setbitCommand(redisClient *c);
void getbitCommand(redisClient *c);
void setrangeCommand(redisClient *c);
void getrangeCommand(redisClient *c);
void incrCommand(redisClient *c);
void decrCommand(redisClient *c);
void incrbyCommand(redisClient *c);
void decrbyCommand(redisClient *c);
void incrbyfloatCommand(redisClient *c);
void selectCommand(redisClient *c);
void randomkeyCommand(redisClient *c);
void keysCommand(redisClient *c);
void scanCommand(redisClient *c);
void dbsizeCommand(redisClient *c);
void lastsaveCommand(redisClient *c);
void saveCommand(redisClient *c);
void bgsaveCommand(redisClient *c);
void bgrewriteaofCommand(redisClient *c);
void shutdownCommand(redisClient *c);
void moveCommand(redisClient *c);
void renameCommand(redisClient *c);
void renamenxCommand(redisClient *c);
void lpushCommand(redisClient *c);
void rpushCommand(redisClient *c);
void lpushxCommand(redisClient *c);
void rpushxCommand(redisClient *c);
void linsertCommand(redisClient *c);
void lpopCommand(redisClient *c);
void rpopCommand(redisClient *c);
void llenCommand(redisClient *c);
void lindexCommand(redisClient *c);
void lrangeCommand(redisClient *c);
void ltrimCommand(redisClient *c);
void typeCommand(redisClient *c);
void lsetCommand(redisClient *c);
void saddCommand(redisClient *c);
void sremCommand(redisClient *c);
void smoveCommand(redisClient *c);
void sismemberCommand(redisClient *c);
void scardCommand(redisClient *c);
void spopCommand(redisClient *c);
void srandmemberCommand(redisClient *c);
void sinterCommand(redisClient *c);
void sinterstoreCommand(redisClient *c);
void sunionCommand(redisClient *c);
void sunionstoreCommand(redisClient *c);
void sdiffCommand(redisClient *c);
void sdiffstoreCommand(redisClient *c);
void sscanCommand(redisClient *c);
void syncCommand(redisClient *c);
void flushdbCommand(redisClient *c);
void flushallCommand(redisClient *c);
void sortCommand(redisClient *c);
void lremCommand(redisClient *c);
void rpoplpushCommand(redisClient *c);
void infoCommand(redisClient *c);
void mgetCommand(redisClient *c);
void monitorCommand(redisClient *c);
void expireCommand(redisClient *c);
void expireatCommand(redisClient *c);
void pexpireCommand(redisClient *c);
void pexpireatCommand(redisClient *c);
void getsetCommand(redisClient *c);
void ttlCommand(redisClient *c);
void pttlCommand(redisClient *c);
void persistCommand(redisClient *c);
void slaveofCommand(redisClient *c);
void debugCommand(redisClient *c);
void msetCommand(redisClient *c);
void msetnxCommand(redisClient *c);
void zaddCommand(redisClient *c);
void zincrbyCommand(redisClient *c);
void zrangeCommand(redisClient *c);
void zrangebyscoreCommand(redisClient *c);
void zrevrangebyscoreCommand(redisClient *c);
void zrangebylexCommand(redisClient *c);
void zrevrangebylexCommand(redisClient *c);
void zcountCommand(redisClient *c);
void zlexcountCommand(redisClient *c);
void zrevrangeCommand(redisClient *c);
void zcardCommand(redisClient *c);
void zremCommand(redisClient *c);
void zscoreCommand(redisClient *c);
void zremrangebyscoreCommand(redisClient *c);
void zremrangebylexCommand(redisClient *c);
void multiCommand(redisClient *c);
void execCommand(redisClient *c);
void discardCommand(redisClient *c);
void blpopCommand(redisClient *c);
void brpopCommand(redisClient *c);
void brpoplpushCommand(redisClient *c);
void appendCommand(redisClient *c);
void strlenCommand(redisClient *c);
void zrankCommand(redisClient *c);
void zrevrankCommand(redisClient *c);
void hsetCommand(redisClient *c);
void hsetnxCommand(redisClient *c);
void hgetCommand(redisClient *c);
void hmsetCommand(redisClient *c);
void hmgetCommand(redisClient *c);
void hdelCommand(redisClient *c);
void hlenCommand(redisClient *c);
void zremrangebyrankCommand(redisClient *c);
void zunionstoreCommand(redisClient *c);
void zinterstoreCommand(redisClient *c);
void zscanCommand(redisClient *c);
void hkeysCommand(redisClient *c);
void hvalsCommand(redisClient *c);
void hgetallCommand(redisClient *c);
void hexistsCommand(redisClient *c);
void hscanCommand(redisClient *c);
void configCommand(redisClient *c);
void hincrbyCommand(redisClient *c);
void hincrbyfloatCommand(redisClient *c);
void subscribeCommand(redisClient *c);
void unsubscribeCommand(redisClient *c);
void psubscribeCommand(redisClient *c);
void punsubscribeCommand(redisClient *c);
void publishCommand(redisClient *c);
void pubsubCommand(redisClient *c);
void watchCommand(redisClient *c);
void unwatchCommand(redisClient *c);
void clusterCommand(redisClient *c);
void restoreCommand(redisClient *c);
void migrateCommand(redisClient *c);
void askingCommand(redisClient *c);
void readonlyCommand(redisClient *c);
void readwriteCommand(redisClient *c);
void dumpCommand(redisClient *c);
void objectCommand(redisClient *c);
void clientCommand(redisClient *c);
void evalCommand(redisClient *c);
void evalShaCommand(redisClient *c);
void scriptCommand(redisClient *c);
void timeCommand(redisClient *c);
void bitopCommand(redisClient *c);
void bitcountCommand(redisClient *c);
void bitposCommand(redisClient *c);
void replconfCommand(redisClient *c);
void waitCommand(redisClient *c);
void pfselftestCommand(redisClient *c);
void pfaddCommand(redisClient *c);
void pfcountCommand(redisClient *c);
void pfmergeCommand(redisClient *c);
void pfdebugCommand(redisClient *c);

#if defined(__GNUC__)
void *calloc(size_t count, size_t size) __attribute__ ((deprecated));//在内存的动态存储区中分配n个长度为size的连续空间，函数返回一个指向分配起始地址的指针；如果分配不成功，返回NULL。
void free(void *ptr) __attribute__ ((deprecated));//释放
void *malloc(size_t size) __attribute__ ((deprecated));
void *realloc(void *ptr, size_t size) __attribute__ ((deprecated));
#endif

/* Debugging stuff */
void _redisAssertWithInfo(redisClient *c, robj *o, char *estr, char *file, int line);//函数会打印 Redis 服务器当前服务的客户端和某个关键 Redis 对象的信息
void _redisAssert(char *estr, char *file, int line);//应急处理
void _redisPanic(char *msg, char *file, int line);//???
void bugReportStart(void);//????
void redisLogObjectDebugInfo(robj *o);//???
void sigsegvHandler(int sig, siginfo_t *info, void *secret);//????
sds genRedisInfoString(char *section);//创建由INFO命令返回的字符串。这是由INFO命令本身解耦的，因为我们需要在内存损坏问题上报告相同的信息。
void enableWatchdog(int period);//允许软件监视程序以毫秒为单位指定周期。
void disableWatchdog(void);//禁用软件监视程序。
void watchdogScheduleSignal(int period);//在指定的周期后以毫秒为单位安排一个SIGALRM交付。如果计时器已经安排好了，这个函数将重新安排到指定的时间。如果周期为0，则当前计时器禁用。
void redisLogHexDump(int level, char *descr, void *value, size_t len);//日志记录功能调试

#define redisDebug(fmt, ...) \
    printf("DEBUG %s:%d > " fmt "\n", __FILE__, __LINE__, __VA_ARGS__)
#define redisDebugMark() \
    printf("-- MARK %s:%d --\n", __FILE__, __LINE__)

#endif
