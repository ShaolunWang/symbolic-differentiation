#include "fmt/base.h"
#include "ir.h"
#include "parser.h"
#include <sstream>
#include <string>
#include <vector>
class Runner {
public:
  static std::vector<std::string> simple_lexer(const std::string &input);
  /**
   * @brief Runner that evaluate the graph to some outputs given the input
   * @param input string type input
   * @param in_ctx variables being used
   */
  static std::string run(const std::string &input,
                         std::vector<std::string> &in_ctx,
                         bool export_graph = false);

  static std::string transform(const std::string &input,
                               std::vector<std::string> &in_ctx,
                               bool export_graph = false);
  static void lower(const std::string &input,
                    const std::vector<std::string> &in_ctx);
};
