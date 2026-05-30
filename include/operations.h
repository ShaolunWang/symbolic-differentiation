#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
enum class OpTy : uint8_t { Base, BinaryOp, UnaryOp, NamedVar, COUNT };
enum class Token : uint8_t {
  ADD,
  SUB,
  MUL,
  DIV,
  SQ,
  VARNAME,
  LBRACKET,
  RBRACKET,
  NUM,
  COUNT,
};

struct Operation {
  virtual std::string dump() const { return ""; };
  // name is used to make different keys when doing lookup in the ctx
  std::string _name;
  virtual ~Operation() = default;

  virtual std::string export_graphviz(const int &) const { return ""; }
  static std::string convert(const char &op) {
    switch (op) {
    case '+':
      return "PLUS";
    case '-':
      return "MINUS";
    case '*':
      return "MUL";
    case '/':
      return "DIV";
    case '^':
      return "SQ";
    default:
      return "EMPTY";
    }
  }

  // support equality operator - this will check whether two subtree is the same
  friend bool operator==(Operation const &lhs, Operation const &rhs) {
    return lhs._name == rhs._name;
  };

  virtual void
  makeRevOp(std::vector<std::shared_ptr<Operation>> &in_operation) {};

  std::vector<std::shared_ptr<Operation>> getRevOps();
  std::vector<std::shared_ptr<Operation>> revOp_worklist;

  virtual bool hasChild() const { return false; }
  virtual std::vector<std::shared_ptr<Operation>> getChilds() { return {}; };
};

struct RevOp : public Operation {
  RevOp(std::vector<std::shared_ptr<Operation>> in_operations, std::string name)
      : operations{std::move(in_operations)} {
    _name = std::move(name);
  };
  std::vector<std::shared_ptr<Operation>> operations;
  std::string dump() const override;
  std::string export_graphviz(const int &current_idx) const override;

  bool hasChild() const override { return true; }
};

struct BinaryOp : public Operation {
  BinaryOp(std::shared_ptr<Operation> lhs, const char op,
           std::shared_ptr<Operation> rhs)
      : lhs{std::move(lhs)}, rhs{std::move(rhs)}, op{op} {
    _name = dump();
  };

  std::string dump() const override;

  std::string export_graphviz(const int &curr_index) const override;

  std::shared_ptr<Operation> lhs;
  std::shared_ptr<Operation> rhs;
  char op;

  void
  makeRevOp(std::vector<std::shared_ptr<Operation>> &in_operation) override;

  bool hasChild() const override { return true; }
  std::vector<std::shared_ptr<Operation>> getChilds() override {
    return {lhs, rhs};
  };
};

struct Variable : public Operation {
  OpTy _ty{OpTy::NamedVar};
  explicit Variable(const std::string &name) : name{name} { _name = name; };

  void
  makeRevOp(std::vector<std::shared_ptr<Operation>> &in_operation) override;
  std::string dump() const override;
  std::string export_graphviz(const int &curr_index) const override;
  std::string name;

  bool hasChild() const override { return false; }
};

struct UnaryOp : public Operation {
  UnaryOp(std::shared_ptr<Operation> operation, char op)
      : operation{std::move(operation)}, op{op} {
    _name = dump();
  };
  bool hasChild() const override { return true; }
  std::string dump() const override;
  std::string export_graphviz(const int &curr_index) const override;
  void
  makeRevOp(std::vector<std::shared_ptr<Operation>> &in_operation) override;
  std::shared_ptr<Operation> operation;
  char op;
  std::vector<std::shared_ptr<Operation>> getChilds() override {
    return {operation};
  };
};
