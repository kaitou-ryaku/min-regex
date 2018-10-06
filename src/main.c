#include <stdio.h>
#include "../include/node.h"
#include "../include/match.h"
#include "../include/print.h"

int main(void) {
  // const char *regex_str = "hoge";
  // const char *regex_str = "ho*g*e*";
  // const char *regex_str = "h(o|g)e";
  // const char *regex_str = "a((b|c)|d)e";
  // const char *regex_str = "a((bc*d(e|f*g)*|hi*j)|kl*m)*no*p";
  // const char *regex_str = "a((b|c)d|(e|f)g)h";
  const char *regex_str = "a((b*|c)|d)*e";

  NODE node[100];
  initialize_node(node, 100);
  regex_to_all_node(regex_str, node, 100);

  // const int topic_node_list[100] = {0};
  // node_list_to_dot( stdout, node, topic_node_list, "12.0", "0.2", "#FF0000", "#FF0000", "#000000");

  MATCH match[50];
  initialize_match(match, 50);
  const int step = match_all_str( "abbcde", node, match, 50);

  node_match_list_to_dot( stdout, node, "12.0", "0.2", "#FF0000", "#FF0000", "#000000", match, step);

  return 0;
}
