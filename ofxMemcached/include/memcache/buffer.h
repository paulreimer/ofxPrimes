/*
 * Copyright (c) 2005 Sean Chittenden <sean@chittenden.org>
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

#ifndef MC_BUFFER_H
#define MC_BUFFER_H

#include <sys/types.h>

#include <memcache.h>
#include <memcache/_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MCM_BUF_OFF_USED ((char)0x1)

#define mcm_buf_len_add(c,s,n) do { s->len += n; } while (0)
#define mcm_buf_len_set(c,s,n) do { s->len = n; } while (0)
#define mcm_buf_size(c,s) (s->size)
#define mcm_buf_val(c,s) (s->ptr)
#define mcm_buf_off_ptr(c,b) (&(mcm_buf_to_cstr(c, b))[b->off])

int	 mcm_buf_append(struct memcache_ctxt *, struct memcache_buf *, const char *, const u_int32_t);
int	 mcm_buf_append2(struct memcache_ctxt *, struct memcache_buf *, const char *);
int	 mcm_buf_append_buf(struct memcache_ctxt *, struct memcache_buf *, struct memcache_buf *);
int	 mcm_buf_append_char(struct memcache_ctxt *, struct memcache_buf *, char);
int	 mcm_buf_cmp(struct memcache_ctxt *, const struct memcache_buf *, const char *, const size_t);
int	 mcm_buf_cmp2(struct memcache_ctxt *, const struct memcache_buf *, const char *);
int	 mcm_buf_cmp_buf(struct memcache_ctxt *, const struct memcache_buf *, const struct memcache_buf *);
int	 mcm_buf_compact(struct memcache_ctxt *, struct memcache_buf *);
struct memcache_buf	*mcm_buf_copy(struct memcache_ctxt *, struct memcache_buf *);
void	 mcm_buf_eat_line(struct memcache_ctxt *, struct memcache_buf *);
int	 mcm_buf_end(struct memcache_ctxt *, const struct memcache_buf *, const char *, const size_t);
int	 mcm_buf_end2(struct memcache_ctxt *, const struct memcache_buf *, const char *);
int	 mcm_buf_end_buf(struct memcache_ctxt *, const struct memcache_buf *, struct memcache_buf *);
char	*mcm_buf_find_buf(struct memcache_ctxt *, const struct memcache_buf *, const struct memcache_buf *);
char	*mcm_buf_find(struct memcache_ctxt *, const struct memcache_buf *, const char *);
char	*mcm_buf_find2(struct memcache_ctxt *, const struct memcache_buf *, const char *, const size_t);
struct memcache_buf	*mcm_buf_find_replace(struct memcache_ctxt *, struct memcache_buf *,
						   struct memcache_buf *, struct memcache_buf *);
struct memcache_buf	*mcm_buf_find_replace2(struct memcache_ctxt *, struct memcache_buf *,
						    const char *, struct memcache_buf *);
int	 mcm_buf_free(struct memcache_ctxt *, struct memcache_buf **);
/*inline*/ u_int32_t	 mcm_buf_len(const struct memcache_ctxt *, const struct memcache_buf *);
struct memcache_buf	*mcm_buf_new(struct memcache_ctxt *);
struct memcache_buf	*mcm_buf_new2(struct memcache_ctxt *, const char *, const u_int32_t);
struct memcache_buf	*mcm_buf_new3(struct memcache_ctxt *, const char *);
size_t	 mcm_buf_read(struct memcache_ctxt *, struct memcache_buf *, int);
int	 mcm_buf_realloc(struct memcache_ctxt *, struct memcache_buf *, const u_int32_t);
/*inline*/ size_t	 mcm_buf_remain(const struct memcache_ctxt *, const struct memcache_buf *);
/*inline*/ size_t	 mcm_buf_remain_off(const struct memcache_ctxt *, const struct memcache_buf *);
int	 mcm_buf_replace(struct memcache_ctxt *, struct memcache_buf *, const char *, const u_int32_t);
int	 mcm_buf_replace2(struct memcache_ctxt *, struct memcache_buf *, const char *);
int	 mcm_buf_replace_buf(struct memcache_ctxt *, struct memcache_buf *, struct memcache_buf *);
int	 mcm_buf_reset(struct memcache_ctxt *, struct memcache_buf *);
int	 mcm_buf_resize(struct memcache_ctxt *, struct memcache_buf *, const u_int32_t);
char	*mcm_buf_tail(struct memcache_ctxt *, struct memcache_buf *);
char	*mcm_buf_to_cstr(struct memcache_ctxt *, struct memcache_buf *);
struct memcache_buf	*mcm_buf_to_upper(struct memcache_ctxt *, struct memcache_buf *);

#ifdef __cplusplus
}
#endif

#endif
