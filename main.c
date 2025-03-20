#include "file_table.h"
#include "shs_accessor.h"
#include "shs_builder.h"
#include "shs_pack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(fb_file_table, x)

volatile sig_atomic_t got_sigint = 0;

void sigint_handler(int sig) { got_sigint = 1; }

// int main(int argc, char **argv) {
//
//     flatcc_builder_t builder;
//     flatcc_builder_init(&builder);
//
//     flatcc_builder_clear(&builder);
//
//
//     signal(SIGINT, sigint_handler); // 设置SIGINT信号的处理函数
//
//     shs_file_packer_t packer;
//     q_string_t root_path = q_string_new(argv[1], strlen(argv[1]));
//     q_string_t output_path = q_string_new(argv[2], strlen(argv[2]));
//
//     shs_file_packer_init(&packer, root_path, output_path);
//
//     packer.pack(&packer);
//
//     printf("程序开始运行，按Ctrl+C退出\n");
//
//
//     shs_file_packer_release(&packer);
//
//     return 0;
// }

static int is_shs(const char *path) {
  // 检查后缀是否为".shs"

  if (!path) {
    return 0;
  }

  char *suffix = (char *)path;
  char *very_last_dot = strrchr(path, '.');

  while ((suffix = strchr(suffix, '.')) != NULL) {
    size_t offset = suffix - path;
    if (offset < strlen(path)) {
      suffix++;
      very_last_dot = suffix;
    } else {
      break;
    }
  }

  if (!very_last_dot)
    return 0;

  return strncmp(very_last_dot, "shs", 3) == 0;
}

int main(int argc, char **argv) {

  printf("is_shs: %d\n",
         is_shs("/resource/fitness/Two_way_turning_head_4_times.shs"));

  shs_accessor_t accessor;
  char *buf;
  shs_accessor_init(&accessor, q_string_new(argv[1], strlen(argv[1])));

  signal(SIGINT, sigint_handler); // 设置SIGINT信号的处理函数

  printf("程序开始运行，按Ctrl+C退出\n");

  while (!got_sigint) {
    // 这里接受用户输入，可以使用scanf或者fgets等函数
    // 例如：
    char input[100];
    printf("请输入：");
    fgets(input, sizeof(input), stdin);

    // 处理用户输入的内容
    printf("您输入了：%s", input);

    char key[100];
    size_t index = 0;

    memset(key, 0, sizeof(key));

    if (input[0] != '/') {
      key[0] = '/';
      index++;
    }
    printf("key: %s\n", key);
    memcpy(key + index, input, strlen(input) - 1);

    // accessor.test(&accessor, key);

    int fd = accessor.open(&accessor, key);

    if (fd < 0) {
      fprintf(stderr, "Open file %s fail\n", key);
      goto exit;
    }

    file_info_t info;

    printf("file fd is %d\n", fd);
    int rc = accessor.stat(&accessor, fd, &info);

    if (rc != 0) {
      fprintf(stderr, "Get file state fail: %d\n", rc);
      goto exit;
    }

    printf("file size: %ld \n", info.file_size);

    rc = accessor.lseek(&accessor, fd, 0, SEEK_SET);

    if (rc < 0) {
      fprintf(stderr, "Lseek fail: %d\n", rc);
      goto exit;
    }

    size_t buf_size = info.file_size;

    buf = (char *)malloc(buf_size + 1);

    if (!buf) {
      fprintf(stderr, "malloc fail\n");
      goto exit;
    }

    memset(buf, 0, buf_size);

    size_t read_size = accessor.read(&accessor, fd, buf_size, buf);

    if (read_size != buf_size) {
      fprintf(stderr, "read size not same: %ld but expect %ld\n", read_size,
              buf_size);
      goto exit;
    }

    printf("buf: %s\n", buf);

    accessor.close(&accessor, fd);
  }

exit:
  shs_accessor_release(&accessor);

  if (buf) {
    free(buf);
    buf = NULL;
  }

  return 0;
}
