#ifndef SHS_INNER_FILE_TABLE_H
#define SHS_INNER_FILE_TABLE_H

#include "hash_table.h"
#include <stdint.h>
#include <stdio.h>

typedef struct file_info {
  uint32_t fd;
  size_t offset_to_shs;
  size_t offset_in_file;
  size_t file_size;
} file_info_t;

DECLARE_HASH_TABLE(shs_inner_file_table, file_info_t *);

#endif
