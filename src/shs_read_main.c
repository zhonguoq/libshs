#include "file_table.h"
#include "shs_accessor.h"
#include "shs_builder.h"
#include "shs_pack.h"
#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(fb_file_table, x)

const char *argp_program_version = "shs-read v0.0.1";
const char *argp_program_bug_address = "zhongguoqiang@vivo.com";

/* Program documentation. */
static char doc[] = "SHS file reader -- use to check whether the .shs is "
                    "illegal and inner files can be readed";

/* A description of the arguments we accept. */
static char args_doc[] = "SHS_FILE: the .shs file path that need to be readed";

/* The options we understand. */
static struct argp_option options[] = {{0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
  char *args[1]; /* arg1 */
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key) {
  case ARGP_KEY_ARG:
    if (state->arg_num >= 1)
      /* Too many arguments. */
      argp_usage(state);

    arguments->args[state->arg_num] = arg;

    break;

  case ARGP_KEY_END:
    if (state->arg_num < 1)
      /* Not enough arguments. */
      argp_usage(state);
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

volatile sig_atomic_t got_sigint = 0;

void sigint_handler(int sig) { got_sigint = 1; }

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

  struct arguments arguments;

  /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  printf("is_shs: %d\n", is_shs(arguments.args[0]));

  shs_accessor_t accessor;
  char *buf;
  shs_accessor_init(&accessor, q_string_new(argv[1], strlen(argv[1])));

  // 设置 sigaction 而非 signal
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigint_handler;
  // 注意：不要设置 SA_RESTART
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  printf("程序开始运行，输入想要查看的文件路径.(大小写敏感)\n "
         "如果该文件不存在或者输入CTRL-C，程序会自动退出:");

  while (!got_sigint) {
    // 这里接受用户输入，可以使用scanf或者fgets等函数
    // 例如：
    char input[100];
    printf("请输入：");
    fgets(input, sizeof(input), stdin);
    size_t i_len = strlen(input);

    // 处理用户输入的内容

    if (i_len == 0 || strncmp(input, "\n", 1) == 0) {
      continue;
    }

    printf("您输入了：%s\n", input);

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
