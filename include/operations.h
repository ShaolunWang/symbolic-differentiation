#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
// llvm style isa
template <typename To, typename From> bool isa(From const &f) {
  return To::classof(&f);
}
// overload for shared_ptr
template <typename To, typename T> bool isa(const std::shared_ptr<T> &ptr) {
  return ptr && To::classof(ptr.get());
}
enum class OpTy : uint8_t { Base, BinaryOp, UnaryOp, NamedVar, RevOp, COUNT };
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

  explicit Operation(OpTy ty) : _ty{ty} {}
  // typecheck
  static bool classof(Operation const *) { return true; }
  [[nodiscard]] OpTy getType() const { return _ty; }
  OpTy _ty;

  [[nodiscard]] virtual std::string dump() const { return ""; };
  // name is used to make different keys when doing lookup in the ctx
  std::string _name;
  virtual ~Operation() = default;

  [[nodiscard]] virtual std::string export_graphviz(const int &) const {
    return "";
  }
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

  [[nodiscard]] virtual bool hasChild() const { return false; }
  virtual std::vector<std::shared_ptr<Operation>> getChilds() { return {}; };
};

struct RevOp : public Operation {
  RevOp(std::vector<std::shared_ptr<Operation>> in_operations, std::string name,
        OpTy ty = OpTy::RevOp)
      : Operation(ty), operations{std::move(in_operations)} {
    _name = std::move(name);
  };

  static bool classof(RevOp const *) { return true; }

  static bool classof(Operation const *B) {
    return B->getType() == OpTy::RevOp;
  }

  std::vector<std::shared_ptr<Operation>> operations;
  [[nodiscard]] std::string dump() const override;
  [[nodiscard]] std::string
  export_graphviz(const int &current_idx) const override;

  [[nodiscard]] bool hasChild() const override { return true; }
};

struct BinaryOp : public Operation {
  BinaryOp(std::shared_ptr<Operation> lhs, const char op,
           std::shared_ptr<Operation> rhs, OpTy ty = OpTy::BinaryOp)
      : Operation(ty), lhs{std::move(lhs)}, rhs{std::move(rhs)}, op{op} {
    _name = dump();
  };
  // typecheck
  static bool classof(BinaryOp const *) { return true; }

  static bool classof(Operation const *B) {
    return B->getType() == OpTy::BinaryOp;
  }

  [[nodiscard]] std::string dump() const override;

  [[nodiscard]] std::string
  export_graphviz(const int &curr_index) const override;

  std::shared_ptr<Operation> lhs;
  std::shared_ptr<Operation> rhs;
  char op;

  void
  makeRevOp(std::vector<std::shared_ptr<Operation>> &in_operation) override;

  [[nodiscard]] bool hasChild() const override { return true; }
  std::vector<std::shared_ptr<Operation>> getChilds() override {
    return {lhs, rhs};
  };
};

struct Variable : public Operation {
  OpTy _ty{OpTy::NamedVar};
  explicit Variable(const std::string &name, OpTy ty = OpTy::NamedVar)
      : Operation(ty), name{name} {
    _name = name;
  };
  // typecheck
  static bool classof(Variable const *) { return true; }

  static bool classof(Operation const *B) {
    return B->getType() == OpTy::NamedVar;
  }
  void
  makeRevOp(std::vector<std::shared_ptr<Operation>> &in_operation) override;
  [[nodiscard]] std::string dump() const override;
  [[nodiscard]] std::string
  export_graphviz(const int &curr_index) const override;
  std::string name;

  [[nodiscard]] bool hasChild() const override { return false; }
};

struct UnaryOp : public Operation {
  UnaryOp(std::shared_ptr<Operation> operation, char op,
          OpTy ty = OpTy::UnaryOp)
      : Operation(ty), operation{std::move(operation)}, op{op} {
    _name = dump();
  };
  // typecheck
  static bool classof(UnaryOp const *) { return true; }

  static bool classof(Operation const *B) {
    return B->getType() == OpTy::UnaryOp;
  }

  [[nodiscard]] bool hasChild() const override { return true; }
  [[nodiscard]] std::string dump() const override;
  [[nodiscard]] std::string
  export_graphviz(const int &curr_index) const override;
  void
  makeRevOp(std::vector<std::shared_ptr<Operation>> &in_operation) override;
  std::shared_ptr<Operation> operation;
  char op;
  std::vector<std::shared_ptr<Operation>> getChilds() override {
    return {operation};
  };
};
