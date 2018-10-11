#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/node.h"
#include "../include/match.h"
#include "../include/print.h"

int correct  = 0;
int question = 0;

static bool is_match(const char* regex_str, const char* match_str);
static void is_valid(const char* regex_str, const char* match_str, const bool answer);
// マッチ関数/*{{{*/
static bool is_match(const char* regex_str, const char* match_str) {
  char simple_regex[200];
  int current = 0;
  simplify_regex(regex_str, 0, strlen(regex_str), simple_regex, &current, 200);
  simple_regex[current] = '\0';
  NODE node[200];
  initialize_node(node, 200);
  regex_to_all_node(simple_regex, node, 200);
  MATCH match[200];
  initialize_match(match, 200);
  const int step = match_all_str(match_str, node, match, 200);

  bool ret;
  if (step > 0) {
    ret = true;
  } else {
    ret = false;
  }

  return ret;
}
/*}}}*/
// チェック関数/*{{{*/
static void is_valid(const char* regex_str, const char* match_str, const bool answer) {
  question++;
  bool trial = is_match(regex_str, match_str);
  if (trial == answer) {
    fprintf(stderr, "%04d [\x1b[32mO\x1b[39m] %15s       \"%s\"\n"                , question, regex_str, match_str);
    correct++;
  }
  if (trial != answer) {
    fprintf(stderr, "%04d [\x1b[31mX\x1b[39m] \x1b[41m%15s       \"%s\"\x1b[49m\n", question, regex_str, match_str);
  }
}/*}}}*/

int main(void) {
  is_valid(""       , ""           , false);
  is_valid("@"      , ""           , true );
  is_valid("."      , ""           , false);
  is_valid("a"      , ""           , false);
  is_valid("(@|@)"  , ""           , true );
  is_valid("(.|@)"  , ""           , true );
  is_valid("(@|.)"  , ""           , true );
  is_valid("(.|.)"  , ""           , false);
  is_valid("(a|@)"  , ""           , true );
  is_valid("(@|a)"  , ""           , true );
  is_valid("(a|a)"  , ""           , false);
  is_valid("(.|a)"  , ""           , false);
  is_valid("(a|.)"  , ""           , false);

  is_valid("@*"     , ""           , true );
  is_valid(".*"     , ""           , false);
  is_valid("a*"     , ""           , false);
  is_valid("(@|@)*" , ""           , true );
  is_valid("(.|@)*" , ""           , true );
  is_valid("(@|.)*" , ""           , true );
  is_valid("(.|.)*" , ""           , false);
  is_valid("(a|@)*" , ""           , true );
  is_valid("(@|a)*" , ""           , true );
  is_valid("(a|a)*" , ""           , false);
  is_valid("(.|a)*" , ""           , false);
  is_valid("(a|.)*" , ""           , false);

  is_valid("@@"     , ""           , true );
  is_valid("@."     , ""           , false);
  is_valid(".@"     , ""           , false);
  is_valid("@a"     , ""           , false);
  is_valid("a@"     , ""           , false);
  is_valid(".a"     , ""           , false);
  is_valid("a."     , ""           , false);
  is_valid(".."     , ""           , false);
  is_valid("aa"     , ""           , false);
  is_valid("ab"     , ""           , false);

  is_valid("@*@"    , ""           , true );
  is_valid("@*."    , ""           , false);
  is_valid(".*@"    , ""           , true );
  is_valid("@*a"    , ""           , false);
  is_valid("a*@"    , ""           , true );
  //is_valid(".*a"    , ""           , false);
  is_valid("a*."    , ""           , false);
  //is_valid(".*."    , ""           , false);
  is_valid("a*a"    , ""           , false);
  is_valid("a*b"    , ""           , false);

  is_valid("@@*"    , ""           , true );
  is_valid("@.*"    , ""           , true );
  is_valid(".@*"    , ""           , false);
  is_valid("@a*"    , ""           , true );
  is_valid("a@*"    , ""           , false);
  is_valid(".a*"    , ""           , false);
  //is_valid("a.*"    , ""           , false);
  //is_valid("..*"    , ""           , false);
  is_valid("aa*"    , ""           , false);
  is_valid("ab*"    , ""           , false);

  is_valid("@*@*"   , ""           , true );
  is_valid("@*.*"   , ""           , true );
  is_valid(".*@*"   , ""           , true );
  is_valid("@*a*"   , ""           , true );
  is_valid("a*@*"   , ""           , true );
  is_valid(".*a*"   , ""           , true );
  is_valid("a*.*"   , ""           , true );
  is_valid(".*.*"   , ""           , true );
  is_valid("a*a*"   , ""           , true );
  is_valid("a*b*"   , ""           , true );

  fprintf(stderr, "STATISTICS: [%d/%d] ARE PASSED\n", correct, question);
  return 0;
}
