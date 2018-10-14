#ifndef __MIN_REGEX_MATCH__
#define __MIN_REGEX_MATCH__
#include "common.h"
#include <stdbool.h>

void (*__static_match_func__(const char *func_name)) (void);
int exact_match(            const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size);
int forward_shortest_match( const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size);
int forward_longest_match(  const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size);
int backward_shortest_match(const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size);
int backward_longest_match( const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size);

#endif
