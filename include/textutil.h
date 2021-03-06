#ifndef __MIN_REGEX_TEXTUTILS__
#define __MIN_REGEX_TEXTUTILS__
#include <stdbool.h>

int search_corresponding_paren(const char* s, const int i, const int end);
int search_inner_letter(const char* s, const int i, const char c, const int end);
bool is_magick(const char c);

#endif
