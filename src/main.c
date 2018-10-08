#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/node.h"
#include "../include/match.h"
#include "../include/print.h"

int main(int argc, const char **argv) {
  // コマンドラインから正規表現と比較文字列を取得
  if (argc != 3) {
    fprintf(stderr, "Invalid argc (%d)\n", argc);
    return 1;
  }
  const char  *regex_str = argv[1];
  const char  *match_str = argv[2];
  fprintf(stderr, "regex   : %s\n", regex_str);

  // 正規表現を簡約
  char simple_regex[200];
  int current = 0;
  simplify_regex(regex_str, 0, strlen(regex_str), simple_regex, &current, 200);
  simple_regex[current] = '\0';
  fprintf(stderr, "process : %s\n", simple_regex);

  // 簡約された正規表現をオートマトンに変換
  NODE node[100];
  initialize_node(node, 100);
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
  fprintf(stderr, "compare : %s\n", match_str);
  MATCH match[50];
  initialize_match(match, 50);
  const int step = match_all_str(match_str, node, match, 50);

  // マッチ結果をdotファイルに書き出し
  {
    FILE *fp;
    char *filename = "match.dot";
    if ((fp = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "Failed to open %s.\n", filename);
    }
    node_match_list_to_dot(fp, node, "12.0", "0.2", "#FF0000", "#FF0000", "#000000", match, step);
    fclose(fp);
  }

  // マッチの成否を表示
  if (step > 0) {
    fprintf(stderr, "match\n");
  } else {
    fprintf(stderr, "unmatch\n");
  }

  return 0;
}
