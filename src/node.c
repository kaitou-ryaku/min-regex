#include "../include/node.h"
#include "../include/textutil.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define __REGISTER_STATIC_FUNCTION__(x) do {if (strcmp(func_name, #x) == 0) return (void (*)(void)) x;} while(0)

// 関数プロトタイプ/*{{{*/
static void regex_to_node_list(
  const char* regex_str
  , const int regex_begin
  , const int regex_end
  , NODE*     node
  , int*      node_in
  , int*      node_out
  , int*      node_empty
  , const int node_list_size
);
/*}}}*/
// デバッグ用プロトタイプ/*{{{*/
static void debug_print_node_list(const NODE *node, const int node_size);
static void debug_print_regex_to_node_list_args(const char* regex_str, const int regex_begin, const int regex_end, const int regex_next, const int node_empty);
/*}}}*/
// 関数本体/*{{{*/
extern void regex_to_all_node(/*{{{*/
  const char* regex_str
  , NODE*     node
  , const int node_list_size
) {

  // node[0]の始点^と、node[1]の終点$を定義
  const int regex_begin = 0;
  const int regex_end   = strlen(regex_str);

  node[0].symbol       = '^';
  node[0].symbol_index = regex_begin;
  node[0].is_magick    = true;

  node[1].symbol       = '$';
  node[1].symbol_index = regex_end;
  node[1].is_magick    = true;

  // 再帰実行
  int node_in, node_out, node_empty = 2;
  regex_to_node_list(
    regex_str
    , regex_begin
    , regex_end
    , node
    , &node_in
    , &node_out
    , &node_empty
    , node_list_size
  );

  // ^と$のノードに、再帰実行結果のinとoutをつなぐ
  node[0].out_fst = node_in;
  node[1].in_fst  = node_out;
  node[node_in ].in_fst  = 0;
  node[node_out].out_fst = 1;

  // ノードの大域情報を書き込む
  for (int i=0; i<=node_empty; i++) {
    node[i].self  = i;
    node[i].total = node_empty;
  }
}/*}}}*/
static void regex_to_node_list(/*{{{*/
  const char* regex_str
  , const int regex_begin
  , const int regex_end
  , NODE*     node
  , int*      node_in    // 出力only
  , int*      node_out   // 出力only
  , int*      node_empty // 入出力
  , const int node_list_size
) {

  assert(*node_empty < node_list_size);
  const char current_char = regex_str[regex_begin];
  const int node_current = *node_empty;
  const int regex_next = search_next_char_index(regex_str, regex_begin, regex_end);

  // debug_print_node_list(node, *node_empty);
  // debug_print_regex_to_node_list_args(regex_str, regex_begin, regex_end, regex_next, *node_empty);

  // 次の文字*が存在する場合
  if ((regex_next > 0) && (regex_str[regex_next] == '*') && (current_char != '\\')) {
    node[node_current  ].symbol       = '*';
    node[node_current  ].symbol_index = regex_next;
    node[node_current  ].is_magick    = true;
    (*node_in) = node_current;
    (*node_empty)++;

    int tmp_in, tmp_out;
    regex_to_node_list(regex_str, regex_begin, regex_next, node, &tmp_in, &tmp_out, node_empty, node_list_size);
    node[node_current].in_snd  = tmp_out;
    node[node_current].out_fst = node_current;
    node[node_current].out_snd = tmp_in;
    node[tmp_in ].in_fst         = node_current;
    node[tmp_out].out_fst        = node_current;

    if (regex_next+1 < regex_end) {
      regex_to_node_list(regex_str, regex_next+1, regex_end, node, &tmp_in, &tmp_out, node_empty, node_list_size);
      node[node_current].out_fst = tmp_in;
      node[tmp_in].in_fst = node_current;
      (*node_out) = tmp_out;

    } else {
      (*node_out) = node_current;
    }

  // 次の文字が存在しない
  // 次の文字が*以外の文字
  // 次の文字が*'で、その次の文字が存在しない
  // これらのいずれかで、かつ現在の文字が普通のアルファベットか空文字@の場合 ... 空文字@はis_magick()関数でマジック判定されない
  } else if (is_magick(regex_str[regex_begin]) == false){
    int regex_begin_next = regex_begin+1;

    // 現在の文字がエスケープ -> 次の文字をis_magick = trueで登録
    if (current_char == '\\') {
      const char next_char = regex_str[regex_begin+1];
      assert((next_char == '\\') || (next_char == '(') || (next_char == '|') || (next_char == ')') || (next_char == '*') || (next_char == '@'));
      node[node_current].symbol       = next_char;
      node[node_current].symbol_index = regex_begin+1;
      node[node_current].is_magick    = false;
      regex_begin_next = regex_begin+2;

    // 現在の文字が空文字 -> is_magick = trueで登録。これはis_magick()関数と逆
    } else if (current_char == '@') {
      node[node_current].symbol       = current_char;
      node[node_current].symbol_index = regex_begin;
      node[node_current].is_magick    = true;
      regex_begin_next = regex_begin+1;

    // 現在の文字が普通のアルファベット -> is_magick = falseで登録
    } else {
      node[node_current].symbol       = current_char;
      node[node_current].symbol_index = regex_begin;
      node[node_current].is_magick    = false;
      regex_begin_next = regex_begin+1;

    }
    (*node_in) = node_current;
    (*node_empty)++;

    if (regex_begin_next < regex_end) {
      int tmp_in, tmp_out;
      regex_to_node_list(regex_str, regex_begin_next, regex_end, node, &tmp_in, &tmp_out, node_empty, node_list_size);
      node[node_current].out_fst = tmp_in;
      node[tmp_in].in_fst = node_current;
      (*node_out) = tmp_out;

    } else {
      (*node_in)  = node_current;
      (*node_out) = node_current;
    }

  // 次の文字が {'*'以外 or 存在しない} で、現在の文字が(の場合
  } else if (regex_str[regex_begin] == '(') {
    node[node_current  ].symbol       = '(';
    node[node_current  ].symbol_index = regex_begin;
    node[node_current  ].is_magick    = true;
    (*node_in) = node_current;
    (*node_empty)++;

    node[node_current+1].symbol       = ')';

    // ')'の後続がある場合
    if (0 < regex_next) {
      node[node_current+1].symbol_index = regex_next-1;

    // ')'の後続がない場合
    } else {
      node[node_current+1].symbol_index = regex_end-1;
    }

    node[node_current+1].is_magick    = true;
    (*node_out) = node_current+1;
    (*node_empty)++;

    const int regex_pipe = search_inner_letter(regex_str, regex_begin, '|', regex_end);
    int tmp_in, tmp_out;

    // (...|の処理
    regex_to_node_list(regex_str, regex_begin+1, regex_pipe, node, &tmp_in, &tmp_out, node_empty, node_list_size);
    node[node_current  ].out_fst = tmp_in;
    node[node_current+1].in_fst  = tmp_out;
    node[tmp_in].in_fst          = node_current;
    node[tmp_out].out_fst        = node_current+1;

    if (0 < regex_next) {
      // ')'の後続がある場合の|...)の処理
      regex_to_node_list(regex_str, regex_pipe+1, regex_next-1, node, &tmp_in, &tmp_out, node_empty, node_list_size);
      node[node_current  ].out_snd = tmp_in;
      node[node_current+1].in_snd  = tmp_out;
      node[tmp_in].in_fst          = node_current;
      node[tmp_out].out_fst        = node_current+1;

      // )...の処理
      regex_to_node_list(regex_str, regex_next, regex_end, node, &tmp_in, &tmp_out, node_empty, node_list_size);
      node[node_current+1].out_fst = tmp_in;
      node[tmp_in].in_fst = node_current+1;
      (*node_out) = tmp_out;

    // ')'の後続がない場合|...)の処理
    } else {
      regex_to_node_list(regex_str, regex_pipe+1, regex_end-1, node, &tmp_in, &tmp_out, node_empty, node_list_size);
      node[node_current  ].out_snd = tmp_in;
      node[node_current+1].in_snd  = tmp_out;
      node[tmp_in].in_fst          = node_current;
      node[tmp_out].out_fst        = node_current+1;

      (*node_out) = node_current+1;
    }
  }

}/*}}}*/
extern void initialize_node(NODE *node, const int node_list_size) {/*{{{*/
  for (int i=0; i<node_list_size; i++) {
    node[i].self         =  i;
    node[i].total        = -1;
    node[i].symbol       = '@';
    node[i].symbol_index = -1;
    node[i].is_magick    = true;
    node[i].in_fst       = -1;
    node[i].in_snd       = -1;
    node[i].out_fst      = -1;
    node[i].out_snd      = -1;
  }
}/*}}}*/
/*}}}*/
// デバッグ用関数/*{{{*/
static void debug_print_node_list(const NODE *node, const int node_size) {
  for (int i=0; i<node_size; i++) {
    NODE n = node[i];
    fprintf(stderr, "%3d %3d %c %3d %3d  %3d %3d  %3d %3d\n", i, node_size, n.symbol, n.symbol_index, n.is_magick, n.in_fst, n.in_snd, n.out_fst, n.out_snd);
  }
}

static void debug_print_regex_to_node_list_args(const char* regex_str, const int regex_begin, const int regex_end, const int regex_next, const int node_empty) {
  fprintf(stderr, "regex_begin:%3d[%c] regex_end:%3d[%c:%c] regex_next:%d[%c] node_empty=node_current:%d \n", regex_begin, regex_str[regex_begin], regex_end, regex_str[regex_end-1], (regex_str[regex_end]=='\0')?'N':regex_str[regex_end], regex_next, (regex_next == -1)?'-':regex_str[regex_next], node_empty);
}
/*}}}*/
// static関数の密輸関数/*{{{*/
extern void (*__static_node_func__(const char *func_name)) (void) {
  __REGISTER_STATIC_FUNCTION__(search_corresponding_paren);
  __REGISTER_STATIC_FUNCTION__(regex_to_node_list);
  __REGISTER_STATIC_FUNCTION__(debug_print_node_list);
  __REGISTER_STATIC_FUNCTION__(debug_print_regex_to_node_list_args);
  return NULL;
}/*}}}*/
