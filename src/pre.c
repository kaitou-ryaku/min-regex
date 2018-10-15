#include "../include/textutil.h"
#include <string.h>
#include <assert.h>

static void simplify_regex_partial( const char* original_regex, const int begin, const int end, char *simple_regex, int* current, const int size);
static void simplify_regex_partial( const char* original_regex, const int begin, const int end, char *simple_regex, int* current, const int size) {/*{{{*/
  const char c = original_regex[begin];
  // (...???...)/*{{{*/
  if (c == '(') {
    // (a)       --> (a)
    // (a|b)     --> (a|b)
    // (a|b|c|d) --> (a|(b|(c|d)))
    const int end_pipe  = search_corresponding_paren(original_regex, begin, end);
    int next_pipe = search_inner_letter(original_regex, begin, '|', end_pipe);

    // (...)/*{{{*/
    if (next_pipe < 0) {
      simple_regex[(*current)] = '(';
      (*current)++;
      simplify_regex_partial(original_regex, begin+1, end_pipe, simple_regex, current ,size);
      simple_regex[(*current)] = ')';
      (*current)++;
    }/*}}}*/
    // (...|...) or (...|...|...)/*{{{*/
    else {
      // (a| -> (a| ループ1周目
      // |b| -> (b| ループ2周目以降
      int pipe = begin; // ループの一周目では(を指す
      while (next_pipe > 0) {
        assert(pipe+1 < next_pipe);
        simple_regex[(*current)] = '(';
        (*current)++;
        simplify_regex_partial(original_regex, pipe+1, next_pipe, simple_regex, current ,size);
        simple_regex[(*current)] = '|';
        (*current)++;
        pipe      = next_pipe;
        next_pipe = search_inner_letter(original_regex, next_pipe, '|', end_pipe);
      }

      // |d) -> d
      simplify_regex_partial(original_regex, pipe+1, end_pipe, simple_regex, current ,size);

      // 最後にパイプの数だけ)を書く
      pipe = search_inner_letter(original_regex, begin, '|', end_pipe);
      while (pipe > 0) {
        simple_regex[(*current)] = ')';
        (*current)++;
        pipe = search_inner_letter(original_regex, pipe, '|', end_pipe);
      }
    }/*}}}*/
    // (...???...)の続き/*{{{*/
    if (end_pipe+1 < end) {
      simplify_regex_partial( original_regex, end_pipe+1, end, simple_regex, current ,size);
    }/*}}}*/
  }/*}}}*/
  // エスケープシーケンス ... この処理が無いと、メタ文字の(とエスケープされた(が区別できない/*{{{*/
  else if (c == '\\') {
    simple_regex[(*current)  ] = '\\';
    simple_regex[(*current)+1] = original_regex[begin+1];
    (*current) = (*current) + 2;
    if (begin+2 < end) {
      simplify_regex_partial( original_regex, begin+2, end, simple_regex, current ,size);
    }
  }/*}}}*/
  // 処理の不要な普通の文字/*{{{*/
  else {
    simple_regex[(*current)] = c;
    (*current)++;
    if (begin+1 < end) {
      simplify_regex_partial( original_regex, begin+1, end, simple_regex, current ,size);
    }
  }/*}}}*/
}/*}}}*/
extern void simplify_regex( const char* original_regex, char *simple_regex, const int size) {/*{{{*/
  int current = 0;
  simplify_regex_partial(original_regex, 0, strlen(original_regex), simple_regex, &current, size);
  assert(current < size);
  simple_regex[current] = '\0';
}/*}}}*/
