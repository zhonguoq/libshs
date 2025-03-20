#ifndef _SHS_PACK_H_
#define _SHS_PACK_H_
#include "file_table.h"
#include "q_string.h"
#include "shs_binary.h"

struct shs_file_packer {
  q_string_t root_path;
  q_string_t output_path;

  file_table_t file_table; /* !< file entry table */
  shs_binary_stream_t bs; /* !< binary stream which condense all file content */

  int (*pack)(struct shs_file_packer *packer);
};

typedef struct shs_file_packer shs_file_packer_t;

int shs_file_packer_init(shs_file_packer_t *packer, q_string_t root_path,
                         q_string_t output_path);
void shs_file_packer_release(shs_file_packer_t *packer);

#endif // !_SHS_PACK_H_
