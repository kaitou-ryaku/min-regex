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
static int get_next_token(const char* regex_str, const int regex_begin, const int regex_end);
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
  // [regex_begin = token_begin] 最初の1個のトークン [token_end] [token_end+1] 2番目のトークン ... n番目のトークン ... [regex_end]
  // 上記の右端から左端の全体を解析し、入口のノード添字を*node_in、出口のノード添字を*node_outとして返す

  assert(*node_empty < node_list_size);
  const int token_begin  = regex_begin;
  const int token_end    = get_next_token(regex_str, regex_begin, regex_end);

  // 全トークンの解析が完了している場合
  if (token_begin >= token_end) return;

  const int c_begin = regex_str[token_begin]; // この1個のトークンの先頭文字
  const int c_end   = regex_str[token_end-1]; // この1個のトークンの末尾文字
  const int node_begin = (*node_empty);       // この1個のトークンの先頭ノード
  int       node_end;                         // この1個のトークンの末尾ノード

  assert(c_begin != ')');
  assert(c_begin != '|');
  assert(c_begin != '*');

  // \\* or (...)* or \\@* or \\** or a*/*{{{*/
  if (c_end == '*') {
    // \\* (アスタリスクをエスケープしたやつの場合)
    if (c_begin == '\\' && token_begin+2 == token_end) {
      node[node_begin].symbol       = '*';
      node[node_begin].symbol_index = token_end-1;
      node[node_begin].is_magick    = false;
      (*node_in) = node_begin;
      node_end = node_begin;
      (*node_empty)++;

    // ..* (繰り返しの場合)
    } else {
      // このトークン内の繰り返しの共通処理として、最初のノード(node_begin)を*にする
      // このトークンの末尾ノード(node_end)も*になり、後続のノードにつながる
      node[node_begin].symbol       = '*';
      node[node_begin].symbol_index = token_end-1;
      node[node_begin].is_magick    = true;
      (*node_in) = node_begin;
      node_end = node_begin;
      (*node_empty)++;

      // 繰り返しの中身の処理
      int tmp_in, tmp_out;
      regex_to_node_list(regex_str, token_begin, token_end-1, node, &tmp_in, &tmp_out, node_empty, node_list_size);
      node[node_begin].in_snd  = tmp_out;    // 繰り返すやつ -> *
      node[tmp_out].out_fst    = node_begin; // 繰り返すやつ -> *
      node[node_begin].out_snd = tmp_in;     // * -> 繰り返すやつ
      node[tmp_in ].in_fst     = node_begin; // * -> 繰り返すやつ
    }
  }/*}}}*/
  // (...) or \\@ or a (繰り返さない場合)/*{{{*/
  else {

    // (...)
    if (c_begin == '(') {
      assert(c_end == ')');

      node[node_begin  ].symbol       = '(';
      node[node_begin  ].symbol_index = token_begin;
      node[node_begin  ].is_magick    = true;
      (*node_in) = node_begin;
      (*node_empty)++;

      node[node_begin+1].symbol       = ')';
      node[node_begin+1].symbol_index = token_end-1;
      node[node_begin+1].is_magick    = true;
      node_end = node_begin+1;
      (*node_empty)++;

      const int regex_pipe = search_inner_letter(regex_str, regex_begin, '|', regex_end);
      int tmp_in, tmp_out;

      // (...|の処理
      regex_to_node_list(regex_str, token_begin+1, regex_pipe, node, &tmp_in, &tmp_out, node_empty, node_list_size);
      node[node_begin  ].out_fst = tmp_in;
      node[node_begin+1].in_fst  = tmp_out;
      node[tmp_in].in_fst        = node_begin;
      node[tmp_out].out_fst      = node_begin+1;

      // |...)の処理
      regex_to_node_list(regex_str, regex_pipe+1, token_end-1, node, &tmp_in, &tmp_out, node_empty, node_list_size);
      node[node_begin  ].out_snd = tmp_in;
      node[node_begin+1].in_snd  = tmp_out;
      node[tmp_in].in_fst        = node_begin;
      node[tmp_out].out_fst      = node_begin+1;

    // \\@ or \\*
    } else if (c_begin == '\\') {
      assert(token_begin+2 == token_end);
      char symbol = regex_str[token_begin+1];
      assert((symbol == '(') || (symbol == '|') || (symbol == ')') || (symbol == '*') || (symbol == '@') || (symbol == '\\'));

      node[node_begin].symbol       = symbol;
      node[node_begin].symbol_index = token_begin+1;
      node[node_begin].is_magick    = false;
      (*node_in) = node_begin;
      node_end = node_begin;
      (*node_empty)++;

    // @
    } else if (c_begin == '@'){
      node[node_begin].symbol       = '@';
      node[node_begin].symbol_index = token_begin;
      node[node_begin].is_magick    = true;
      (*node_in) = node_begin;
      node_end = node_begin;
      (*node_empty)++;

    // a
    } else {
      node[node_begin].symbol       = c_begin;
      node[node_begin].symbol_index = token_begin;
      node[node_begin].is_magick    = false;
      (*node_in) = node_begin;
      node_end = node_begin;
      (*node_empty)++;
    }
  }/*}}}*/
  // 後続トークンの処理/*{{{*/
  if (token_end < regex_end) {
    int next_token_in, next_token_out;
    regex_to_node_list(regex_str, token_end, regex_end, node, &next_token_in, &next_token_out, node_empty, node_list_size);
    node[node_end].out_fst = next_token_in;
    node[next_token_in].in_fst = node_end;
    (*node_out) = next_token_out;

  } else {
    (*node_out) = node_end;
  }/*}}}*/
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
static int get_next_token(const char* regex_str, const int regex_begin, const int regex_end) {/*{{{*/
  char c = regex_str[regex_begin];
  int ret = 0;

  if (regex_begin == regex_end) {
    ret = regex_end;

  } else if ((c == '|') || (c == ')') || (c == '*')) {
    assert(0);

  } else if (c == '(') {
    // (a|b) --> )
    ret = search_corresponding_paren(regex_str, regex_begin, regex_end);
    assert(ret > 0);
    assert(ret < regex_end);

  } else if (c == '\\') {
    // \\@ -> @
    assert(regex_begin + 1 < regex_end);
    ret = regex_begin+1;

  } else {
    // a -> @
    ret = regex_begin;
  }

  // 次の文字が*なら、それも含める
  if ((ret+1 < regex_end) && (regex_str[ret+1] == '*')) ret++;

  return ret+1;
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
