#include "runner.h"
std::vector<std::string> Runner::simple_lexer(const std::string &input) {
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
std::string Runner::run(const std::string &input,
                        std::vector<std::string> &in_ctx, bool export_graph) {
  Parser parser{simple_lexer(input), in_ctx};
  parser.parseMain();

  if (export_graph) {
    return parser.exportGraphviz();
  }
  return parser.dump();
}
std::string Runner::transform(const std::string &input,
                              std::vector<std::string> &in_ctx,
                              bool export_graph) {
  Parser parser{simple_lexer(input), in_ctx};
  parser.parseMain();
  parser.genRev();

  if (export_graph) {
    return parser.exportRevGraphviz();
  }
  return parser.dumpRev();
}
void Runner::lower(const std::string &input,
                   const std::vector<std::string> &in_ctx) {
  Parser parser{simple_lexer(input), in_ctx};
  parser.parseMain();
  IRLowering lowerPass{parser.getOpList()};
  lowerPass.flatten();
  const std::vector<std::shared_ptr<Operation>> &list = lowerPass.getOpList();
  for (int i = 0; i < list.size(); i++) {
    std::string str = list[i]->dump();
    fmt::println("%x{} = {}", i, str);
  }
}
