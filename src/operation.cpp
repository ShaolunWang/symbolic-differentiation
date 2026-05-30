/**
 * @file This file contains functions on Operation
 * @brief
 *  - dump():
 *  	generated formatted expression with brackets around it
 *  - export_graphviz():
 *  	generate a dot output string for graphviz
 *  - make_RevOp():
 *  	this is where we pass in the rev operations into the
 * 		current node. it takes parent operations as input.
 * 	- getRevOp():
 * 		this is the operation that merges converging edges at the
 *    current node
 */

#include "fmt/base.h"
#include "fmt/format.h"
#include "operations.h"
std::string BinaryOp::dump() const {
  std::string result;
  result = fmt::format("{}", '(');
  result += lhs->dump();
  result += fmt::format(" {} ", op);
  result += rhs->dump();
  result += fmt::format("{}", ')');
  return result;
}

std::string RevOp::dump() const {
  std::string result;
  result = fmt::format("{} : \n", _name);
  for (const auto &x : operations) {
    result += x->dump();
    result += '\n';
  }
  return result;
}

std::string Variable::dump() const { return fmt::format("{}", name); }

std::string UnaryOp::dump() const {
  std::string result;
  result += fmt::format("{}", '(');
  result += operation->dump();
  result += fmt::format(" {} 2", op);
  result += fmt::format("{}", ')');
  return result;
}

/////////////////////////////////////////////////////

std::string BinaryOp::export_graphviz(const int &curr_index) const {
  std::string result;
  std::string lhs_export;
  std::string rhs_export;
  lhs_export = lhs->export_graphviz(curr_index);
  rhs_export = rhs->export_graphviz(curr_index);
  result += fmt::format("{}{}_{}", convert(op), curr_index,
                        std::hash<std::string>{}(_name));
  if (!lhs_export.empty()) {
    result += fmt::format(" -> {}", lhs_export);
  }

  if (result.back() != ';') {
    result += ";";
  }

  result += fmt::format("{}{}_{}", convert(op), curr_index,
                        std::hash<std::string>{}(_name));
  if (!rhs_export.empty()) {
    result += fmt::format(" -> {}", rhs_export);
  }
  if (result.back() != ';') {
    result += ";";
  }
  return result;
}
std::string UnaryOp::export_graphviz(const int &curr_index) const {
  std::string result;

  std::string operation_export;
  operation_export = operation->export_graphviz(curr_index);
  result += fmt::format("{}{}_{}", convert(op), curr_index,
                        std::hash<std::string>{}(_name));
  result += " 2";
  if (!operation_export.empty()) {
    result += fmt::format(" -> {}", operation_export);
  }
  if (result.back() != ';') {
    result += ";";
  }
  return result;
}

std::string Variable::export_graphviz(const int & /*curr_index*/) const {
  return fmt::format("{}", name);
}
std::string RevOp::export_graphviz(const int &current_idx) const {
  std::string result;
  if (_name.empty()) {
    fmt::print("{}", dump());
  }
  for (const auto &x : operations) {
    auto graph_dump = x->export_graphviz(current_idx);
    result += _name;
    result += "_VAR";
    result += fmt::format("-> {}", graph_dump);
  }
  if (result.back() != ';') {
    result += ";";
  }

  return result;
}
////////////////////////////////

std::vector<std::shared_ptr<Operation>> Operation::getRevOps() {

  if (revOp_worklist.size() > 2) {
    std::shared_ptr<Operation> result = revOp_worklist[0];
    for (int i = 1; i < revOp_worklist.size(); i++) {
      result = std::make_shared<BinaryOp>(result, '+', revOp_worklist[i]);
    }
    return {result};
  }

  return revOp_worklist;
}

void BinaryOp::makeRevOp(
    std::vector<std::shared_ptr<Operation>> &in_operation) {
  switch (op) {
  case '+':
    for (auto &i : in_operation) {
      // c
      revOp_worklist.push_back(i);
      // c
      revOp_worklist.push_back(i);
    }
    break;
  case '-':
    for (auto &i : in_operation) {
      // c
      revOp_worklist.push_back(i);
      auto zero = std::make_shared<Variable>("0");
      // -c
      revOp_worklist.push_back(std::make_shared<BinaryOp>(zero, '-', i));
    }
    break;
  case '*':
    for (auto &i : in_operation) {
      // c*b
      auto b = std::make_shared<Variable>("b");
      auto lhs = std::make_shared<BinaryOp>(i, '*', b);
      revOp_worklist.push_back(lhs);

      // c*a
      auto a = std::make_shared<Variable>("a");
      auto rhs = std::make_shared<BinaryOp>(i, '*', a);
      revOp_worklist.push_back(rhs);
    }
    break;
  case '/':
    for (auto &i : in_operation) {
      // c / b
      auto a = std::make_shared<Variable>("a");
      auto b = std::make_shared<Variable>("b");
      auto lhs = std::make_shared<BinaryOp>(i, '/', b);
      revOp_worklist.push_back(lhs);

      auto zero = std::make_shared<Variable>("0");
      // –c * a / b^2
      auto sq = std::make_shared<UnaryOp>(b, '^');
      auto div = std::make_shared<BinaryOp>(a, '/', sq);
      auto mul = std::make_shared<BinaryOp>(i, '*', div);
      auto rhs = std::make_shared<BinaryOp>(zero, '-', mul);
      // fmt::println("{}", rhs->dump());
      revOp_worklist.push_back(rhs);
    }
    break;
  default:
    break;
  }
};
void UnaryOp::makeRevOp(std::vector<std::shared_ptr<Operation>> &in_operation) {

  switch (op) {
  case '^':
    for (auto &i : in_operation) {
      auto a = std::make_shared<Variable>("a");
      auto two = std::make_shared<Variable>("2");
      auto mul = std::make_shared<BinaryOp>(i, '*', a);
      mul = std::make_shared<BinaryOp>(two, '*', mul);
      revOp_worklist.push_back(mul);
    }
    break;
  default:
    break;
  }
}
void Variable::makeRevOp(
    std::vector<std::shared_ptr<Operation>> &in_operation) {

  for (auto &i : in_operation) {
    revOp_worklist.push_back(i);
  }
}
