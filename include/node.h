#ifndef __NODE__
#define __NODE__
#include "common.h"
#include <stdio.h>
#include <stdbool.h>

void regex_to_all_node( const char* regex_str, NODE* node, const int node_list_size);
void initialize_node( NODE *node, const int node_list_size);
void node_list_to_dot(
  FILE*         fp
  , NODE*       node
  , const int   topic_node
  , const char* fontsize
  , const char* width
  , const char* topic_color
  , const char* boundary_color
  , const char* normal_color
);

#endif
