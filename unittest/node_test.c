#include <stdio.h>
#include "../include/node.h"

int main(int argc, const char **argv) {
  int  (*search_corresponding_paren)(const char*, const int, const int) = (int  (*)(const char*, const int, const int)) __static_node_func__("search_corresponding_paren");
  int a = search_corresponding_paren("hoge(piyo)fuga", 4, 13);
  fprintf(stderr, "%d\n", a);

  // int  (*search_inner_letter)(const char*, const int, const char, const int) = (int  (*)(const char*, const int, const char, const int)) __static_node_func__("search_inner_letter");
  // int  (*search_next_char_index)(const char*, const int, const int) = (int  (*)(const char*, const int, const int)) __static_node_func__("search_next_char_index");
  // bool (*is_magick)(const char) = (bool (*)(const char)) __static_node_func__("is_magick");
  // void (*debug_print_node_list)(const NODE*, const int) = (void (*)(const NODE*, const int)) __static_node_func__("debug_print_node_list");
  // void (*debug_print_regex_to_node_list_args)(const char *, const int, const int, const int, const int) = (void (*)(const char *, const int, const int, const int, const int)) __static_node_func__("debug_print_regex_to_node_list_args");
  // void (*regex_to_node_list)(const char *, const int, const int, NODE*, int*, int*, int*, const int) = (void (*)(const char *, const int, const int, NODE*, int*, int*, int*, const int)) __static_node_func__("regex_to_node_list");
  return 0;
}

