#pragma once
#include "operations.h"
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <vector>
class Parser {

public:
  Parser(const Parser &) = delete;
  Parser(Parser &&) = delete;
  Parser &operator=(const Parser &) = delete;
  Parser &operator=(Parser &&) = delete;

  Parser(const std::vector<std::string> &input,
         const std::vector<std::string> &free_var)
      : name_ctx{free_var}, input{input} {};
  void parse_main();
  void gen_rev();
  std::string dump() const;
  std::string dump_rev() const;

  std::string export_graphviz();
  std::string export_rev_graphviz();
  std::vector<std::shared_ptr<Operation>> operation_list;
  std::unordered_map<std::string, std::shared_ptr<Operation>> operation_ctx;

  std::vector<std::shared_ptr<Operation>> revOp_worklist;
  std::unordered_map<std::string, std::shared_ptr<Operation>> rev_map;

private:
  bool check(Token type);
  bool match(Token type);
  std::shared_ptr<Operation> parse_expr();
  std::shared_ptr<Operation> parse_unary();
  std::shared_ptr<Operation> parse_mul_div_expr();
  std::shared_ptr<Operation> parse_div_expr();
  std::shared_ptr<Operation> parse_plus_minus_expr();
  std::shared_ptr<Operation> parse_minus_expr();
  std::shared_ptr<Operation> parse_variable();
  static bool isVarName(const std::string &s) {
    return std::ranges::all_of(s, [](char c) { return std::isalnum(c); });
  }
  static bool isNum(const std::string &s) {
    return std::ranges::all_of(s, [](char c) { return std::isdigit(c); });
  }

  std::vector<std::string> name_ctx;
  std::vector<std::string> out_ctx;
  std::vector<std::string> input;
  std::vector<std::string>::iterator it{input.begin()};
};
