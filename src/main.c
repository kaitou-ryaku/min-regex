#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/min-regex.h"

int main(int argc, const char **argv) {
  // コマンドラインから正規表現と比較文字列を取得
  if (argc != 3) {
    fprintf(stderr, "Invalid argc (%d)\n", argc);
    return 1;
  }
  const char  *regex_str = argv[1];
  const char  *match_str = argv[2];
  fprintf(stderr, "regex    : %s\n", regex_str);

  // 正規表現を簡約
  char simple_regex[200];
  simplify_regex(regex_str, simple_regex, 200);
  fprintf(stderr, "simplify : %s\n", simple_regex);

  // 簡約された正規表現をオートマトンに変換
  MIN_REGEX_NODE node[100];
  regex_to_all_node(simple_regex, node, 100);

  // オートマトンをdotファイルに書き出し
  {
    FILE *fp;
    char *filename = "regex.dot";
    if ((fp = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "Failed to open %s.\n", filename);
    }
    const int topic_node_list[100] = {0};
    node_list_to_dot(fp, node, topic_node_list, "12.0", "0.2", "#FF0000", "#FF0000", "#000000");
    fclose(fp);
  }

  // オートマトンと比較文字列をマッチング
  fprintf(stderr, "compare  : %s\n", match_str);
  MIN_REGEX_MATCH match[50];
  const int step = exact_match(match_str, node, match, 50);
  // const int step = forward_shortest_match( match_str, node, match, 50);
  // const int step = forward_longest_match(  match_str, node, match, 50);
  // const int step = backward_shortest_match(match_str, node, match, 50);
  // const int step = backward_longest_match( match_str, node, match, 50);

  // マッチ成功
  if (step > 0) {
    fprintf(stderr, "match    : ");
    print_match_list(stderr, node, match, step);
    fprintf(stderr, "\n");

    // マッチの様子をファイルに書き出し
    FILE *fp;
    char *filename = "match.dot";
    if ((fp = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "Failed to open %s.\n", filename);
    }
    node_match_list_to_dot(fp, node, "12.0", "0.2", "#FF0000", "#FF0000", "#000000", match, step);
    fclose(fp);

  // マッチ失敗
  } else {
    fprintf(stderr, "unmatch\n");
  }

  return 0;
}
