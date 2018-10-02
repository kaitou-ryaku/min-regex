#ifndef __COMMON__
#define __COMMON__

#include <stdbool.h>

typedef struct {
  int  self;         // この構造体配列における、このノードの番号。
  int  total;        // この構造体配列のサイズ。
  char symbol;       // 正規表現の記号列に含まれる、対応する記号。
  int  symbol_index; // 対応する記号の配列番号。
  bool is_magick;    // 記号が役文字ならtrue、文字ならfalse。
  int  in_fst;       // 1番目の入力ノードの番号。最初のノードなら-1。他は0以上。
  int  in_snd;       // 2番目の入力ノードの番号。集結点でなければ-1。
  int  out_fst;      // 1番目の出力ノードの番号。最後のノードなら-1。他は0以上。
  int  out_snd;      // 2番目の出力ノードの番号。分岐点でなければ-1。
} NODE;

typedef struct {
  int step;           // オートマトン遍歴のステップ数。つまりこのノードの番号
  int node_index;     // オートマトンの添字番号。
  int str_index;      // 文字列の添字番号。
} MATCH;

#endif
