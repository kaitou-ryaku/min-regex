#ifndef __PRINT__
#define __PRINT__
#include "common.h"
#include <stdio.h>

void node_list_to_dot(
  FILE*         fp
  , const NODE* node
  , const int*  topic_node_list
  , const char* fontsize
  , const char* width
  , const char* topic_color
  , const char* boundary_color
  , const char* normal_color
);

void node_match_list_to_dot(
  FILE*          fp
  , const NODE*  node
  , const char*  fontsize
  , const char*  width
  , const char*  topic_color
  , const char*  boundary_color
  , const char*  normal_color
  , const MATCH* match
  , const int    step
);

extern void node_match_list_to_dot_inside(
  FILE*          fp
  , const int    graph_id
  , const NODE*  node
  , const char*  fontsize
  , const char*  width
  , const char*  topic_color
  , const char*  boundary_color
  , const char*  normal_color
  , const MATCH* match
  , const int step
);

extern void print_match_list(
  FILE*          fp
  , const NODE*  node
  , const MATCH* match
  , const int    step
);
#endif
