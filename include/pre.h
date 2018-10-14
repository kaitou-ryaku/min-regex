#ifndef __MIN_REGEX_PRE__
#define __MIN_REGEX_PRE__
#include <stdio.h>
#include <stdbool.h>

void simplify_regex( const char* original_regex, const int begin, const int end, char *simple_regex, int* current, const int size);

#endif
