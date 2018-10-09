#include <stdio.h>
#include "../include/print.h"
#include "../include/node.h"
#include "../include/match.h"

// 関数プロトタイプ/*{{{*/
static void node_list_to_dot_inside( FILE *fp, const int graph_id, const NODE *node, const int* topic_node_list, const char* fontsize, const char* width, const char* topic_color, const char* boundary_color, const char* normal_color);
/*}}}*/
// 関数本体/*{{{*/
static void node_list_to_dot_inside( FILE *fp, const int graph_id, const NODE *node, const int* topic_node_list, const char* fontsize, const char* width, const char* topic_color, const char* boundary_color, const char* normal_color) {/*{{{*/
  for (int i=0; i<node[0].total;i ++) {
    NODE n = node[i];
    fprintf( fp, "  ");
    fprintf( fp, "%05d%05d [ "      , graph_id, i);
    if (n.symbol == '\\') {
      fprintf( fp, "label=\"\\\\\", ");
    } else {
      fprintf( fp, "label=\"%c\", " , n.symbol);
    }
    fprintf( fp, "fontsize=%s, ", fontsize);
    fprintf( fp, "width=%s, "   , width);

    if (n.is_magick) {
      fprintf( fp, "shape=circle, ");
    } else {
      fprintf( fp, "shape=square, ");
    }

    if (topic_node_list[i] == 1) {
      fprintf( fp, "style=filled, fillcolor=\"%s\"",  topic_color);
    } else if (i == 0 || i == 1){
      fprintf( fp, "fontcolor=\"%s\", color=\"%s\"", boundary_color, boundary_color);
    }  else {
      fprintf( fp, "fontcolor=\"%s\", color=\"%s\"", normal_color, normal_color);
    }

    fprintf( fp, "]\n");
  }

  fprintf( fp, "\n");

  for (int i=0; i<node[0].total; i++) {
    NODE n = node[i];
    if (n.out_fst >= 0) fprintf( fp, "  %05d%05d -> %05d%05d\n", graph_id, i, graph_id, n.out_fst);
    if (n.out_snd >= 0) fprintf( fp, "  %05d%05d -> %05d%05d\n", graph_id, i, graph_id, n.out_snd);
  }
}/*}}}*/
extern void node_list_to_dot( FILE *fp, const NODE *node, const int* topic_node_list, const char* fontsize, const char* width, const char* topic_color, const char* boundary_color, const char* normal_color) {/*{{{*/
  fprintf( fp, "digraph graphname {\n");
  fprintf( fp, "  graph [rankdir = LR]\n");

  node_list_to_dot_inside( fp, 0, node, topic_node_list, fontsize, width, topic_color, boundary_color, normal_color);

  fprintf( fp, "}\n");
}/*}}}*/
extern void node_match_list_to_dot_inside( FILE *fp, const int graph_id, const NODE *node, const char* fontsize, const char* width, const char* topic_color, const char* boundary_color, const char* normal_color, const MATCH *match, const int step) {/*{{{*/

  int topic_node_list[100] = {0};
  for (int i=0; i<step; i++) {
    topic_node_list[match[i].node_index] = 1;
  }

  node_list_to_dot_inside( fp, graph_id, node, topic_node_list, fontsize, width, topic_color, boundary_color, normal_color);

  fprintf( fp, "\n  GRAPH%05d [ label=\"", graph_id);
  print_match_list(fp, node, match, step);
  fprintf( fp, "\", shape=none]\n");
}/*}}}*/
extern void node_match_list_to_dot( FILE *fp, const NODE *node, const char* fontsize, const char* width, const char* topic_color, const char* boundary_color, const char* normal_color, const MATCH *match, const int step) {/*{{{*/
  fprintf( fp, "digraph graphname {\n");
  fprintf( fp, "  graph [rankdir = LR]\n");

  for (int i=0; i<step; i++) {
    node_match_list_to_dot_inside( fp, step-i, node, "12.0", "0.2", "#FF0000", "#FF0000", "#000000", match, step-i);
  }

  fprintf( fp, "}\n");
}/*}}}*/
extern void print_match_list( FILE* fp, const NODE* node, const MATCH* match, const int step) {/*{{{*/
  for (int i=0; i<step; i++) {
    fprintf( fp, "%c", node[match[i].node_index].symbol);
  }
}/*}}}*/
/*}}}*/
