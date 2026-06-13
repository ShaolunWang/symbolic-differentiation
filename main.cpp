#include "fmt/base.h"
#include "runner.h"
#include <cstdlib>
#include <string>
#include "spdlog/spdlog.h"
#include <vector>

int main(int argc, char *argv[]) {
	
  spdlog::set_pattern("[%^%l%$] %v");
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
    fmt::println("Parser:");
    fmt::print("{}", Runner::run(example, name_ctx, graph_mode));
    fmt::println("Lowering");
    Runner::lower(example, name_ctx);

  } else {
    fmt::print("{}", Runner::transform(example, name_ctx, graph_mode));
  }

  return 0;
}
