#ifndef _SHS_ACCESSOR_H_
#define _SHS_ACCESSOR_H_
#include "q_string.h"
#include "shs_file.h"
#include "shs_inner_file_table.h"
#include <stdio.h>

#define DEFAULT_FILE_TABLE_SIZE (100)
#define MAX_OPEN_FILE_INDEX (65535)

// define error code
#define SHS_ACCESSOR_OK (0)
#define SHS_FILE_OPENED (-89999)
#define SHS_FILE_NOT_FOUND (-89998)
#define SHS_NO_BINARY_FILE_TABLE (-89997)

typedef struct shs_accessor {
  q_string_t file_path;
  FILE *file;

  char *binary_table;
  shs_header_info_t header;

  shs_inner_file_table_t file_table;
  size_t open_file_index;

  int (*test)(struct shs_accessor *accessor, const char *file_path);
  int (*open)(struct shs_accessor *accessor, const char *file_path);
  int (*lseek)(struct shs_accessor *accessor, int fd, long int off, int whence);
  int (*read)(struct shs_accessor *accessor, int fd, size_t size, char *stream);
  int (*close)(struct shs_accessor *accessor, int fd);
  int (*stat)(struct shs_accessor *accessor, int fd, file_info_t *info);
} shs_accessor_t;

int shs_accessor_init(shs_accessor_t *accessor, q_string_t file_path);

void shs_accessor_release(shs_accessor_t *accessor);

#endif // !_SHS_ACCESSOR_H_
