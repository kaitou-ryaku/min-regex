#ifndef __MIN_REGEX_NODE__
#define __MIN_REGEX_NODE__
#include "common.h"
#include <stdio.h>
#include <stdbool.h>

void (*__static_node_func__(const char *func_name)) (void);
void regex_to_all_node( const char* regex_str, MIN_REGEX_NODE* node, const int node_list_size);

#endif
