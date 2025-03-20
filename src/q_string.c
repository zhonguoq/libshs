#include "q_string.h"

int q_string_init(q_string_t *qs, const char *s, size_t len) {
  qs->s = (char *)malloc(len + 1);
  if (qs->s == NULL) {
    return -1;
  }
  memcpy(qs->s, s, len);
  qs->s[len] = '\0';
  qs->len = len;
  return 0;
}

void q_string_release(q_string_t *qs) {
  free(qs->s);
  qs->s = NULL;
  qs->len = 0;
}

q_string_t q_string_new(const char *s, size_t len) {
  q_string_t qs;
  if (q_string_init(&qs, s, len) != 0) {
    return (q_string_t){NULL, 0};
  }
  return qs;
}

void q_string_free(q_string_t qs) { q_string_release(&qs); }

bool is_q_string_equal(const q_string_t *a, const q_string_t *b) {
  return a->len == b->len && memcmp(a->s, b->s, a->len) == 0;
}

bool is_q_string_nil(const q_string_t *qs) { return qs->s == NULL; }
