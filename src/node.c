#include "../include/node.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

// 関数プロトタイプ/*{{{*/
void regex_to_node_list(
  const char* regex_str
  , const int regex_begin
  , const int regex_end
  , NODE*     node
  , int*      node_in
  , int*      node_out
  , int*      node_empty
  , const int node_list_size
);
int search_corresponding_paren(const char* s, const int i, const int end);
int search_inner_letter(const char* s, const int i, const char c, const int end);
int search_next_char_index(const char *s, const int i, const int end);
bool is_magick(const char c);
/*}}}*/
// デバッグ用プロトタイプ/*{{{*/
void debug_print_node_list(const NODE *node, const int node_size);
void debug_print_regex_to_node_list_args(const char* regex_str, const int regex_begin, const int regex_end, const int regex_next, const int node_empty);
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
void regex_to_node_list(/*{{{*/
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

  debug_print_node_list(node, *node_empty);
  debug_print_regex_to_node_list_args(regex_str, regex_begin, regex_end, regex_next, *node_empty);

  // 次の文字*が存在する場合
  if ((regex_next > 0) && (regex_str[regex_next] == '*')) {
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

  // 次の文字が {'*'以外 or 存在しない} で、現在の文字が普通のアルファベットか空文字'@'の場合
  } else if (is_magick(regex_str[regex_begin]) == false){
    int regex_begin_next = regex_begin+1;
    if (current_char == '@') {
      node[node_current].symbol       = current_char;
      node[node_current].symbol_index = regex_begin;
      node[node_current].is_magick    = true;
    } else if (current_char == '\\') {
      assert(regex_begin_next < regex_end);
      const char next_char = regex_str[regex_begin+1];
      assert((next_char == '\\') || (next_char == '(') || (next_char == '|') || (next_char == ')') || (next_char == '*') || (next_char == '@'));
      node[node_current].symbol       = next_char;
      node[node_current].symbol_index = regex_begin+1;
      regex_begin_next++;
      node[node_current].is_magick    = false;
    } else {
      node[node_current].symbol       = current_char;
      node[node_current].symbol_index = regex_begin;
      node[node_current].is_magick    = false;
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
    node[node_current+1].symbol_index = regex_next-1;
    node[node_current+1].is_magick    = true;
    (*node_out) = node_current+1;
    (*node_empty)++;

    const int regex_pipe = search_inner_letter(regex_str, regex_begin, '|', regex_end);
    int tmp_in, tmp_out;
    regex_to_node_list(regex_str, regex_begin+1, regex_pipe, node, &tmp_in, &tmp_out, node_empty, node_list_size);
    node[node_current  ].out_fst = tmp_in;
    node[node_current+1].in_fst  = tmp_out;
    node[tmp_in].in_fst          = node_current;
    node[tmp_out].out_fst        = node_current+1;

    regex_to_node_list(regex_str, regex_pipe+1, regex_next-1, node, &tmp_in, &tmp_out, node_empty, node_list_size);
    node[node_current  ].out_snd = tmp_in;
    node[node_current+1].in_snd  = tmp_out;
    node[tmp_in].in_fst          = node_current;
    node[tmp_out].out_fst        = node_current+1;

    if (regex_next > 0) {
      regex_to_node_list(regex_str, regex_next, regex_end, node, &tmp_in, &tmp_out, node_empty, node_list_size);
      node[node_current+1].out_fst = tmp_in;
      node[tmp_in].in_fst = node_current+1;
      (*node_out) = tmp_out;

    } else {
      (*node_out) = node_current+1;
    }
  }

}/*}}}*/
int search_corresponding_paren(const char* s, const int i, const int end) {/*{{{*/
  assert(s[i]=='(');
  int j=i, count=0;
  while(j < end) {
    if (s[j] == '(') count++;
    if (s[j] == ')') count--;
    if (count == 0) break;
    j++;
  }

  if (count == 0) return j;
  else return -1;
}/*}}}*/
int search_inner_letter(const char* s, const int i, const char c, const int end) {/*{{{*/
  assert((s[i]=='(') || (s[i]==c));
  int j=i+1, count=0;
  while(j < end) {
    if (s[j] == '(') count++;
    if (s[j] == ')') count--;
    if (count == 0 && s[j] == c) break;
    j++;
  }

  if (s[j] == c) return j;
  else return -1;
}/*}}}*/
int search_next_char_index(const char *s, const int i, const int end) {/*{{{*/
  if (i >= end-1) return -1;

  if (s[i] == '(') {
    int ret = search_corresponding_paren(s, i, end);
    assert(ret > 0);
    if (ret+1 < end) return ret+1;
    else return -1;
  }

  if ((is_magick(s[i]) == false) || s[i] == '*') {
    return i+1;
  } else {
    assert(0);
  }
}/*}}}*/
bool is_magick(const char c) {/*{{{*/
  if ((c == '(') | (c == '|') | (c == ')') | (c == '*')) return true;
  else return false;
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
extern void simplify_regex( const char* original_regex, const int begin, const int end, char *simple_regex, int* current, const int size) {/*{{{*/
  const char c = original_regex[begin];
  if (c == '(') {
    // (a|b|c|d) --> (a|(b|(c|d)))

    // (a| -> (a|
    int pipe = search_inner_letter(original_regex, begin, '|', end);
    assert(pipe > 0);
    simple_regex[(*current)] = '(';
    (*current)++;
    simplify_regex(original_regex, begin+1, pipe, simple_regex, current ,size);
    simple_regex[(*current)] = '|';
    (*current)++;

    // |b| -> (b|
    int next_pipe = search_inner_letter(original_regex, pipe, '|', end);
    while (next_pipe > 0) {
      assert(pipe+1 < next_pipe);
      simple_regex[(*current)] = '(';
      (*current)++;
      simplify_regex(original_regex, pipe+1, next_pipe, simple_regex, current ,size);
      simple_regex[(*current)] = '|';
      (*current)++;
      pipe      = next_pipe;
      next_pipe = search_inner_letter(original_regex, next_pipe, '|', end);
    }

    // |d) -> d
    const int end_pipe = search_corresponding_paren(original_regex, begin, end);
    simplify_regex(original_regex, pipe+1, end_pipe, simple_regex, current ,size);

    // 最後に'|'の数だけ')'を書きまくる
    pipe = search_inner_letter(original_regex, begin, '|', end);
    while (pipe > 0) {
      simple_regex[(*current)] = ')';
      (*current)++;
      pipe = search_inner_letter(original_regex, pipe, '|', end);
    }

    // 分岐の続き
    if (end_pipe+1 < end) {
      simplify_regex( original_regex, end_pipe+1, end, simple_regex, current ,size);
    }

  } else if (c == '\\') {
    // エスケープシーケンス
    simple_regex[(*current)  ] = '\\';
    simple_regex[(*current)+1] = original_regex[begin+1];
    (*current) = (*current) + 2;
    if (begin+2 < end) {
      simplify_regex( original_regex, begin+2, end, simple_regex, current ,size);
    }

  } else {
    // 処理の不要な普通の文字
    simple_regex[(*current)] = c;
    (*current)++;
    if (begin+1 < end) {
      simplify_regex( original_regex, begin+1, end, simple_regex, current ,size);
    }
  }
}/*}}}*/
/*}}}*/
// デバッグ用関数/*{{{*/
void debug_print_node_list(const NODE *node, const int node_size) {
  for (int i=0; i<node_size; i++) {
    NODE n = node[i];
    fprintf(stderr, "%3d %3d %c %3d %3d  %3d %3d  %3d %3d\n", i, node_size, n.symbol, n.symbol_index, n.is_magick, n.in_fst, n.in_snd, n.out_fst, n.out_snd);
  }
}

void debug_print_regex_to_node_list_args(const char* regex_str, const int regex_begin, const int regex_end, const int regex_next, const int node_empty) {
  fprintf(stderr, "regex_begin:%3d[%c] regex_end:%3d[%c:%c] regex_next:%d[%c] node_empty=node_current:%d \n", regex_begin, regex_str[regex_begin], regex_end, regex_str[regex_end-1], (regex_str[regex_end]=='\0')?'N':regex_str[regex_end], regex_next, (regex_next == -1)?'-':regex_str[regex_next], node_empty);
}
/*}}}*/
