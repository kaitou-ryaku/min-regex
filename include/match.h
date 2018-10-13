#ifndef __MATCH__
#define __MATCH__
#include "common.h"
#include <stdbool.h>

void (*__static_match_func__(const char *func_name)) (void);
int match_all_str( const char* str, const NODE* node, MATCH *match, const int match_list_size);

#endif
