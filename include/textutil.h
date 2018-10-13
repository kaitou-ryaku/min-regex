#ifndef __PRE__
#define __PRE__
#include <stdbool.h>

int search_corresponding_paren(const char* s, const int i, const int end);
int search_inner_letter(const char* s, const int i, const char c, const int end);
int search_next_char_index(const char *s, const int i, const int end);
bool is_magick(const char c);
extern int search_factor_end(const char* s, const int i, const int end);

#endif
