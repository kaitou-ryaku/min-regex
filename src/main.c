#include <stdio.h>
#include "../include/node.h"
#include "../include/match.h"

int main(void) {
  // const char *regex_str = "hoge";
  // const char *regex_str = "ho*g*e*";
  // const char *regex_str = "h(o|g)e";
  // const char *regex_str = "a((b|c)|d)e";
  // const char *regex_str = "a((bc*d(e|f*g)*|hi*j)|kl*m)*no*p";
  const char *regex_str = "a(b(c|d)|(e|f)g)h";

  NODE node[100];
  initialize_node(node, 100);
  regex_to_all_node(regex_str, node, 100);

  node_list_to_dot( stdout, node, -1, "12.0", "0.2", "#FF0000", "#FF0000", "#000000");

  return 0;
}
