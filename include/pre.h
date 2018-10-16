#ifndef __MIN_REGEX_PRE__
#define __MIN_REGEX_PRE__
#include <stdio.h>
#include <stdbool.h>

int simplify_regex( const char* original_regex, char *simple_regex, const int size);
int simplify_regex_arbitary( const char* original_regex, const int original_regex_begin, const int original_regex_end, char *simple_regex, const int size);

#endif
