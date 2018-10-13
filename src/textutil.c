#include "../include/textutil.h"
#include <stdbool.h>
#include <assert.h>

extern int search_corresponding_paren(const char* s, const int i, const int end) {/*{{{*/
  assert(s[i]=='(');
  int j=i, count=0;
  while(j < end) {
    if (s[j] == '\\') {
      j=j+2;
      continue;
    }
    if (s[j] == '(') count++;
    if (s[j] == ')') count--;
    if (count == 0) break;
    j++;
  }

  if (count == 0) return j;
  else return -1;
}/*}}}*/
extern int search_inner_letter(const char* s, const int i, const char c, const int end) {/*{{{*/
  assert((s[i]=='(') || (s[i]==c));
  int j=i+1, count=0;
  while(j < end) {
    if (s[j] == '\\') {
      j=j+2;
      continue;
    }
    if (s[j] == '(') count++;
    if (s[j] == ')') count--;
    if (count == 0 && s[j] == c) break;
    j++;
  }

  if (s[j] == c) return j;
  else return -1;
}/*}}}*/
extern bool is_magick(const char c) {/*{{{*/
  if ((c == '(') || (c == '|') || (c == ')') || (c == '*') ||  (c == '@') || (c == '\\')) return true;
  else return false;
}/*}}}*/
