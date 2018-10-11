#include "../include/match.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/print.h"

#define __REGISTER_STATIC_FUNCTION__(x) do {if (strcmp(func_name, #x) == 0) return (void (*)(void)) x;} while(0)

static int debug_id=0;
// 関数プロトタイプ宣言/*{{{*/
static void match_str( const char* str, int* seek, const NODE* node, MATCH *match, int* step, const int match_list_size, const bool is_back);
static int find_back_node(const NODE* node, const MATCH* match, const int step);
/*}}}*/
// デバッグ用プロトタイプ/*{{{*/
static void debug_print_match_list(const MATCH *match, const NODE* node, const char* str, const int total_step);
static void debug_print_match_str_args(const char* str, const int* seek, const NODE* node, const MATCH *match, const int* step, const int match_list_size, const bool is_back);
static void debug_print_match_str_dot(const NODE* node, const MATCH* match, const int step);
/*}}}*/
// 関数本体/*{{{*/
extern void initialize_match( MATCH* match, const int match_list_size) {/*{{{*/
  for (int i=0; i<match_list_size; i++) {
    match[i].step       =  i;    // オートマトン遍歴のステップ数。
    match[i].node_index = -1;    // オートマトンの添字番号。
    match[i].str_index  = -1;    // 文字列の添字番号。
  }
}/*}}}*/
extern int match_all_str( const char* str, const NODE* node, MATCH *match, const int match_list_size) {/*{{{*/
  int step = 0;
  // 0ステップ目は、str[-1]に対応する空文字を^にマッチしたと考える
  match[step].node_index = 0;  // node '^'
  match[step].str_index  = -1;

  int seek = 0;
  // 再帰呼出の末尾でseekが再び0になると、アンマッチで終了
  match_str(str, &seek, node, match, &step, match_list_size, false);

  return step;
}/*}}}*/
static void match_str( const char* str, int* seek, const NODE* node, MATCH *match, int* step, const int match_list_size, const bool is_back) {/*{{{*/
  // 一時的に画像を書き出し
  // debug_print_match_str_dot(node, match, *(step));
  // debug_print_match_str_args(str, seek, node, match, step, match_list_size, is_back);
  // debug_print_match_list(match,  node,  str, (*step)+1);
  assert((*step) < match_list_size);
  if (is_back && ((*step) == 0)) return;

  // バックトラックでは、今までのステップ中に含まれる最近の役文字に移動する。
  // その役文字へ来訪様式の違いと、次回ステップの関係は
  // * 通常訪問 -> fstを新規訪問
  // * バックトラックで訪問。fstのみ訪問済み。sndは空でない -> sndを新規訪問
  // * バックトラックで訪問。fstのみ訪問済み。sndが空 -> 更にバックトラック
  // * バックトラックで訪問。fst,snd共に探索済み -> 更にバックトラック

  const MATCH m = match[(*step)];
  const NODE  n = node[m.node_index];

  // 役文字のノードにいる場合、訪問方法が通常かバックトラックか考慮する
  if (n.is_magick) {
    // 文字列が減らないまま再訪したかチェック
    bool revisit_without_decrease = false;
    for (int i=0; i<(*step)-1; i++) {
      const MATCH tmp_m = match[i];
      if (tmp_m.str_index == (*seek) && tmp_m.node_index == m.node_index) {
        revisit_without_decrease = true;
        break;
      }
    }

    // グラフが$でない、初めてこのノードを訪問 -> fstに行く
    if ((n.symbol != '$') && (!revisit_without_decrease) && (!is_back)) {
      match[(*step)].str_index  = (*seek);
      (*step)++;
      match[(*step)].node_index = n.out_fst;
      match_str( str, seek, node, match, step, match_list_size, false);

    // グラフが$でない、バックトラックで訪問、fst のみ探索済みかつ、snd が空でない -> sndに行く
    } else if ((n.symbol != '$') && (!revisit_without_decrease) && (n.out_fst == match[(*step)+1].node_index) && (n.out_snd >= 0)) {
      match[(*step)].str_index  = (*seek);
      (*step)++;
      match[(*step)].node_index = n.out_snd;
      match_str( str, seek, node, match, step, match_list_size, false);

    // グラフが$で、比較文字が文字が残ってない -> 終了
    } else if ((n.symbol == '$') && ((*seek) == strlen(str))) {
      match[(*step)].str_index  = -1;
      match[(*step)].str_index  = 1;
      (*step)++;

    // 文字列が減らないまま再訪問                         -> バックトラック
    // バックトラックで訪問、fst のみ探索済みで、snd が空 -> 更にバックトラック
    // バックトラックで訪問、fst, snd 共に探索済み        -> 更にバックトラック
    } else {
      (*step) = find_back_node( node, match, (*step));
      (*seek) = match[(*step)].str_index;
      match_str( str, seek, node, match, step, match_list_size, true);
    }

  // 文字のノードにいる場合、マッチの成否を調べる
  } else {

    // マッチ成功 -> 次のノードに行く
    if (str[(*seek)] == n.symbol) {
      match[(*step)].str_index  = (*seek);
      (*step)++;
      (*seek)++;
      match[(*step)].node_index = n.out_fst;
      match_str( str, seek, node, match, step, match_list_size, false);

    // マッチ失敗 -> バックトラック
    } else {
      (*step) = find_back_node( node, match, (*step));
      (*seek) = match[(*step)].str_index;
      match_str( str, seek, node, match, step, match_list_size, true);
    }

  }
}/*}}}*/
static int find_back_node(const NODE* node, const MATCH* match, const int step) {/*{{{*/
  int delta = 1;
  while (delta < step) {
    const NODE back_node  = node[match[step-delta].node_index];
    if (back_node.is_magick) break;
    delta++;
  }
  return step - delta;
}/*}}}*/
/*}}}*/
// デバッグ用関数本体/*{{{*/
static void debug_print_match_list(const MATCH *match, const NODE* node, const char* str, const int total_step) {/*{{{*/
  for (int i=0; i<total_step; i++) {
    MATCH m = match[i];
    NODE  n = node[match[i].node_index];
    char  c;
    if (m.str_index < 0 ) c = ' ';
    else c = str[m.str_index];
    fprintf(stderr, "%3d %c | %3d %c | %3d %3d\n", m.step, c, m.node_index, n.symbol, n.out_fst, n.out_snd);
  }
}/*}}}*/

static void debug_print_match_str_args(const char* str, const int* seek, const NODE* node, const MATCH *match, const int* step, const int match_list_size, const bool is_back) {
  char c;
  if ((*seek) == -1) c = ' ';
  else               c = str[(*seek)];
  fprintf(stderr, "\n--- %c %3d %3d %s ---\n", c, (*seek), (*step), is_back?"back":"forward");
}
static void debug_print_match_str_dot(const NODE* node, const MATCH* match, const int step) {
  // 一時的に画像を書き出し
  debug_id++;
  char filename[100];
  sprintf(filename, "graph%05d.dot", debug_id);
  FILE *file = fopen(filename, "w");
  fprintf( file, "digraph graphname {\n");
  fprintf( file, "  graph [rankdir = LR]\n");
  node_match_list_to_dot_inside( file, debug_id, node, "12.0", "0.2", "#FF0000", "#FF0000", "#000000", match, step);
  fprintf( file, "}\n");
  fclose(file);
}
/*}}}*/
// static関数の密輸関数/*{{{*/
extern void (*__static_match_func__(const char *func_name)) (void) {
  __REGISTER_STATIC_FUNCTION__(match_str);
  __REGISTER_STATIC_FUNCTION__(find_back_node);
  __REGISTER_STATIC_FUNCTION__(debug_print_match_list);
  __REGISTER_STATIC_FUNCTION__(debug_print_match_str_args);
  __REGISTER_STATIC_FUNCTION__(debug_print_match_str_dot);
  return NULL;
}/*}}}*/
