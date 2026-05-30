#include "fmt/base.h"
#include "include/parser.h"
#include <sstream>

static std::vector<std::string> simple_lexer(const std::string &input) {
  std::vector<std::string> tokens;
  std::istringstream iss(input);
  std::string token;
  while (iss >> token) {
    // remove all spaces
    if (token != " ") {
      tokens.push_back(token);
    }
  }
  return tokens;
}
/**
 * @brief Runner that evaluate the graph to some outputs given the input
 * @param input string type input
 * @param in_ctx variables being used
 */
std::string run(const std::string &input, std::vector<std::string> &in_ctx,
                bool export_graph = false) {
  Parser parser{simple_lexer(input), in_ctx};
  parser.parse_main();

  if (export_graph) {
    return parser.export_graphviz();
  }
  return parser.dump();
}

std::string transform(const std::string &input,
                      std::vector<std::string> &in_ctx,
                      bool export_graph = false) {
  Parser parser{simple_lexer(input), in_ctx};
  parser.parse_main();
  parser.gen_rev();

  if (export_graph) {
    return parser.export_rev_graphviz();
  }
  return parser.dump_rev();
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fmt::println("`ad 0 0` to generate forward mode, without graphviz output ",
                 "`ad 0 1` to generate forward mode, with graphviz output ");
    fmt::println("`ad 1 0` to generate reverse mode, without graphviz output",
                 "`ad 1 1` to generate reverse mode, with graphviz output ");
    return -1;
  }
  int mode = atoi(argv[1]);
  int graph = atoi(argv[2]);
  bool forward_mode;
  bool graph_mode;
  if (mode == 1 || mode == 0) {
    forward_mode = mode == 0;
  } else {
    fmt::println("`ad 0 0` to generate forward mode, without graphviz output\n "
                 "`ad 0 1` to generate forward mode, with graphviz output ");
    fmt::println("`ad 1 0` to generate reverse mode, without graphviz output",
                 "`ad 1 1` to generate reverse mode, with graphviz output");
    return -1;
  }

  if (graph == 1 || graph == 0) {
    graph_mode = graph == 1;
  } else {
    fmt::println("`ad 0 0` to generate forward mode, without graphviz output\n "
                 "`ad 0 1` to generate forward mode, with graphviz output ");
    fmt::println("`ad 1 0` to generate reverse mode, without graphviz output",
                 "`ad 1 1` to generate reverse mode, with graphviz output");
    return -1;
  }
  std::string example{
      "x , y = a * b - a / c , a * ( b + c )",
  };
  std::vector<std::string> name_ctx{"a", "b", "c"};
  if (forward_mode) {
    fmt::print("{}", run(example, name_ctx, graph_mode));
  } else {
    fmt::print("{}", transform(example, name_ctx, graph_mode));
  }

  return 0;
}
