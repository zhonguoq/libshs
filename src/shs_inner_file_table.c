#include "shs_inner_file_table.h"
#include "hash_table_def.h"
DEFINE_HASH_TABLE(shs_inner_file_table)

#include "hash_table_impl.h"

static inline int ht_match(const void *key, size_t len, file_info_t *info) {
  return len == (size_t)sizeof(uint32_t) && memcmp(key, &info->fd, len) == 0;
}

static inline const void *ht_key(file_info_t *info) { return &info->fd; }

static inline size_t ht_key_len(file_info_t *info) {
  return (size_t)sizeof(uint32_t);
}
