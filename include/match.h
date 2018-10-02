#ifndef __MATCH__
#define __MATCH__
#include "common.h"
#include <stdbool.h>

void initialize_match( MATCH* match, const int match_list_size);
int match_all_str( const char* str, const NODE* node, MATCH *match, const int match_list_size);

#endif
