#include "../include/match.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/print.h"

#define __REGISTER_STATIC_FUNCTION__(x) do {if (strcmp(func_name, #x) == 0) return (void (*)(void)) x;} while(0)

static int debug_id=0;
// 関数プロトタイプ宣言/*{{{*/
static void initialize_match( MIN_REGEX_MATCH* match, const int match_list_size);
static int arbitary_match( const int begin, const int end, const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size);
static void match_str( const char* str, const int str_length, int* seek, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, int* step, const int match_list_size, const bool is_back);
static int find_back_node(const MIN_REGEX_NODE* node, const MIN_REGEX_MATCH* match, const int step);
/*}}}*/
// デバッグ用プロトタイプ/*{{{*/
static void debug_print_match_list(const MIN_REGEX_MATCH *match, const MIN_REGEX_NODE* node, const char* str, const int total_step);
static void debug_print_match_str_args(const char* str, const int* seek, const MIN_REGEX_NODE* node, const MIN_REGEX_MATCH *match, const int* step, const int match_list_size, const bool is_back);
static void debug_print_match_str_dot(const MIN_REGEX_NODE* node, const MIN_REGEX_MATCH* match, const int step);
/*}}}*/
// 関数本体/*{{{*/
static void initialize_match( MIN_REGEX_MATCH* match, const int match_list_size) {/*{{{*/
  for (int i=0; i<match_list_size; i++) {
    match[i].step       =  i;    // オートマトン遍歴のステップ数。
    match[i].node_index = -1;    // オートマトンの添字番号。
    match[i].str_index  = -1;    // 文字列の添字番号。
  }
}/*}}}*/
extern int exact_match( const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size) {/*{{{*/
  int step = arbitary_match(0, strlen(str), str, node, match, match_list_size);
  return step;
}/*}}}*/
extern int forward_shortest_match( const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size) {/*{{{*/
  int step;
  for (int end=0; end<=strlen(str); end++) {
    step = arbitary_match(0, end, str, node, match, match_list_size);
    if (step > 0) break;
  }
  return step;
}/*}}}*/
extern int forward_longest_match( const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size) {/*{{{*/
  int step;
  for (int rest=0; rest<=strlen(str); rest++) {
    int end = strlen(str) - rest;
    step = arbitary_match(0, end, str, node, match, match_list_size);
    if (step > 0) break;
  }
  return step;
}/*}}}*/
extern int backward_longest_match( const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size) {/*{{{*/
  int step;
  for (int begin=0; begin<=strlen(str); begin++) {
    step = arbitary_match(begin, strlen(str), str, node, match, match_list_size);
    if (step > 0) break;
  }
  return step;
}/*}}}*/
extern int backward_shortest_match( const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size) {/*{{{*/
  int step;
  for (int rest=0; rest<=strlen(str); rest++) {
    int begin = strlen(str) - rest;
    step = arbitary_match(begin, strlen(str), str, node, match, match_list_size);
    if (step > 0) break;
  }
  return step;
}/*}}}*/
static int arbitary_match( const int begin, const int end, const char* str, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, const int match_list_size) {/*{{{*/
  // str[begin], str[begin+1], ..., str[end-1], (str[end]==\0)
  // と完全一致すれば、埋まったmatch[]配列のサイズを返す

  assert(0 <= begin);
  assert(end <= strlen(str));
  assert(begin <= end);

  const char* str_begin  = &(str[begin]);
  const int   str_length = end - begin;

  initialize_match(match, match_list_size);
  int step = 0;

  // 0ステップ目は、str_begin[-1]に対応する空文字を^にマッチしたと考える
  match[step].node_index = 0;  // node '^'
  match[step].str_index  = -1;

  int seek = 0;
  // 再帰呼出の末尾でseekが再び0になると、アンマッチで終了
  match_str(str_begin, str_length, &seek, node, match, &step, match_list_size, false);

  return step;
}/*}}}*/
static void match_str( const char* str, const int str_length, int* seek, const MIN_REGEX_NODE* node, MIN_REGEX_MATCH *match, int* step, const int match_list_size, const bool is_back) {/*{{{*/
  // 一時的に画像を書き出し
  // debug_print_match_str_dot(node, match, *(step));
  // debug_print_match_str_args(str, seek, node, match, step, match_list_size, is_back);
  // debug_print_match_list(match,  node,  str, (*step)+1);
  assert((*step) < match_list_size);
  if (is_back && ((*step) == 0)) return;

  // バックトラックでは、今までのステップ中に含まれる最近のメタ文字に移動する。
  // そのメタ文字へ来訪様式の違いと、次回ステップの関係は
  // * 通常訪問 -> fstを新規訪問
  // * バックトラックで訪問。fstのみ訪問済み。sndは空でない -> sndを新規訪問
  // * バックトラックで訪問。fstのみ訪問済み。sndが空 -> 更にバックトラック
  // * バックトラックで訪問。fst,snd共に探索済み -> 更にバックトラック

  const MIN_REGEX_MATCH m = match[(*step)];
  const MIN_REGEX_NODE  n = node[m.node_index];

  // メタ文字のノードにいる場合、訪問方法が通常かバックトラックか考慮する
  if (n.is_magick) {
    // 文字列が減らないまま再訪したかチェック
    bool revisit_without_decrease = false;
    for (int i=0; i<(*step)-1; i++) {
      const MIN_REGEX_MATCH tmp_m = match[i];
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
      match_str( str, str_length, seek, node, match, step, match_list_size, false);

    // グラフが$でない、バックトラックで訪問、fst のみ探索済みかつ、snd が空でない -> sndに行く
    } else if ((n.symbol != '$') && (!revisit_without_decrease) && (n.out_fst == match[(*step)+1].node_index) && (n.out_snd >= 0)) {
      match[(*step)].str_index  = (*seek);
      (*step)++;
      match[(*step)].node_index = n.out_snd;
      match_str( str, str_length, seek, node, match, step, match_list_size, false);

    // グラフが$で、比較文字が文字が残ってない -> 終了
    } else if ((n.symbol == '$') && ((*seek) == str_length)) {
      match[(*step)].str_index  = -1;
      match[(*step)].str_index  = 1;
      (*step)++;

    // 文字列が減らないまま再訪問                         -> バックトラック
    // バックトラックで訪問、fst のみ探索済みで、snd が空 -> 更にバックトラック
    // バックトラックで訪問、fst, snd 共に探索済み        -> 更にバックトラック
    } else {
      (*step) = find_back_node( node, match, (*step));
      (*seek) = match[(*step)].str_index;
      match_str( str, str_length, seek, node, match, step, match_list_size, true);
    }

  // 文字のノードにいる場合、マッチの成否を調べる
  } else {

    // マッチ成功 -> 次のノードに行く
    if (str[(*seek)] == n.symbol) {
      match[(*step)].str_index  = (*seek);
      (*step)++;
      (*seek)++;
      match[(*step)].node_index = n.out_fst;
      match_str( str, str_length, seek, node, match, step, match_list_size, false);

    // マッチ失敗 -> バックトラック
    } else {
      (*step) = find_back_node( node, match, (*step));
      (*seek) = match[(*step)].str_index;
      match_str( str, str_length, seek, node, match, step, match_list_size, true);
    }

  }
}/*}}}*/
static int find_back_node(const MIN_REGEX_NODE* node, const MIN_REGEX_MATCH* match, const int step) {/*{{{*/
  int delta = 1;
  while (delta < step) {
    const MIN_REGEX_NODE back_node  = node[match[step-delta].node_index];
    if (back_node.is_magick) break;
    delta++;
  }
  return step - delta;
}/*}}}*/
/*}}}*/
// デバッグ用関数本体/*{{{*/
static void debug_print_match_list(const MIN_REGEX_MATCH *match, const MIN_REGEX_NODE* node, const char* str, const int total_step) {/*{{{*/
  for (int i=0; i<total_step; i++) {
    MIN_REGEX_MATCH m = match[i];
    MIN_REGEX_NODE  n = node[match[i].node_index];
    char  c;
    if (m.str_index < 0 ) c = ' ';
    else c = str[m.str_index];
    fprintf(stderr, "%3d %c | %3d %c | %3d %3d\n", m.step, c, m.node_index, n.symbol, n.out_fst, n.out_snd);
  }
}/*}}}*/

static void debug_print_match_str_args(const char* str, const int* seek, const MIN_REGEX_NODE* node, const MIN_REGEX_MATCH *match, const int* step, const int match_list_size, const bool is_back) {
  char c;
  if ((*seek) == -1) c = ' ';
  else               c = str[(*seek)];
  fprintf(stderr, "\n--- %c %3d %3d %s ---\n", c, (*seek), (*step), is_back?"back":"forward");
}
static void debug_print_match_str_dot(const MIN_REGEX_NODE* node, const MIN_REGEX_MATCH* match, const int step) {
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
  __REGISTER_STATIC_FUNCTION__(initialize_match);
  __REGISTER_STATIC_FUNCTION__(match_str);
  __REGISTER_STATIC_FUNCTION__(find_back_node);
  __REGISTER_STATIC_FUNCTION__(debug_print_match_list);
  __REGISTER_STATIC_FUNCTION__(debug_print_match_str_args);
  __REGISTER_STATIC_FUNCTION__(debug_print_match_str_dot);
  return NULL;
}/*}}}*/
