#ifndef __NODE__
#define __NODE__
#include "common.h"
#include <stdio.h>
#include <stdbool.h>

void (*__static_node_func__(const char *func_name)) (void);
void regex_to_all_node( const char* regex_str, NODE* node, const int node_list_size);

#endif
