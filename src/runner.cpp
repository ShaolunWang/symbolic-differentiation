#include "runner.h"
#include "fmt/color.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
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
	fmt::println("Lowering Result: ");
  for (int i = 0; i < list.size(); i++) {
    std::string str = list[i]->_name;
    fmt::print(fmt::emphasis::bold, "%x{}", i);
    fmt::println(" = {}", str);
  }
}
