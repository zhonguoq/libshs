#ifndef _FILE_TABLE_H_
#define _FILE_TABLE_H_
#include "hash_table.h"
#include "q_string.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct file_name file_name_t;

struct file_entry {
  q_string_t s;
  uint32_t offset;
  size_t size;
};

typedef struct file_entry file_entry_t;

DECLARE_HASH_TABLE(file_table, file_entry_t *);

#endif // !_FILE_TABLE_H_
