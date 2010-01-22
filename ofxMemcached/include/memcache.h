/* -*- mode: C -*-
 *
 * Copyright (c) 2004-2005 Sean Chittenden <sean@chittenden.org>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * The following copyright is included as the TAILQ_* macros come from
 * sys/queue.h which has the following LICENSE/Copyright notice.  XXX
 *
 * Copyright (c) 1991, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      @(#)queue.h     8.5 (Berkeley) 8/20/94
 * $FreeBSD: src/sys/sys/queue.h,v 1.58 2004/04/07 04:19:49 imp Exp $
 */

#ifndef MEMCACHE_H
#define MEMCACHE_H

#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef __MEMCACHE__
# define mc_const
#else
# define mc_const const
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Macros for the platform int definitions */
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;


/* Macros for testing versions */
#define MEMCACHE_VER		"1.4.0.rc2"
#define MEMCACHE_VERNUM		10400
#define MEMCACHE_RELDATE	20060220

/* A handful of #define's for callbacks that discourage developers
 * from improperly using the interface. */
#define MCM_CALLBACK_CTXT	md597f78ab4a96c3e71c3bb9fe036f37367
#define MCM_CALLBACK_KEY	md5783365dee46035fa36d8d9e6fe15da0f
#define MCM_CALLBACK_LEN	md54c1f40392e708222b8a25a4337cd05fd
#define MCM_CALLBACK_MC		md5646277084faa5a0f511793a728521ef5
#define MCM_CALLBACK_PTR	md57f5686851aeded9e00761d4aaaac20ee
#define MCM_CALLBACK_RES	md52e8a13a74753effc0f149a3ce900dc28
#define MCM_ERR_MASK		md5fc9f4e2249ad88de0081136fef04defa
#define MCM_ERR_FUNC_ERR_CTXT	md5096953205b8982964e25927bd8154148
#define MCM_ERR_FUNC_MC_CTXT	md521e67c8edeaac774b00d960e230c8686
#define MCM_KEY_VALID_KEY	md58926735b19e189407440aa1ba3ab1962
#define MCM_KEY_VALID_LEN	md5d6077405f1a7b35ddeac18ccc2a8f4c7

/* Our initial read(2) buffer has to be long enough to read the
 * first line of the response.  ie:
 *
 * "VALUE #{'k' * 250} #{2 ** 15} #{2 ** 32}\r\n.length => 275
 *
 * However, since we want to avoid the number of system calls
 * necessary, include trailing part of the protocol in our estimate:
 *
 * "\r\nEND\r\n".length => 7
 *
 * Which yields a mandatory limit of 282 bytes for a successful
 * response.  If we wish to try and get lucky with our first read(2)
 * call and be able to read(2) in small values without making a second
 * read(2) call, pad this number with a sufficiently large byte value.
 * If most of your keys are 512B, then a INIT_GET_BUF_SIZE of 794
 * would be prudent (512 + 282).
 *
 * The default value of 4096 means that values less than 724 bytes
 * will always be read(2) via the first read(2) call.  Increasing this
 * value to large values is not beneficial.  If a second read(2) call
 * is necessary, the read(2) will be made with a sufficiently large
 * buffer already allocated. */
#ifndef INIT_GET_BUF_SIZE
# define INIT_GET_BUF_SIZE ((size_t)4096)
#endif

#define MAX_KEY_LEN 250

/* Error severity levels */
#define MCM_ERR_LVL_NONE	0x00
/* INFO: Execution information */
#define MCM_ERR_LVL_INFO	0x01
/* NOTICE: Routine usage information (ie: deactivated a down server) */
#define MCM_ERR_LVL_NOTICE	0x02
/* WARN: Problem during execution (ie: invalid data of sorts) */
#define MCM_ERR_LVL_WARN	0x04
/* ERR: Severe problem during execution (ie: OS system/library call failed) */
#define MCM_ERR_LVL_ERR		0x08
/* FATAL: Unable to proceed with execution (ie: protocol ) */
#define MCM_ERR_LVL_FATAL	0x10

/* MC_ compatiblity for severity */
#define MC_ERR_LVL_INFO		MCM_ERR_LVL_INFO
#define MC_ERR_LVL_NOTICE	MCM_ERR_LVL_NOTICE
#define MC_ERR_LVL_WARN		MCM_ERR_LVL_WARN
#define MC_ERR_LVL_ERR		MCM_ERR_LVL_ERR
#define MC_ERR_LVL_FATAL	MCM_ERR_LVL_FATAL

/* Set various error codes */
#define MCM_ERR_NONE			 0
#define MCM_ERR_ASSERT		 	 1
#define MCM_ERR_LIB_SNPRINTF		 2
#define MCM_ERR_LIB_STRTOL		 3
#define MCM_ERR_LIB_STRTOLL		 4
#define MCM_ERR_MC_RECONN		 5
#define MCM_ERR_MC_SEND_CMD		 6
#define MCM_ERR_MC_SERV_LIST		 7
#define MCM_ERR_MC_STORE		 8
#define MCM_ERR_MC_VALID_SERVER		 9
#define MCM_ERR_MEM_MALLOC		10
#define MCM_ERR_MEM_REALLOC		11
#define MCM_ERR_NET_CONNECT		12
#define MCM_ERR_NET_HOST		13
#define MCM_ERR_PROTO			14
#define MCM_ERR_SYS_CLOSE		16
#define MCM_ERR_SYS_CONNECT		17
#define MCM_ERR_SYS_FCNTL		18
#define MCM_ERR_SYS_READ		19
#define MCM_ERR_SYS_SELECT		20
#define MCM_ERR_SYS_SETSOCKOPT		21
#define MCM_ERR_SYS_SOCKET		22
#define MCM_ERR_SYS_WRITEV		23
#define MCM_ERR_TEST			24
#define MCM_ERR_TIMEOUT			25
#define MCM_ERR_TRACE			26
#define MCM_ERR_UNKNOWN_STAT		27


/* Various values for _flags.  Use their function counterparts instead
 * of testing these bits directly (ie: mcm_res_free_on_delete(),
 * mcm_res_found(), and mcm_res_attempted()). */
#define MCM_RES_FREE_ON_DELETE		0x01
#define MCM_RES_NO_FREE_ON_DELETE	0x02
#define MCM_RES_FOUND			0x04
#define MCM_RES_ATTEMPTED		0x08
#define MCM_RES_NEED_FREE_KEY		0x10

/* Aliases for MCM_RES_* #define's. Use their function counterparts
 * instead of testing these bits directly (ie:
 * mc_res_free_on_delete(), mc_res_found(), and
 * mc_res_attempted()).  */
#define MC_RES_FREE_ON_DELETE		MCM_RES_FREE_ON_DELETE
#define MC_RES_NO_FREE_ON_DELETE	MCM_RES_NO_FREE_ON_DELETE
#define MC_RES_FOUND			MCM_RES_FOUND
#define MC_RES_ATTEMPTED		MCM_RES_ATTEMPTED
#define MC_RES_NEED_FREE_KEY		MCM_RES_NEED_FREE_KEY

/* A convenience macro that lets people avoid the expense of strlen(3)
 * if they're using a string that's defined at compile time. */
#define MCM_CSTRLEN(_str) (sizeof(_str) - 1)

/*
 * Function authors MUST use the following two macros instead of
 * explicitly defining the various struct members.  These MD5s *will*
 * change every release to ensure developers (ie, YOU!) use these
 * macros.  Consider yourself warned. */

/* Error function signature and arguments */
#define MCM_CALLBACK_FUNC	const struct memcache_ctxt *MCM_CALLBACK_CTXT, struct memcache_res *MCM_CALLBACK_RES, void *MCM_CALLBACK_PTR
#define MCM_CALLBACK_SIG	const struct memcache_ctxt *, struct memcache_res *, void *
#define MCM_ERR_FUNC_ARGS	const void *MCM_ERR_FUNC_MC_CTXT, void *MCM_ERR_FUNC_ERR_CTXT
#define MCM_ERR_FUNC_SIG	const void *, void *

/* This see memcache.c:mcm_err_func() for an example on how to use
 * this. */
#define MCM_ERR_INIT_CTXT(_ctxt, _ectxt) do { \
	_ctxt = (const struct memcache_ctxt *)MCM_ERR_FUNC_MC_CTXT; \
	_ectxt = (struct memcache_err_ctxt *)MCM_ERR_FUNC_ERR_CTXT; \
} while (0)

/* Key validation function signature and arguments */
#define MCM_KEY_VALID_FUNC_ARGS	const void *MCM_KEY_VALID_CTXT, char *MCM_KEY_VALID_KEY, size_t MCM_KEY_VALID_LEN
#define MCM_KEY_VALID_FUNC_SIG	const void *, char *, size_t

#define MCM_KEY_VALID_INIT(_ctxt, _key, _len) do { \
	_ctxt = (const struct memcache_ctxt *)MCM_KEY_VALID_CTXT; \
	_key = (char *)MCM_KEY_VALID_KEY; \
	_len = MCM_KEY_VALID_LEN; \
} while (0)

/* Hash function signature and arguments.
 *
 * Arg 1: struct memcache_ctxt *
 * Arg 2: struct memcache *
 * Arg 3: key
 * Arg 4: key length
 */
#define MCM_HASH_FUNC		const void *MCM_CALLBACK_CTXT, const void *MCM_CALLBACK_MC, const char *MCM_CALLBACK_KEY, const size_t MCM_CALLBACK_LEN
#define MCM_HASH_SIG		const void *, const void *, const char *, const size_t

#define MCM_HASH_INIT(_ctxt, _mc, _key, _len) do { \
	_ctxt = (const struct memcache_ctxt *)MCM_CALLBACK_CTXT; \
	_mc = (const struct memcache *)MCM_CALLBACK_MC; \
	_key = (const char *)MCM_CALLBACK_KEY; \
	_len = MCM_CALLBACK_LEN; \
} while (0)

/* Begin various TAILQ macros */
#define TRASHIT(x)	do {(x) = (void *)-1;} while (0)

#define TAILQ_HEAD_INITIALIZER(head)	{ NULL, &(head).tqh_first }

#define TAILQ_HEAD(name, type)					\
struct name {							\
	struct type *tqh_first; /* first element */		\
	struct type **tqh_last; /* addr of last next element */	\
}

#define TAILQ_ENTRY(type)						\
struct {								\
	struct type *tqe_next;	/* next element */			\
	struct type **tqe_prev;	/* address of previous next element */	\
}

#define TAILQ_FIRST(head)	((head)->tqh_first)

#define TAILQ_NEXT(elm, field)	((elm)->field.tqe_next)

#define TAILQ_FOREACH(var, head, field)                                 \
	for (var = TAILQ_FIRST(head); var; var = TAILQ_NEXT(var, field))

#define TAILQ_INIT(head) do {				\
	TAILQ_FIRST((head)) = NULL;			\
	(head)->tqh_last = &TAILQ_FIRST((head));	\
} while (0)

#define TAILQ_INSERT_TAIL(head, elm, field) do {	\
	TAILQ_NEXT((elm), field) = NULL;		\
	(elm)->field.tqe_prev = (head)->tqh_last;	\
	*(head)->tqh_last = (elm);			\
	(head)->tqh_last = &TAILQ_NEXT((elm), field);	\
} while (0)

#define TAILQ_REMOVE(head, elm, field) do {						\
	if ((TAILQ_NEXT((elm), field)) != NULL)						\
		TAILQ_NEXT((elm), field)->field.tqe_prev = (elm)->field.tqe_prev;	\
	else										\
		(head)->tqh_last = (elm)->field.tqe_prev;				\
	*(elm)->field.tqe_prev = TAILQ_NEXT((elm), field);				\
	TRASHIT((elm)->field.tqe_next);							\
	TRASHIT((elm)->field.tqe_prev);							\
} while (0)
/* End various TAILQ macros */


/* Load struct memcache_buf, but none of the prototypes that operate
 * on struct memcache_buf. */
#include <memcache/_buffer.h>


/* Error handling information struct */
struct memcache_err_ctxt {
  /* A generic pointer not used by memcache(3), but can be used by
   * calling programs. */
  void *misc;

  /* The function name that generated the error */
  const char *funcname;

  /* The line number in source file that had the problem */
  mc_const u_int32_t lineno;

  /* The errno.  If zero, there was no errno set. */
  mc_const u_int32_t errnum;

  /* memcache(3) specific error code */
  mc_const u_int32_t errcode;

  /* Severity of the error (INFO, NOTICE, WARN, ERR, FATAL) */
  mc_const char severity;

  /* Should we continue executing after we return from this error
   * handler?  'y'es, 'n'o, or 'a'bort?  Handlers can modify this
   * value and change the execution of the error handler
   * dispatcher. */
  char cont;

  /* An optional return code that error handlers can set to return
   * application specific status codes to memcache(3) callers.  Where
   * integers are returned, this value will be returned to the caller
   * if it is a non-zero value. */
  int32_t retcode;

  /* If memcache(3) causes the program to exit from an error handler,
   * it will use the following exit status. */
  int sysexit;

  /* The error message that pertains to the given error code */
  const char *errstr;

  /* Per-error specific error message (null terminated) */
  const char *errmsg;

  /* The length of the error message. */
  mc_const size_t errlen;

  /* Pointer to the struct memcache_ctxt of the calling function. */
  const void *ctxt;
};

/* The memcache API allows callers to provide their own memory
 * allocation routines to aid in embedability with existing programs,
 * libraries, programming languages, and environments that have their
 * own memory handling routines. */
typedef void	 (*mcFreeFunc)(void *mem);
typedef void	*(*mcMallocFunc)(const size_t size);
typedef void	*(*mcReallocFunc)(void *mem, const size_t size);

/* See the MCM_ERR_FUNC_* macros for details on the arguments and
 * memcache.c:mcm_err_func() for an example of how to use this. */
typedef int32_t	 (*mcErrFunc)(MCM_ERR_FUNC_ARGS);

/* Function prototype for scanning keys and either changing values of
 * invalid keys, or returning an error code. See the MCM_KEY_VALID_*
 * macros to gain access to the passed in values. */
typedef int32_t	 (*mcKeyValidFunc)(const void *, char *, size_t);

/* Function prototype for hashing keys.  See MCM_HASH_FUNC docs for
 * argument explaination. */
typedef u_int32_t	(*mcHashKeyFunc)(MCM_HASH_FUNC);

/* Function prototype for finding a server.
 *
 * Arg1: struct memcache_ctxt *
 * Arg2: struct memcache *
 * Arg3: hash value */
typedef void *(*mcServerFindFunc)(const void *, void *, const u_int32_t);

/* This structure is only used to support multiple memory contexts.
 * By default, all memcache(3) API calls use the global memory
 * context, mcGlobalCtxt.  Under special circumstances (ie, Apache),
 * it is necessary to have multiple memory contexts that correspond
 * with their various different calling libraries (PHP, PostgreSQL,
 * APR, etc).  struct memcache_ctxt and its friends mcm_*() are used
 * to fulfill this goal.  Under most instances, programs, use of
 * mc_*() is sufficient, however there is nothing wrong with defining
 * your own memory context.
 *
 * mcMallocAtomic is used where applicable in the event that the
 * calling application makes use of a garbage collection mechanism
 * (ie, Boehm).  In non-GC'ed environments, this should be set to the
 * same things as mcMalloc.  When allocating a struct memcache_ctxt
 * object, it is absolutely necessary to allocate a new context and
 * assign it to call mcMemNewCtxt() instead of independently creating
 * a struct memcache_ctxt object (ie: "struct memcache_ctxt *ctxt;
 * ctxt = mcMemNewCtxt(...);" == good.  "struct memcache_ctxt ctxt;
 * bzero(ctxt, sizeof(struct memcache_ctxt));" == bad. */
struct memcache_ctxt {
  /* Memory context function pointers. */
  mcFreeFunc	mcFree;
  mcMallocFunc	mcMalloc;
  mcMallocFunc	mcMallocAtomic;
  mcReallocFunc	mcRealloc;

  /* Error handling system.  Users only need to worry about having
   * mcErrFunc().  All warnings, etc., go through this function.  See
   * struct mcm_errctxt for details on information passed to the
   * handler. */
  mcErrFunc mcErr;

  /* Key validation system.  Certain applications need to verify the
   * contents of keys and ensure that various values are contained
   * inside of keys.  If this function pointer is not null, before
   * keys are sent to the server, they will be passed to this function
   * pointer for testing/modification.  All modifications to the key
   * must be in place and can not change the memory location or the
   * length of the key. */
  mcKeyValidFunc mcKeyValid;

  /* Key hash function. */
  mcHashKeyFunc mcHashKey;

  /* Find Server function. */
  mcServerFindFunc mcServerFind;

  /* In non-fatal, normal operating conditions, it is plausble (indeed
   * likely) that certain operations are going to fail, but do so
   * wihin the bounds of normal operating levels for memcache(7).
   * Prime examples include performing an increment operation on a key
   * that doesn't exist.  Given that such cases are to be considered
   * normal, it is less than desirable to fall back to the error
   * handling system in memcache(3).  The following errnum value acts
   * to mimic the global errno(2) value, but on a per-context
   * basis. */
  u_int32_t errnum;

  /* Internal.  Pointer to the last command that was written out: the
   * state of this pointer is not guaranteed and its use is internal
   * to memcache(3).  Only used when a server dies and we need to pass
   * around the server's last command or other data. */
  struct memcache_buf *_rbuf;
  struct memcache_buf *_wbuf;
  u_int32_t _last_hash;

  /* Pointer to the context's error context/information. */
  struct memcache_err_ctxt *ectxt;

  /* Determines what error levels are ignored.  See mc_err_filter_*()
   * for the interface to this struct member.  Manual manipulation is
   * prohibited. */
  u_int32_t MCM_ERR_MASK;
};

struct memcache_server {
  /* A generic pointer not used by memcache(3), but can be used by
   * calling programs. */
  void *misc;

  /* The hostname of the server. */
  char *hostname;

  /* Port number of the host we're connecting to. */
  char *port;

  /* The file descriptor for this server */
  int fd;

  /* The timeout for this server */
  struct timeval tv;

  /* Is this particular server active or not?
   *
   * 'd' == Down	Last request was unsuccessful
   * 'n' == No host	The hostname doesn't exist
   * 't' == Try		Haven't connected to it yet, will attempt
   * 'u' == Up		Has been connected to successfully
   */
  char active;

  /* A cached copy of the looked up host. */
  struct addrinfo *hostinfo;

  /* The number of addresses in the cached copy.  If there is more
   * than one per DNS entry (discouraged), we establish a connection
   * to them all. */
  u_int32_t num_addrs;

#ifdef HAVE_SELECT
  /* Reduces the amount of user time required when reading data. */
  fd_set fds;
#endif

  /* read(2) buffer */
  struct memcache_buf *rbuf;

  /* write(2) buffer */
  struct memcache_buf *wbuf;

  u_int32_t _last_hash;

  /* Internal.  A cursor for where we are in the buffer.  This changes
   * every time we examine a bit of data in our buffer. */
  size_t soff;

  /* Internal.  A pointer to the start of the current line in the
   * buffer. */
  size_t startoff;

  /* Misc list bits */
  TAILQ_ENTRY(memcache_server) entries;
};


struct memcache_server_stats {
  pid_t pid;
  time_t uptime;
  time_t time;
  char *version;
  struct timeval rusage_user;
  struct timeval rusage_system;
  u_int32_t curr_items;
  u_int64_t total_items;
  u_int64_t bytes;
  u_int32_t curr_connections;
  u_int64_t total_connections;
  u_int32_t connection_structures;
  u_int64_t cmd_get;
#ifdef SEAN_HACKS
  u_int64_t cmd_refresh;
#endif
  u_int64_t cmd_set;
  u_int64_t get_hits;
  u_int64_t get_misses;
#ifdef SEAN_HACKS
  u_int64_t refresh_hits;
  u_int64_t refresh_misses;
#endif
  u_int64_t bytes_read;
  u_int64_t bytes_written;
  u_int64_t limit_maxbytes;
};


/* struct memcache.  Any of the bits that are commented as "Internal"
 * should not be twiddled with, ever.  The misc member can be used by
 * applications and is *never* touched/accessed by memcache(3).  Its
 * primary purpose is to aid in embedding memcache(3) in other
 * programming languages. */
struct memcache {
  /* A generic pointer not used by memcache(3), but can be used by
   * calling programs. */
  void *misc;

  /* The default timeout for all servers. */
  struct timeval tv;

  /* The number of servers in **servers. */
  u_int32_t num_servers;

  /* An array of usable memcache_servers. */
  struct memcache_server **servers;

  /* The complete list of servers. */
  TAILQ_HEAD(memcache_server_list, memcache_server) server_list;
};


struct memcache_res {
  /* A generic pointer not used by memcache(3), but can be used by
   * calling programs. */
  void *misc;

  char *key;		/* key */
  size_t len;		/* length of key */
  u_int32_t hash;	/* hash of the key */
  size_t bytes;		/* length of val */
  void *val;		/* the value */

  /* If size is zero (default), the memory for val is automatically
   * allocated using mcMalloc(3).  If size is zero, _flags has its
   * MC_RES_FREE_ON_DELETE bit set.
   *
   * If size is non-zero, memcache(3) assumes that the caller has set
   * val to an available portion of memory that is size bytes long.
   * memcache(3) will only populate val with as many bytes as are
   * specified by size (ie, it will trim the value in order to fit
   * into val). If size is non-zero, _flags has its
   * MC_RES_NO_FREE_ON_DELETE bit set by default. */
  size_t size;

  TAILQ_ENTRY(memcache_res) entries;

  /* This is the client supplied flags.  Please note, this flags is
   * very different than _flags (_flags is an internal bit and
   * shouldn't be read/changed, etc). */
  u_int16_t flags;

  /* If _flags has 0x01 set, val will be free(3)'ed on when this
   * struct is cleaned up via mc_res_free() or the request is cleaned
   * up via mc_req_free().
   *
   * If _flags has is 0x02 set, val will not be free(3)'ed when this
   * response or its parent request are cleaned up.
   *
   * Note: Use mc_res_free_on_delete() to set the "free on delete"
   * bits. */
  char _flags;
};

struct memcache_req {
  /* A generic pointer not used by memcache(3), but can be used by
   * calling programs. */
  void *misc;

  TAILQ_HEAD(memcache_res_list, memcache_res) query;
  TAILQ_HEAD(memcache_res_cb_list, memcache_res_cb) cb;
  u_int16_t num_keys;
};


/* Call back interface bits.  memcache(3) offers a callback
 * mechanism wherein many get requests can be lumped together into a
 * single get request.  After a response has been read from the
 * server, the callbacks are executed.
 *
 * Example:
 *
 * static void my_callback_func(MCM_CALLBACK_SIG);
 * static void
 * my_callback_func(MCM_CALLBACK_FUNC) {
 *   struct my_struct *ptr = (struct my_struct *)MCM_CALLBACK_PTR;
 *   struct memcache_ctxt *ctxt = MCM_CALLBACK_CTXT;
 *   struct memcache_res *res = MCM_CALLBACK_RES;
 *   ...
 * }
 *
 * and callbacks are registered like:
 *
 * struct memcache_req *req = mc_req_new();
 * struct memcache_res *res = mc_req_add(req, key, key_len);
 * mc_res_register_fetch_cb(req, res, my_callback_func, NULL);
 *
 * or, if you want to pass an optional void * pointer:
 *
 * struct my_struct *ptr;
 * struct memcache_res_cb *cb = mc_res_register_fetch_cb(req, res, my_callback_func, ptr);
 *
 * Then call mc_get() like normal whenever and your callback will be
 * executed.  Ex:
 *
 * mc_get(req);
 */

typedef void (*mcResCallback)(MCM_CALLBACK_FUNC);
struct memcache_res_cb {
  void *misc;
  const struct memcache_ctxt *ctxt;
  struct memcache_req *req;
  struct memcache_res *res;
  mcResCallback cb;
  TAILQ_ENTRY(memcache_res_cb) entries;
};


/* Adds a given key to the cache */
int			 mc_add(struct memcache *mc,
				char *key, const size_t key_len,
				const void *val, const size_t bytes,
				const time_t expire, const u_int16_t flags);

/* Gets the value from memcache and allocates the data for the caller.
 * It is the caller's responsibility to free the returned value.
 * mc_get() is the preferred interface, however. */
void			*mc_aget(struct memcache *mc, char *key, const size_t len);

/* Gets the value from memcache and allocates the data for the caller.
 * It is the caller's responsibility to free the returned value.  If
 * retlen is non-null, the size of the response will be set upon
 * return, or zero in the event of a failure.  mc_get() is the
 * preferred interface, however. */
void			*mc_aget2(struct memcache *mc, char *key, const size_t len, size_t *retlen);

#ifdef SEAN_HACKS
void			*mc_alisten(struct memcache *mc, char *key, const size_t len);

/* Gets the value from memcache and allocates the data for the caller.
 * It is the caller's responsibility to free the returned value.
 * mc_refresh() is the preferred interface, however. */
void			*mc_arefresh(struct memcache *mc, char *key, const size_t len);
#endif

/* Decrements a given key */
u_int32_t		 mc_decr(struct memcache *mc, char *key, const size_t key_len, const u_int32_t val);

/* Deletes a given key */
int			 mc_delete(struct memcache *mc, char *key, const size_t key_len, const time_t hold);

/* Returns true if the given error level was added to the filter */
int			 mc_err_filter_add(const u_int32_t err_mask);

/* Returns true if the given error level was removed from the filter */
int			 mc_err_filter_del(const u_int32_t err_mask);

/* Returns the current filter mask.  This interface has a questionable
 * future and may get removed or have its result set change from
 * release to release. */
u_int32_t		 mc_err_filter_get(void);

/* Returns true if the specified error level is being filtered. */
int			 mc_err_filter_test(const u_int32_t err_lvl);

/* Executes the error handler for testing */
void			 mc_err_test(void);

/* Flushes all keys on a given server */
int			 mc_flush(struct memcache *mc, struct memcache_server *ms);

/* Flushes all keys on all servers */
int			 mc_flush_all(struct memcache *mc);

/* cleans up a memcache object. */
void			 mc_free(struct memcache *mc);

/* mc_get() is the preferred method of accessing memcache.  It accepts
 * multiple keys and lets a user (should they so choose) perform
 * memory caching to reduce the number of mcMalloc(3) calls makes. */
void			 mc_get(struct memcache *mc, struct memcache_req *req);

/* Generates a hash value from a given key */
u_int32_t		 mc_hash(const struct memcache *mc, const char *key, const size_t len);

/* Generates a hash value from a given key (depreciated, use mc_hash())*/
u_int32_t		 mc_hash_key(const char *key, const size_t len);

/* Increments a given key */
u_int32_t		 mc_incr(struct memcache *mc, char *key, const size_t key_len, const u_int32_t val);

/* Allocates a new memcache object */
struct memcache	*mc_new(void);

#ifdef SEAN_HACKS
void			 mc_listen(struct memcache *mc, struct memcache_req *req);

/* mc_refresh() is the preferred method of accessing memcache.  It
 * accepts multiple keys and lets a user (should they so choose)
 * perform memory caching to reduce the number of mcMalloc(3) calls
 * makes.  mc_refresh() differs from mc_get() in that mc_refresh
 * updates the expiration time to be now + whatever the expiration for
 * the item was set to.  Sessions should use this as a way of noting
 * sessions expiring. */
void			 mc_refresh(struct memcache *mc, struct memcache_req *req);
#endif

/* Returns the release date for the library */
u_int32_t		 mc_reldate(void);

/* Replaces a given key to the cache */
int			 mc_replace(struct memcache *mc,
				    char *key, const size_t key_len,
				    const void *val, const size_t bytes,
				    const time_t expire, const u_int16_t flags);

/* Safely adds a key to a given request (the key is mc_strdup()'ed).
   See mc_req_add_ref() to avoid the mc_strdup(): note the warning in
   mc_req_add_ref() if you decide to use the other function. */
struct memcache_res	*mc_req_add(struct memcache_req *req, char *key, const size_t len);

/* Adds a key to a given request. Stores a pointer to key.  key can
   not be modified or free(3)'ed until the passed in request and the
   returning struct memcache_res object is done being used. */
struct memcache_res	*mc_req_add_ref(struct memcache_req *req, char *key, const size_t len);

/* Cleans up a given request and its subsequent responses.  If _flags
 * has the MC_RES_FREE_ON_DELETE bit set (default), it will clean up
 * the value too.  If _flags has MC_RES_NO_FREE_ON_DELETE set,
 * however, it is the caller's responsibility to free the value.  To
 * prevent double free(3) errors, if a value is free(3)'ed before
 * mc_req_free() is called, set val to NULL. */
void			 mc_req_free(struct memcache_req *req);

/* Allocates a new memcache request object. */
struct memcache_req	*mc_req_new(void);

/* Returns 1 if there has been an attempt by the library to fill the
 * struct's bits from a memcache server.  Returns 0 if there has been
 * no attempt to fill this struct's data. */
int			 mc_res_attempted(const struct memcache_res *res);

/* Tells the response object to free the allocated memory when it gets
 * cleaned up or to let the caller manage the memory. */
void			 mc_res_free_on_delete(struct memcache_res *res, const int free_on_delete);

/* Returns 1 if the given memcache_res struct contains data that was
 * found and if there has been an attempt at filling the data.  Return
 * 0 if no value was found for the key *or* there has been no attempt
 * at filling the data. */
int			 mc_res_found(const struct memcache_res *res);

/* Cleans up an individual response object.  Normally this is not
 * necessary as a call to mc_req_free() will clean up its response
 * objects. */
void			 mc_res_free(struct memcache_req *req, struct memcache_res *res);

/* Registers a callback with the request so that upon completion of a
 * fetch request, a callback gets executed. */
int			 mc_res_register_fetch_cb(struct memcache_req *req, struct memcache_res *res,
						  mcResCallback cb, void *misc);

/* Marks a given server as available again.  Does not reconnect to the
 * server, however. */
int			 mc_server_activate(struct memcache *mc, struct memcache_server *ms);

/* Mark all servers as available.  Does not connect to any servers,
 * but marks them as available. */
int			 mc_server_activate_all(struct memcache *mc);

/* Disconnects from a given server and marks it as down. */
void			 mc_server_deactivate(struct memcache *mc, struct memcache_server *ms);

/* Disconnects from a given server */
void			 mc_server_disconnect(struct memcache_server *ms);

/* Disconnects from all servers (leaves their active flag alone). */
void			 mc_server_disconnect_all(const struct memcache *mc);

/* When given a hash value, this function returns the appropriate
 * server to connect to in order to find the key. */
struct memcache_server	*mc_server_find(struct memcache *mc, const u_int32_t hash);

/* Adds a server to the list of available servers.  By default,
 * servers are assumed to be available.  Return codes:
 *
 * 0:	success
 * -1:	Unable to allocate a new server instance
 * -2:	Unable to strdup hostname
 * -3:	Unable to strdup port
 * -4:	Unable to Unable to resolve the host, server deactivated, but added to list
 * -5:	Unable to realloc(3) the server list, server list unchanged */
int			 mc_server_add(struct memcache *mc, const char *hostname, const char *port);
int			 mc_server_add2(struct memcache *mc,
					const char *hostname, const size_t hostname_len,
					const char *port, const size_t port_len);
int			 mc_server_add3(struct memcache *mc, struct memcache_server *ms);

/* Like the above, except hostport can be in the format:
 * "hostname:port" or just "hostname".  Ex: "127.0.0.1:11211" */
int			 mc_server_add4(struct memcache *mc, mc_const char *hostport);

/* Same as mc_server_add4(), except with a length for hostport */
int			 mc_server_add5(struct memcache *mc, mc_const char *hostport, const size_t hostlen);

/* Free's the space from a struct memcache_server.  Use mc_free(3)
 * instead: only use this function if you really know what you're
 * doing. */
void			 mc_server_free(struct memcache_server *ms);

/* Creates a new server struct */
struct memcache_server	*mc_server_new(void);


/* Cleans up a given stat's object */
void			 mc_server_stats_free(struct memcache_server_stats *s);

/* Gets a stats object from the given server.  It is the caller's
 * responsibility to cleanup the resulting object via
 * mc_server_stats_free(). */
struct memcache_server_stats	*mc_server_stats(struct memcache *mc, struct memcache_server *ms);

/* Set the timeout on a per server basis */
int			 mc_server_timeout(struct memcache_server *ms, const int sec, const int usec);

/* Sets a given key */
int			 mc_set(struct memcache *mc,
				char *key, const size_t key_len,
				const void *val, const size_t bytes,
				const time_t expire, const u_int16_t flags);

/* Creates a stats object for all available servers and returns the
 * cumulative stats.  Per host-specific data is generally the same as
 * the last server queried. */
struct memcache_server_stats	*mc_stats(struct memcache *mc);

/* memcache(3)'s strdup */
char			*mc_strdup(const char *str);

/* memcache(3)'s strnchr(3) */
char			*mc_strnchr(mc_const char *str, const int c, const size_t len);

/* memcache(3)'s strndup: returns a dup of str up to len bytes long,
 * and pads the string with a null character (ie: len + 1). */
char			*mc_strndup(const char *str, const size_t len);


/* Sets the default timeout for new servers. */
void			 mc_timeout(struct memcache *mc, const int sec, const int usec);

/* Returns a numeric version of the library */
u_int32_t		 mc_vernum(void);

/* Returns a string version of the library */
const char		*mc_version(void);



/* BEGIN memory management API functions and support for multiple
 * memory contexts.  Most users of memcache(3) should use the
 * functions prototyped above.  The below functions should be used by
 * advanced developers seeking a tad bit more control over their app's
 * use of memcache(3), authors of language extensions, or developers
 * who need to write modules that exist inside of a single process
 * with multiple memory allocation routines (ex: Apache and PHP). */


/* The following two functions are used to setup additional memory
 * allocations for programs that use memcache(3), but are not using
 * the standard system memory management routines (ex: PostgreSQL,
 * Ruby, etc.) */
int	mcMemSetup(mcFreeFunc freeFunc, mcMallocFunc mallocFunc,
		   mcMallocFunc mallocAtomicFunc, mcReallocFunc reallocFunc);
int	mcMemGet(mcFreeFunc *freeFunc, mcMallocFunc *mallocFunc,
		 mcMallocFunc *mallocAtomicFunc, mcReallocFunc *reallocFunc);

/* Returns a pointer to the global context. */
/*inline*/ struct memcache_ctxt *mc_global_ctxt(void);

/* The next two functions are used to setup an error handler. */
int	mcErrSetup(mcErrFunc errFunc);
int	mcErrSetupCtxt(struct memcache_ctxt *ctxt, mcErrFunc errFunc);
int	mcErrGet(mcErrFunc *errFunc);


/* From here on out, the API assumes callers are providing a valid
 * memory context.  This allows multiple memory contexts to exist
 * inside the same process.  Very handy for PHP/Apache/APR authors, or
 * developers in similar situations.  For maximum portability and
 * embedability, use of the mcm_*() functions is *strongly*
 * encouraged. */

/* Creates a new memory context from scratch: should be sufficient for
 * most applications. */
struct memcache_ctxt	*mcMemNewCtxt(mcFreeFunc freeFunc, mcMallocFunc mallocFunc,
				      mcMallocFunc mallocAtomicFunc, mcReallocFunc reallocFunc);

/* Safely assigns the various function pointers to the passed in
 * memory context.  Only needed when an application needs to change
 * its memory allocation routines (not sure why this would ever need
 * to happen, to be honest). */
int                     mcMemSetupCtxt(struct memcache_ctxt *ctxt, mcFreeFunc freeFunc,
				       mcMallocFunc mallocFunc, mcMallocFunc mallocAtomicFunc,
				       mcReallocFunc reallocFunc);


/* Free's a given memcache context */
void			 mcMemFreeCtxt(struct memcache_ctxt *ctxt);

/* Functions from here to the bottom of the section behave identically
 * to the above functions, except they have one additional argument, a
 * struct memcache_ctxt pointer.  See above for documentation. */
int			 mcm_add(struct memcache_ctxt *ctxt, struct memcache *mc,
				 char *key, const size_t key_len,
				 const void *val, const size_t bytes,
				 const time_t expire, const u_int16_t flags);
void			*mcm_aget(struct memcache_ctxt *ctxt, struct memcache *mc,
				  char *key, const size_t len);
void			*mcm_aget2(struct memcache_ctxt *ctxt, struct memcache *mc,
				   char *key, const size_t len, size_t *retlen);
#ifdef SEAN_HACKS
void			*mcm_alisten(struct memcache_ctxt *ctxt, struct memcache *mc,
				     char *key, const size_t len);
void			*mcm_arefresh(struct memcache_ctxt *ctxt, struct memcache *mc,
				      char *key, const size_t len);
#endif
u_int32_t		 mcm_decr(struct memcache_ctxt *ctxt, struct memcache *mc,
				  char *key, const size_t key_len, const u_int32_t val);
int			 mcm_delete(struct memcache_ctxt *ctxt, struct memcache *mc,
				    char *key, const size_t key_len, const time_t hold);
void			 mcm_err(const struct memcache_ctxt *ctxt, const u_int32_t flags,
				 const char *funcname, const u_int32_t lineno, const u_int32_t errcode,
				 const char *msg, const u_int32_t msglen, const u_int32_t errlvl);
int			 mcm_err_filter_add(struct memcache_ctxt *ctxt, const u_int32_t err_mask);
int			 mcm_err_filter_del(struct memcache_ctxt *ctxt, const u_int32_t err_mask);
u_int32_t		 mcm_err_filter_get(const struct memcache_ctxt *ctxt);
int			 mcm_err_filter_test(const struct memcache_ctxt *ctxt, const u_int32_t err_lvl);
void			 mcm_err_test(const struct memcache_ctxt *ctxt);
int			 mcm_flush(struct memcache_ctxt *ctxt, struct memcache *mc,
				   struct memcache_server *ms);
int			 mcm_flush_all(struct memcache_ctxt *ctxt, struct memcache *mc);
void			 mcm_free(struct memcache_ctxt *ctxt, struct memcache *mc);
void			 mcm_get(struct memcache_ctxt *ctxt, struct memcache *mc, struct memcache_req *req);
u_int32_t		 mcm_hash(const struct memcache_ctxt *ctxt, const struct memcache *mc, const char *key, const size_t len);
u_int32_t		 mcm_hash_key(const struct memcache_ctxt *ctxt, const char *key, const size_t len);
u_int32_t		 mcm_incr(struct memcache_ctxt *ctxt, struct memcache *mc,
				  char *key, const size_t key_len, const u_int32_t val);
struct memcache		*mcm_new(struct memcache_ctxt *ctxt);
#ifdef SEAN_HACKS
void			 mcm_listen(struct memcache_ctxt *ctxt, struct memcache *mc,
				     struct memcache_req *req);
void			 mcm_refresh(struct memcache_ctxt *ctxt, struct memcache *mc,
				     struct memcache_req *req);
#endif
u_int32_t		 mcm_reldate(const struct memcache_ctxt *ctxt);
int			 mcm_replace(struct memcache_ctxt *ctxt, struct memcache *mc,
				     char *key, const size_t key_len,
				     const void *val, const size_t bytes,
				     const time_t expire, const u_int16_t flags);
struct memcache_res	*mcm_req_add(const struct memcache_ctxt *ctxt, struct memcache_req *req,
				     char *key, const size_t len);
struct memcache_res	*mcm_req_add_ref(const struct memcache_ctxt *ctxt, struct memcache_req *req,
					 char *key, const size_t len);
void			 mcm_req_free(const struct memcache_ctxt *ctxt, struct memcache_req *req);
struct memcache_req	*mcm_req_new(const struct memcache_ctxt *ctxt);
int			 mcm_res_attempted(const struct memcache_ctxt *ctxt, const struct memcache_res *res);
int			 mcm_res_found(const struct memcache_ctxt *ctxt, const struct memcache_res *res);
void			 mcm_res_free(const struct memcache_ctxt *ctxt, struct memcache_req *req,
				      struct memcache_res *res);
void			 mcm_res_free_on_delete(const struct memcache_ctxt *ctxt, struct memcache_res *res,
						const int free_on_delete);
int			 mcm_res_register_fetch_cb(struct memcache_ctxt *ctxt, struct memcache_req *req,
						   struct memcache_res *res, mcResCallback callback, void *misc);
int			 mcm_server_activate(const struct memcache_ctxt *ctxt, struct memcache *mc, struct memcache_server *ms);
int			 mcm_server_activate_all(const struct memcache_ctxt *ctxt, struct memcache *mc);
int			 mcm_server_add(struct memcache_ctxt *ctxt, struct memcache *mc,
					const char *hostname, const char *port);
int			 mcm_server_add2(struct memcache_ctxt *ctxt, struct memcache *mc,
					 const char *hostname, const size_t hostname_len,
					 const char *port, const size_t port_len);
int			 mcm_server_add3(struct memcache_ctxt *ctxt, struct memcache *mc,
					 struct memcache_server *ms);
int			 mcm_server_add4(struct memcache_ctxt *ctxt, struct memcache *mc,
					 mc_const char *hostport);
int			 mcm_server_add5(struct memcache_ctxt *ctxt, struct memcache *mc,
					 mc_const char *hostport, const size_t hostlen);
void			 mcm_server_deactivate(struct memcache_ctxt *ctxt, struct memcache *mc,
					       struct memcache_server *ms);
void			 mcm_server_disconnect(const struct memcache_ctxt *ctxt, struct memcache_server *ms);
void			 mcm_server_disconnect_all(const struct memcache_ctxt *ctxt, const struct memcache *mc);
struct memcache_server	*mcm_server_find(const struct memcache_ctxt *ctxt,
					 struct memcache *mc, const u_int32_t hash);
void			 mcm_server_free(struct memcache_ctxt *ctxt, struct memcache_server *ms);
struct memcache_server	*mcm_server_new(struct memcache_ctxt *ctxt);
void			 mcm_server_stats_free(const struct memcache_ctxt *ctxt, struct memcache_server_stats *s);
struct memcache_server_stats	*mcm_server_stats(struct memcache_ctxt *ctxt, struct memcache *mc, struct memcache_server *ms);
int			 mcm_server_timeout(const struct memcache_ctxt *ctxt, struct memcache_server *ms, const int sec, const int usec);
int			 mcm_set(struct memcache_ctxt *ctxt, struct memcache *mc,
				 char *key, const size_t key_len,
				 const void *val, const size_t bytes,
				 const time_t expire, const u_int16_t flags);
struct memcache_server_stats	*mcm_stats(struct memcache_ctxt *ctxt, struct memcache *mc);
char			*mcm_strdup(const struct memcache_ctxt *ctxt, const char *str);
char			*mcm_strnchr(const struct memcache_ctxt *ctxt, mc_const char *str, const int c, const size_t len);
char			*mcm_strndup(const struct memcache_ctxt *ctxt, const char *str, const size_t len);
char			*mcm_strnstr(const struct memcache_ctxt *ctxt, mc_const char *s, const char *find, size_t slen);
void			 mcm_timeout(const struct memcache_ctxt *ctxt, struct memcache *mc, const int sec, const int usec);
u_int32_t		 mcm_vernum(const struct memcache_ctxt *ctxt);
const char		*mcm_version(const struct memcache_ctxt *ctxt);
/* END memory management API functions */


/* BEGIN - Error handling convenience macros */
#define ERR_FLAG	0x1
#define WARN_FLAG	0x2
#define NO_ERRNO_FLAG	0x4
#define MCM_ERR(_code)			mcm_err(ctxt, ERR_FLAG, __FUNCTION__, __LINE__, _code, NULL, 0, 0)
#define MCM_ERR_MSG(_code, _msg)	mcm_err(ctxt, ERR_FLAG, __FUNCTION__, __LINE__, _code, _msg, (_msg != NULL ? strlen(_msg) : 0), 0)
#define MCM_ERR_MSG_LVL(_code, _msg, _lvl)	mcm_err(ctxt, ERR_FLAG, __FUNCTION__, __LINE__, _code, _msg, (_msg != NULL ? strlen(_msg) : 0), _lvl)
#define MCM_ERRX(_code)			mcm_err(ctxt, ERR_FLAG|NO_ERRNO_FLAG, __FUNCTION__, __LINE__, _code, NULL, 0, 0)
#define MCM_ERRX_MSG_LVL(_code, _msg, _lvl)	mcm_err(ctxt, ERR_FLAG|NO_ERRNO_FLAG, __FUNCTION__, __LINE__, _code, _msg, (_msg != NULL ? strlen(_msg) : 0), _lvl)
#define MCM_ERRX_MSG(_code, _msg)	mcm_err(ctxt, ERR_FLAG|NO_ERRNO_FLAG, __FUNCTION__, __LINE__, _code, _msg, (_msg != NULL ? strlen(_msg) : 0), 0)
#define MCM_WARNX(_code, _msg)		mcm_err(ctxt, WARN_FLAG|NO_ERRNO_FLAG, __FUNCTION__, __LINE__, _code, _msg, (_msg != NULL ? strlen(_msg) : 0), 0)
#define MCM_WARN_MSG(_code, _msg)	mcm_err(ctxt, WARN_FLAG, __FUNCTION__, __LINE__, _code, _msg, (_msg != NULL ? strlen(_msg) : 0), 0)
#define MCM_WARN_MSGLEN(_code, _m, _l)	mcm_err(ctxt, WARN_FLAG, __FUNCTION__, __LINE__, _code, _m, _l, 0)
#define MCM_WARN_MSG_LVL(_code, _msg, _lvl)	mcm_err(ctxt, WARN_FLAG, __FUNCTION__, __LINE__, _code, _msg, (_msg != NULL ? strlen(_msg) : 0), _lvl)
#define MCM_WARNX_MSG(_code, _msg)	mcm_err(ctxt, WARN_FLAG|NO_ERRNO_FLAG, __FUNCTION__, __LINE__, _code, _msg, (_msg != NULL ? strlen(_msg) : 0), 0)
#define MCM_WARNX_MSGLEN(_code, _m, _l)	mcm_err(ctxt, WARN_FLAG|NO_ERRNO_FLAG, __FUNCTION__, __LINE__, _code, _m, _l, 0)
#define MCM_RET_CODE(_v)		(ctxt->ectxt->retcode != 0 ? ctxt->ectxt->retcode : _v)
/* END - Error handling convenience macros */


/* APIs that should be implemented: */

/* Resets all DNS entries */
void mc_server_reset_all_dns(struct memcache *mc);

/* Resets only one host's DNS cache */
void mc_server_reset_dns(struct memcache *mc, const char *hostname, const int port);

#ifdef TCP_NODELAY
/* Enable/disable TCP_NODELAY */
void mc_nodelay_enable(struct memcache *mc, const int enable);

/* Enable/disable TCP_NODELAY for a given server */
void mc_server_nodelay_enable(struct memcache_server *ms, const int enable);
#endif

/* Set the number of seconds you're willing to wait in total for a
 * response. ??? */

#ifdef __cplusplus
}
#endif

#endif
