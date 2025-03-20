#include "shs_file.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int shs_header_deserialize(shs_header_info_t *header, shs_binary_stream_t *bs) {
  if (!header || !bs) {
    return EINVAL;
  }

  int rc = 0;
  char header_buf[SHS_HEADER_SIZE] = {0};
  size_t index = 0;
  char *p = header_buf;

  if (bs->count > SHS_HEADER_SIZE) {
    return EINVAL;
  }

  memcpy(header_buf, bs->buf, bs->count);

  header->magic = *(uint32_t *)(p + index);
  index += sizeof(uint32_t);

  header->version = *(uint32_t *)(p + index);
  index += sizeof(uint32_t);

  header->file_count = *(uint32_t *)(p + index);
  index += sizeof(uint32_t);

  header->file_table_offset = *(uint32_t *)(p + index);
  index += sizeof(uint32_t);

  header->file_table_size = *(uint32_t *)(p + index);
  index += sizeof(uint32_t);

  header->file_content_offset = *(uint32_t *)(p + index);
  index += sizeof(uint32_t);

  header->file_content_size = *(uint32_t *)(p + index);
  index += sizeof(uint32_t);

  return 0;
}

int shs_header_serialize(shs_header_info_t *header, shs_binary_stream_t *bs) {
  if (!header || !bs) {
    return EINVAL;
  }

  int rc = 0;
  char header_buf[SHS_HEADER_SIZE] = {0};
  size_t index = 0;
  char *p = header_buf;

  *(uint32_t *)(p + index) = header->magic;
  index += sizeof(uint32_t);

  *(uint32_t *)(p + index) = header->version;
  index += sizeof(uint32_t);

  *(uint32_t *)(p + index) = header->file_count;
  index += sizeof(uint32_t);

  *(uint32_t *)(p + index) = header->file_table_offset;
  index += sizeof(uint32_t);

  *(uint32_t *)(p + index) = header->file_table_size;
  index += sizeof(uint32_t);

  *(uint32_t *)(p + index) = header->file_content_offset;
  index += sizeof(uint32_t);

  *(uint32_t *)(p + index) = header->file_content_size;
  index += sizeof(uint32_t);

  rc = shs_bs_write(bs, header_buf, SHS_HEADER_SIZE);

  if (rc != 0) {
    return rc;
  }

  return 0;
}

int shs_header_print(shs_header_info_t *header) {
  if (!header) {
    return EINVAL;
  }

  printf("magic: %x\n", header->magic);
  printf("version: %x\n", header->version);
  printf("file_count: %x\n", header->file_count);
  printf("file_table_offset: %x\n", header->file_table_offset);
  printf("file_table_size: %x\n", header->file_table_size);
  printf("file_content_offset: %x\n", header->file_content_offset);
  printf("file_content_size: %x\n", header->file_content_size);

  return 0;
}
