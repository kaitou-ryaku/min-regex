#ifndef __NODE__
#define __NODE__
#include <stdio.h>
#include <stdbool.h>

typedef struct {// NODE/*{{{*/
  int  self;         // この構造体配列における、このノードの番号。
  int  total;        // この構造体配列のサイズ。
  char symbol;       // 正規表現の記号列に含まれる、対応する記号。
  int  symbol_index; // 対応する記号の配列番号。
  bool is_magick;    // 記号が役文字ならtrue、文字ならfalse。
  int  in_fst;       // 1番目の入力ノードの番号。最初のノードなら-1。他は0以上。
  int  in_snd;       // 2番目の入力ノードの番号。集結点でなければ-1。
  int  out_fst;      // 1番目の出力ノードの番号。最後のノードなら-1。他は0以上。
  int  out_snd;      // 2番目の出力ノードの番号。分岐点でなければ-1。
} NODE;/*}}}*/

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
