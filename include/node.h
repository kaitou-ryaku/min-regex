#ifndef __NODE__
#define __NODE__
#include "common.h"
#include <stdio.h>
#include <stdbool.h>

void (*__static_node_func__()) (void);
void regex_to_all_node( const char* regex_str, NODE* node, const int node_list_size);
void initialize_node( NODE *node, const int node_list_size);
void simplify_regex( const char* original_regex, const int begin, const int end, char *simple_regex, int* current, const int size);


#endif
