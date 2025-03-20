#include "shs_binary.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int shs_bs_write(shs_binary_stream_t *bs, const void *data, size_t len) {
  if (!bs || !data) {
    return EINVAL;
  }

  if (bs->count + len > bs->capacity) {
    int old_capacity = bs->capacity;
    int new_capacity = bs->count + len + 1024;
    char *tmp = (char *)realloc(bs->buf, new_capacity);

    if (!tmp) {
      return ENOMEM;
    }

    bs->buf = tmp;
    bs->capacity = new_capacity;
  }

  memcpy(bs->buf + bs->count, data, len);
  bs->count += len;

  return 0;
}

void shs_bs_release(shs_binary_stream_t *bs) {
  if (!bs) {
    return;
  }

  free(bs->buf);
  bs->buf = NULL;
  bs->capacity = 0;
  bs->count = 0;
}

int shs_bs_init(shs_binary_stream_t *bs, size_t capacity) {
  if (!bs) {
    return EINVAL;
  }

  int rc = 0;

  bs->buf = (char *)malloc(capacity);

  if (!bs->buf) {
    return ENOMEM;
  }

  bs->capacity = capacity;
  bs->count = 0;

  return rc;
}
