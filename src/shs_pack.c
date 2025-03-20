#include "shs_pack.h"
#include "file_table.h"
#include "hash_table.h"
#include "shs_builder.h"
#include "shs_file.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <flatcc/support/hexdump.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#undef ns
#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(fb_file_table, x)
/*
 * Convenience macro to deal with long namespace names,
 * and to make code reusable with other namespaces.
 *
 * Note: we could also use
 *
 *     #define ns(x) MyGame_Example_ ## x
 *
 * but it wouldn't doesn't handled nested ns calls.
 *
 * For historic reason some of this test does not use the ns macro
 * and some avoid nesting ns calls by placing parenthesis differently
 * although this isn't required with this wrapper macro.
 */

#undef nsf
#define nsf(x) FLATBUFFERS_WRAP_NAMESPACE(Fantasy, x)

/*
 * Wrap the common namespace (flatbuffers_). Many operations in the
 * common namespace such as `flatbuffers_string_create` are also mapped
 * to member fields such as `MyGame_Example_Monster_name_create` and
 * this macro provides a consistent interface to namespaces with
 * `nsc(string_create)` similar to `ns(Monster_name_create)`.
 */
#undef nsc
#define nsc(x) FLATBUFFERS_WRAP_NAMESPACE(flatbuffers, x)

/* A helper to simplify creating buffers vectors from C-arrays. */
#define c_vec_len(V) (sizeof(V) / sizeof((V)[0]))

#define DEFAULT_FILE_TABLE_SIZE 100

void FOREACH_FILE_IN_DIR(const char *dirPath, const char *relativeDirPath,
                         void (*processer)(const char *, const char *,
                                           void *context),
                         void *context) {

  char *currentPath = (char *)malloc(PATH_MAX * sizeof(char));
  struct dirent *dp;
  DIR *dir = opendir(dirPath);
  if (!dir) {
    free(currentPath);
    return;
  }
  while ((dp = readdir(dir)) != NULL) {
    if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
      char relativePath[PATH_MAX];
      sprintf(currentPath, "%s/%s", dirPath, dp->d_name);
      sprintf(relativePath, "%s/%s", relativeDirPath, dp->d_name);
      if (dp->d_type == DT_REG) {
        processer(currentPath, relativePath, context);
      }
      if (dp->d_type == DT_DIR) {
        FOREACH_FILE_IN_DIR(currentPath, relativePath, processer, context);
      }
    }
  }
  closedir(dir);
  free(currentPath);
}

void file_process(const char *path, const char *relative_path, void *context) {
  printf("Processing file: %s : %s\n", path, relative_path);
  shs_file_packer_t *packer = (shs_file_packer_t *)context;
  file_table_t *T = &packer->file_table;
  file_entry_t *entry = (file_entry_t *)malloc(sizeof(file_entry_t));
  FILE *fp = NULL;
  char *read_buf = NULL;
  int rc = 0;

  entry->s = q_string_new(relative_path, strlen(relative_path));
  entry->offset = 0;

  fp = fopen(path, "r");
  if (fp == NULL) {
    return;
  }
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  printf("File size: %ld\n", size);

  read_buf = (char *)malloc(size);

  if (!read_buf) {
    rc = ENOMEM;
    goto exit;
  }

  fseek(fp, 0, SEEK_SET);

  size_t read_size = fread(read_buf, 1, size, fp);

  if (read_size != size) {
    rc = EIO;
    goto exit;
  }

  rc = shs_bs_write(&packer->bs, read_buf, size);

  if (rc != 0) {
    goto exit;
  }

  entry->offset = packer->bs.count - size;
  entry->size = size;

  file_table_insert_item(T, entry, ht_unique);

exit:
  if (read_buf) {
    free(read_buf);
  }
  if (fp) {
    fclose(fp);
  }
}

void fb_visitor(void *context, file_entry_t *entry) {
  flatcc_builder_t *B = (flatcc_builder_t *)context;

  ns(fileEntry_start(B));
  ns(fileEntry_offset_add(B, entry->offset));
  ns(fileEntry_size_add(B, entry->size));
  ns(fileEntry_name_create_str(B, entry->s.s));
  ns(fileEntry_ref_t) ent = ns(fileEntry_end(B));
  ns(fileEntry_vec_push(B, ent));
}

static int do_pack(shs_file_packer_t *packer) {
  if (!packer) {
    return EINVAL;
  }

  int rc = 0;
  char *root_path = packer->root_path.s;
  char *output_path = packer->output_path.s;
  file_table_t *T = &packer->file_table;
  size_t final_bs_size = 0;
  shs_binary_stream_t final_bs;
  FILE *fp = NULL;
  char *out_buf = NULL;
  size_t out_size = 0;
  shs_header_info_t header;
  shs_binary_stream_t header_bs;

  // recersively walk the root path print all absolute paths
  FOREACH_FILE_IN_DIR(root_path, "", file_process, packer);

  printf("total file count: %ld\n", T->buckets);
  flatcc_builder_t builder;
  flatcc_builder_t *B = &builder;
  flatcc_builder_init(&builder);

  ns(BinaryFileTable_start_as_root(B));
  ns(Any_vec_start(B));

  for (int i = 0; i < T->buckets; i++) {
    file_entry_t **TT = T->table;
    file_entry_t *entry = TT[i];
    if (entry && entry != 2) {

      ns(fileEntry_start(B));
      ns(fileEntry_offset_add(B, entry->offset));
      ns(fileEntry_size_add(B, entry->size));
      ns(fileEntry_name_create_str(B, entry->s.s));
      ns(fileEntry_ref_t) ent = ns(fileEntry_end(B));
      ns(Any_vec_push(B, ns(Any_as_fileEntry(ent))));
    } else {
      ns(thombStone_ref_t) thomb = ns(thombStone_create(B, (uint32_t)(entry)));

      ns(Any_vec_push(B, ns(Any_as_thombStone(thomb))));
    }
  }

  // file_table_visit(T, fb_visitor, B);

  ns(Any_union_vec_ref_t) filetable_vec = ns(Any_vec_end(B));

  ns(BinaryFileTable_fileTable_add(B, filetable_vec));

  ns(BinaryFileTable_end_as_root(B));

  out_buf = flatcc_builder_finalize_buffer(B, &out_size);

  if (!out_buf) {
    fprintf(stderr, "flatcc_builder_finalize_buffer failed\n");
    rc = ENOMEM;
    goto exit;
  }

  hexdump("binary file table", out_buf, out_size, stdout);

  flatcc_builder_clear(B);
  // ----------------- construct header info -------------------
  header.magic = SHS_MAGIC;
  header.version = SHS_VERSION;
  header.file_count = T->count;
  header.file_table_offset = 128; // 头文件结束后紧紧跟着file table
  header.file_table_size = out_size;
  header.file_content_offset =
      128 + out_size; // file table结束后紧跟着file content
  header.file_content_size = packer->bs.count;

  rc = shs_bs_init(&header_bs, SHS_HEADER_SIZE);
  if (rc != 0) {
    fprintf(stderr, "shs_bs_init failed\n");
    rc = ENOMEM;
    goto exit;
  }

  rc = shs_header_serialize(&header, &header_bs);

  if (rc != 0) {
    fprintf(stderr, "shs_header_serialize failed\n");
    rc = EIO;
    goto exit;
  }

  final_bs_size = header_bs.count + out_size + packer->bs.count;

  rc = shs_bs_init(&final_bs, final_bs_size);

  if (rc != 0) {
    fprintf(stderr, "shs_bs_init failed\n");
    rc = ENOMEM;
    goto exit;
  }

  rc = shs_bs_write(&final_bs, header_bs.buf, header_bs.count);

  if (rc != 0) {
    fprintf(stderr, "shs_bs_write header failed\n");
    rc = EIO;
    goto exit;
  }

  rc = shs_bs_write(&final_bs, out_buf, out_size);

  if (rc != 0) {
    fprintf(stderr, "shs_bs_write binary table failed\n");
    rc = EIO;
    goto exit;
  }

  rc = shs_bs_write(&final_bs, packer->bs.buf, packer->bs.count);

  if (rc != 0) {
    fprintf(stderr, "shs_bs_write file content failed\n");
    rc = EIO;
    goto exit;
  }

  // ----------------- save buffer to file -------------------

  fp = fopen(output_path, "w");

  if (!fp) {
    fprintf(stderr, "fopen failed\n");
    rc = EIO;
    goto exit;
  }

  size_t write_size = fwrite(final_bs.buf, 1, final_bs.count, fp);

  if (write_size != final_bs.count) {
    fprintf(stderr, "fwrite failed\n");
    rc = EIO;
    goto exit;
  }

exit:

  fclose(fp);
  // ----------------- save buffer to file -------------------
  free(out_buf);

  shs_bs_release(&header_bs);
  shs_bs_release(&final_bs);

  return 0;
}

int shs_file_packer_init(shs_file_packer_t *packer, q_string_t root_path,
                         q_string_t output_path) {
  int rc = 0;
  packer->root_path = root_path;
  packer->output_path = output_path;

  rc = file_table_init(&packer->file_table, DEFAULT_FILE_TABLE_SIZE);

  if (rc != 0) {
    fprintf(stderr, "file_table_init failed\n");
    return rc;
  }

  rc = shs_bs_init(&packer->bs, 1024);

  if (rc != 0) {
    fprintf(stderr, "shs_bs_init failed\n");
    return rc;
  }

  packer->pack = do_pack;
  return 0;
}

void file_table_deleter(void *context, file_entry_t *entry) {
  printf("Visitor: %s %d\n", entry->s.s, entry->offset);
  file_table_remove_item((file_table_t *)context, entry);
  q_string_free(entry->s);
  free(entry);
}

void shs_file_packer_release(shs_file_packer_t *packer) {
  if (!packer) {
    return;
  }

  file_table_t *T = &packer->file_table;

  q_string_free(packer->root_path);
  q_string_free(packer->output_path);
  shs_bs_release(&packer->bs);

  file_table_visit(T, file_table_deleter, T);

  return;
}
