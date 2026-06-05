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
      : m_nameCtx{free_var}, m_input{input} {};
  void parseMain();
  void genRev();
  std::string dump() const;
  std::string dumpRev() const;

  std::string exportGraphviz();
  std::string exportRevGraphviz();
  std::vector<std::shared_ptr<Operation>> getOpList() const {
    return m_operationList;
  }

private:
  bool check(Token type);
  bool match(Token type);
  std::shared_ptr<Operation> parseExpr();
  std::shared_ptr<Operation> parseUnary();
  std::shared_ptr<Operation> parseMulDivExpr();
  std::shared_ptr<Operation> parseDivExpr();
  std::shared_ptr<Operation> parsePlusMinusExpr();
  std::shared_ptr<Operation> parseMinusExpr();
  std::shared_ptr<Operation> parseVariable();
  static bool isVarName(const std::string &s) {
    return std::ranges::all_of(s, [](char c) { return std::isalnum(c); });
  }
  static bool isNum(const std::string &s) {
    return std::ranges::all_of(s, [](char c) { return std::isdigit(c); });
  }

  std::vector<std::string> m_nameCtx;
  std::vector<std::string> m_outCtx;
  std::vector<std::string> m_input;
  std::vector<std::string>::iterator m_it{m_input.begin()};
  std::vector<std::shared_ptr<Operation>> m_operationList;
  std::unordered_map<std::string, std::shared_ptr<Operation>> m_operationCtx;

  std::vector<std::shared_ptr<Operation>> m_revOpWorklist;
  std::unordered_map<std::string, std::shared_ptr<Operation>> m_revMap;

};
