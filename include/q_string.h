#ifndef _Q_STRING_H_
#define _Q_STRING_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct q_string {
  char *s;
  size_t len;
} q_string_t;

int q_string_init(q_string_t *qs, const char *s, size_t len);

void q_string_release(q_string_t *qs);

q_string_t q_string_new(const char *s, size_t len);

void q_string_free(q_string_t qs);

bool is_q_string_equal(const q_string_t *a, const q_string_t *b);

bool is_q_string_nil(const q_string_t *qs);

q_string_t q_string_copy(q_string_t qs);

#endif
