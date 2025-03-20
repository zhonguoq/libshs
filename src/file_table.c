#include "file_table.h"
#include "hash_table_def.h"
DEFINE_HASH_TABLE(file_table)

#include "hash_table_impl.h"

static inline int ht_match(const void *key, size_t len, file_entry_t *name) {
  return len == (size_t)name->s.len && memcmp(key, name->s.s, len) == 0;
}

static inline const void *ht_key(file_entry_t *name) { return name->s.s; }

static inline size_t ht_key_len(file_entry_t *name) {
  return (size_t)name->s.len;
}
