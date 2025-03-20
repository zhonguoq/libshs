#include "shs_accessor.h"
#include "hash_table_def.h"
#include "shs_inner_file_table.h"
#include "shs_reader.h"
#include <errno.h>
#include <stdio.h>

#ifndef HT_PROBE
#ifdef HT_PROBE_QUADRATIC
#define HT_PROBE(k, i, N) ((k + (i + i * i) / 2) & N)
#else
#define HT_PROBE(k, i, N) ((k + i) & N)
#endif
#endif

static inline int ht_match(const void *key, size_t len, const void *src,
                           size_t src_len) {
  return len == src_len && memcmp(key, src, len) == 0;
}

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

static int get_next_fd(shs_accessor_t *self) {
  uint32_t fd = 0;

  while (fd < MAX_OPEN_FILE_INDEX) {
    if (!shs_inner_file_table_find(&self->file_table, &fd, sizeof(uint32_t))) {
      break;
    }
    ++fd;
  }

  if (fd == MAX_OPEN_FILE_INDEX) {
    return -1;
  }

  return fd;
}

static int shs_lseek(shs_accessor_t *self, int fd, long int off, int whence) {
  file_info_t *entry =
      shs_inner_file_table_find(&self->file_table, &fd, sizeof(uint32_t));

  if (entry == NULL) {
    return SHS_FILE_NOT_FOUND;
  }

  switch (whence) {
  case SEEK_SET:
    break;
  case SEEK_CUR:
    off = off + entry->offset_in_file;
    break;
  case SEEK_END:
    off = off + entry->file_size;
    break;
  }

  if (off < 0)
    return -EINVAL;

  if (off > entry->file_size)
    off = entry->file_size;

  entry->offset_in_file = off;

  return off;
}

static int shs_read(shs_accessor_t *self, int fd, size_t size, char *stream) {
  size_t read_size = 0;
  file_info_t *entry = NULL;

  if (!stream) {
    return -EINVAL;
  }

  entry = shs_inner_file_table_find(&self->file_table, &fd, sizeof(uint32_t));

  if (!entry) {
    return SHS_FILE_NOT_FOUND;
  }

  size_t remain_size = entry->file_size - entry->offset_in_file;

  read_size = size > remain_size ? remain_size : size;

  fseek(self->file, entry->offset_to_shs + entry->offset_in_file, SEEK_SET);

  size_t read = fread(stream, 1, read_size, self->file);

  if (read != read_size) {
    return -EIO;
  }

  entry->offset_in_file += read_size;

  return read_size;
}

static int shs_stat_file(shs_accessor_t *self, int fd, file_info_t *info) {
  file_info_t *entry =
      shs_inner_file_table_find(&self->file_table, &fd, sizeof(uint32_t));
  if (entry == NULL) {
    return SHS_FILE_NOT_FOUND;
  }

  memcpy(info, entry, sizeof(file_info_t));

  return 0;
}

static int shs_close_file(shs_accessor_t *self, int fd) {
  file_info_t *entry =
      shs_inner_file_table_remove(&self->file_table, &fd, sizeof(uint32_t));

  if (entry != NULL) {
    free(entry);
  }

  return 0;
}

static int shs_find_file(shs_accessor_t *self, const char *file_name) {
  if (!self || !file_name) {
    return -EINVAL;
  }

  size_t N, i, j, k;
  char *out_buf = self->binary_table;
  int rc = 0;

  ns(BinaryFileTable_table_t) table = ns(BinaryFileTable_as_root(out_buf));
  ns(Any_vec_t) anyvec_type;
  ns(Any_union_vec_t) anyvec_union;
  ns(Any_union_t) anyelem;
  flatbuffers_generic_vec_t anyvec;
  size_t n;

  if (!ns(BinaryFileTable_fileTable_is_present(table))) {
    return SHS_NO_BINARY_FILE_TABLE;
  }

  anyvec_type = ns(BinaryFileTable_fileTable_type(table));
  anyvec = ns(BinaryFileTable_fileTable(table));

  n = ns(Any_vec_len(anyvec_type));

  N = n - 1;
  k = HT_HASH_FUNCTION(file_name, strlen(file_name));
  i = 0;
  j = HT_PROBE(k, i, N);

  for (;;) {
    if (ns(Any_vec_at(anyvec_type, j)) == ns(Any_fileEntry)) {
      ns(fileEntry_table_t) entry = flatbuffers_generic_vec_at(anyvec, j);
      const char *entry_name = ns(fileEntry_name(entry));
      if (ht_match(file_name, strlen(file_name), entry_name,
                   strlen(entry_name))) {
        // ns(fileEntry_offset(entry)),
        //        ns(fileEntry_size(entry)), ns(fileEntry_name(entry)));

        file_info_t *info = (file_info_t *)malloc(sizeof(file_info_t));

        if (!info) {
          rc = -ENOMEM;
          goto exit;
        }

        uint32_t fd = get_next_fd(self);

        if (fd < 0) {
          // File table is full
          rc = fd;
          goto exit;
        }

        info->fd = fd;
        info->file_size = ns(fileEntry_size(entry));
        info->offset_to_shs =
            ns(fileEntry_offset(entry)) + self->header.file_content_offset;
        info->offset_in_file = 0;

        shs_inner_file_table_insert_item(&self->file_table, info, ht_unique);

        rc = fd;

        break;
      } else {
        ++i;
        j = HT_PROBE(k, i, N);
      }
    } else if (ns(Any_vec_at(anyvec_type, j)) == ns(Any_thombStone)) {
      ns(thombStone_table_t) entry = flatbuffers_generic_vec_at(anyvec, j);
      if (ns(thombStone_value(entry)) != 2) {
        // ns(thombStone_value(entry)));
        rc = -SHS_FILE_NOT_FOUND;
        break;
      } else {
        ++i;
        j = HT_PROBE(k, i, N);
      }
    }
  }

exit:
  return rc;
}

static int find_file_info(shs_accessor_t *self, const char *file_name) {
  if (!self || !file_name) {
    return EINVAL;
  }

  size_t N, i, j, k;
  char *out_buf = self->binary_table;

  ns(BinaryFileTable_table_t) table = ns(BinaryFileTable_as_root(out_buf));
  ns(Any_vec_t) anyvec_type;
  ns(Any_union_vec_t) anyvec_union;
  ns(Any_union_t) anyelem;
  flatbuffers_generic_vec_t anyvec;
  size_t n;

  if (!ns(BinaryFileTable_fileTable_is_present(table))) {
    return SHS_NO_BINARY_FILE_TABLE;
  }

  anyvec_type = ns(BinaryFileTable_fileTable_type(table));
  anyvec = ns(BinaryFileTable_fileTable(table));

  n = ns(Any_vec_len(anyvec_type));

  N = n - 1;
  k = HT_HASH_FUNCTION(file_name, strlen(file_name));
  i = 0;
  j = HT_PROBE(k, i, N);

  for (;;) {
    if (ns(Any_vec_at(anyvec_type, j)) == ns(Any_fileEntry)) {
      ns(fileEntry_table_t) entry = flatbuffers_generic_vec_at(anyvec, j);
      const char *entry_name = ns(fileEntry_name(entry));
      if (ht_match(file_name, strlen(file_name), entry_name,
                   strlen(entry_name))) {
        // ns(fileEntry_offset(entry)),
        //        ns(fileEntry_size(entry)), ns(fileEntry_name(entry)));

        size_t offset =
            ns(fileEntry_offset(entry)) + self->header.file_content_offset;
        size_t size = ns(fileEntry_size(entry));

        fseek(self->file, offset, SEEK_SET);

        char *buf = (char *)malloc(size + 1);

        if (!buf) {
          return ENOMEM;
        }

        memset(buf, 0, size + 1);

        size_t read_size = fread(buf, 1, size, self->file);

        {
          FILE *fp = fopen("test.txt", "wb");

          if (!fp) {
            return errno;
          }

          fwrite(buf, 1, size, fp);

          fclose(fp);
        }

        break;
      } else {
        ++i;
        j = HT_PROBE(k, i, N);
      }
    } else if (ns(Any_vec_at(anyvec_type, j)) == ns(Any_thombStone)) {
      ns(thombStone_table_t) entry = flatbuffers_generic_vec_at(anyvec, j);
      if (ns(thombStone_value(entry)) != 2) {
        // ns(thombStone_value(entry)));
        break;
      } else {
        ++i;
        j = HT_PROBE(k, i, N);
      }
    }
  }

  return 0;
}

void ssh_accessor_release(shs_accessor_t *self);

int shs_accessor_init(shs_accessor_t *self, q_string_t file_path) {
  if (!self) {
    return EINVAL;
  }

  int rc = 0;
  size_t file_size = 0;
  char *header_buf = NULL;
  char *tmp_read_buf = NULL;

  self->file_path = file_path;

  self->file = fopen(file_path.s, "rb");

  if (!self->file) {
    rc = errno;
    goto exit;
  }

  // fseek(self->file, 0, SEEK_END);
  // file_size = ftell(self->file);
  // fseek(self->file, 0, SEEK_SET);
  //
  // tmp_read_buf = (char *)malloc(file_size);
  //
  // if (!tmp_read_buf) {
  //   rc = ENOMEM;
  //   goto exit;
  // }
  //
  // size_t read_size = fread(tmp_read_buf, 1, file_size, self->file);
  //
  // if (read_size != file_size) {
  //   rc = EIO;
  //   goto exit;
  // }
  //

  header_buf = (char *)malloc(SHS_HEADER_SIZE);

  fseek(self->file, 0, SEEK_SET);
  size_t read_size = fread(header_buf, 1, SHS_HEADER_SIZE, self->file);

  shs_binary_stream_t stream;
  shs_bs_init(&stream, read_size);

  shs_bs_write(&stream, header_buf, read_size);

  shs_header_deserialize(&self->header, &stream);

  shs_header_print(&self->header);

  size_t binary_table_size = self->header.file_table_size;
  size_t binary_table_offset = self->header.file_table_offset;

  fseek(self->file, binary_table_offset, SEEK_SET);

  tmp_read_buf = (char *)malloc(binary_table_size);

  if (!tmp_read_buf) {
    rc = ENOMEM;
    goto exit;
  }

  read_size = fread(tmp_read_buf, 1, binary_table_size, self->file);

  if (read_size != binary_table_size) {
    rc = EIO;
    goto exit;
  }

  self->binary_table = tmp_read_buf;
  self->test = find_file_info;
  self->open = shs_find_file;
  self->close = shs_close_file;
  self->read = shs_read;
  self->stat = shs_stat_file;
  self->lseek = shs_lseek;

  rc = shs_inner_file_table_init(&self->file_table, DEFAULT_FILE_TABLE_SIZE);

  if (rc != 0) {
    goto exit;
  }

exit:

  shs_bs_release(&stream);

  if (header_buf) {
    free(header_buf);
  }

  if (rc != 0) {
    shs_accessor_release(self);
  }

  return 0;
}

void shs_inner_file_table_deleter(void *context, file_info_t *info) {
  shs_inner_file_table_remove_item((shs_inner_file_table_t *)context, info);
  free(info);
}

void shs_accessor_release(shs_accessor_t *self) {
  if (!self) {
    return;
  }

  q_string_free(self->file_path);

  if (self->binary_table) {
    free(self->binary_table);
    self->binary_table = NULL;
  }

  if (self->file) {
    fclose(self->file);
    self->file = NULL;
  }

  shs_inner_file_table_visit(&self->file_table, shs_inner_file_table_deleter,
                             &self->file_table);

  return;
}
