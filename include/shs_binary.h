#ifndef _SHS_BINARY_H
#define _SHS_BINARY_H
#include <stdio.h>

struct shs_binary_stream {
  char *buf;
  size_t capacity;
  size_t count;
};
typedef struct shs_binary_stream shs_binary_stream_t;

int shs_bs_init(shs_binary_stream_t *bs, size_t capacity);
void shs_bs_release(shs_binary_stream_t *bs);
int shs_bs_write(shs_binary_stream_t *bs, const void *buf, size_t count);

#endif
