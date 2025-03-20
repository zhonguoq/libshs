#ifndef _SHS_FILE_H
#define _SHS_FILE_H
#include "shs_binary.h"
#include <stdint.h>

#define SHS_HEADER_SIZE (128)
#define SHS_MAGIC (0x4945465A)
#define SHS_VERSION (0x00000001)
typedef struct shs_header_info {
  uint32_t magic;
  uint32_t version;
  uint32_t file_count;
  uint32_t file_table_offset;
  uint32_t file_table_size;
  uint32_t file_content_offset;
  uint32_t file_content_size;
} shs_header_info_t;

int shs_header_serialize(shs_header_info_t *header, shs_binary_stream_t *bs);
int shs_header_deserialize(shs_header_info_t *header, shs_binary_stream_t *bs);
int shs_header_print(shs_header_info_t *header);

#endif
