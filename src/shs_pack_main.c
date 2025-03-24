#include "file_table.h"
#include "shs_accessor.h"
#include "shs_builder.h"
#include "shs_pack.h"
#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *argp_program_version = "shs-pack v0.0.1";
const char *argp_program_bug_address = "zhongguoqiang@vivo.com";

/* Program documentation. */
static char doc[] = "SHS file packer -- "
                    "A tool to pack a directory to a single file."
                    "User can include shs library and access files in the "
                    "packed file without unpacking it.";

/* A description of the arguments we accept. */
static char args_doc[] =
    "DIRECTORY: directory path that need to be packed, FILE: output file name";

/* The options we understand. */
static struct argp_option options[] = {{0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
  char *args[2]; /* arg1 & arg2 */
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key) {
  case ARGP_KEY_ARG:
    if (state->arg_num >= 2)
      /* Too many arguments. */
      argp_usage(state);

    arguments->args[state->arg_num] = arg;

    break;

  case ARGP_KEY_END:
    if (state->arg_num < 2)
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

int main(int argc, char **argv) {
  struct arguments arguments;

  /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  flatcc_builder_t builder;
  flatcc_builder_init(&builder);
  flatcc_builder_clear(&builder);

  shs_file_packer_t packer;
  q_string_t root_path =
      q_string_new(arguments.args[0], strlen(arguments.args[0]));
  q_string_t output_path =
      q_string_new(arguments.args[1], strlen(arguments.args[1]));

  shs_file_packer_init(&packer, root_path, output_path);

  packer.pack(&packer);

  shs_file_packer_release(&packer);

  return 0;
}
