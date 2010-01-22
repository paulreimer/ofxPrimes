#ifndef __MC_BUFFER_H__
#define __MC_BUFFER_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct memcache_buf {
  /* A null terminated buffer. */
  char *ptr;

  /* Buffer flags. */
  char flags;

  /* Length of the buffer.  The length never includes the terminating
   * null zero. */
  size_t len;

  /* Allocated memory dedicated for the buffer.  The length is always
   * at least one byte smaller than the size.  The size includes the
   * null terminating zero. */
  size_t size;

  /* A generic offset that can be internally used by this structure's
   * consumer.  Used primarily for read(2) and write(2) offsets. */
  size_t off;

  /* Recycle buffers where possible: used to keep a linked list of
   * rotating buffers. */
  TAILQ_ENTRY(memcache_buf) next;
};

#ifdef __cplusplus
}
#endif

#endif /* __MC_BUFFER_H__ */
