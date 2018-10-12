#include "../include/textutil.h"
#include <string.h>
#include <assert.h>

extern void simplify_regex( const char* original_regex, const int begin, const int end, char *simple_regex, int* current, const int size) {/*{{{*/
  const char c = original_regex[begin];
  if (c == '(') {
    // (a|b|c|d) --> (a|(b|(c|d)))

    // (a| -> (a|
    const int end_pipe = search_corresponding_paren(original_regex, begin, end);
    int pipe = search_inner_letter(original_regex, begin, '|', end_pipe);
    assert(pipe > 0);
    simple_regex[(*current)] = '(';
    (*current)++;
    simplify_regex(original_regex, begin+1, pipe, simple_regex, current ,size);
    simple_regex[(*current)] = '|';
    (*current)++;

    // |b| -> (b|
    int next_pipe = search_inner_letter(original_regex, pipe, '|', end_pipe);
    while (next_pipe > 0) {
      assert(pipe+1 < next_pipe);
      simple_regex[(*current)] = '(';
      (*current)++;
      simplify_regex(original_regex, pipe+1, next_pipe, simple_regex, current ,size);
      simple_regex[(*current)] = '|';
      (*current)++;
      pipe      = next_pipe;
      next_pipe = search_inner_letter(original_regex, next_pipe, '|', end_pipe);
    }

    // |d) -> d
    simplify_regex(original_regex, pipe+1, end_pipe, simple_regex, current ,size);

    // 最後に'|'の数だけ')'を書きまくる
    pipe = search_inner_letter(original_regex, begin, '|', end_pipe);
    while (pipe > 0) {
      simple_regex[(*current)] = ')';
      (*current)++;
      pipe = search_inner_letter(original_regex, pipe, '|', end_pipe);
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
