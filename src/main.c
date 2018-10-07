#include <stdio.h>
#include <string.h>
#include "../include/node.h"
#include "../include/match.h"
#include "../include/print.h"

int main(void) {
  // const char *original_regex = "hoge";
  // const char *original_regex = "ho*g*e*";
  // const char *original_regex = "h(o|g)e";
  // const char *original_regex = "a((b|c)|d)e";
  // const char *original_regex = "a((bc*d(e|f*g)*|hi*j)|kl*m)*no*p";
  // const char *original_regex = "a((b|c)d|(e|f)g)h";
  // const char *original_regex = "a((b*|c)|d)*e";
  // const char *original_regex = "a(@|b)*c";
  // const char *original_regex = "a\\(b\\)c";
  const char *original_regex = "a(b|c|d|e)f";

  char simple_regex[200];
  int current = 0;
  simplify_regex( original_regex, 0, strlen(original_regex), simple_regex, &current, 200);
  simple_regex[current] = '\0';

  NODE node[100];
  initialize_node(node, 100);
  regex_to_all_node(simple_regex, node, 100);

  const int topic_node_list[100] = {0};
  node_list_to_dot( stdout, node, topic_node_list, "12.0", "0.2", "#FF0000", "#FF0000", "#000000");

  // MATCH match[50];
  // initialize_match(match, 50);
  // const int step = match_all_str( "a\\c", node, match, 50);

  // node_match_list_to_dot( stdout, node, "12.0", "0.2", "#FF0000", "#FF0000", "#000000", match, step);

  return 0;
}
