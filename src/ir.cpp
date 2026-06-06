#include "ir.h"
#include "fmt/base.h"
#include <memory>

void IRLowering::flatten() {
  // while the worklist isn't empty, we can keep flattening the code
  for (const std::shared_ptr<Operation> &operation : m_workList) {
    flatExpr(operation);
  }
}
std::shared_ptr<Operation>
IRLowering::flatExpr(const std::shared_ptr<Operation> &operation) {
  std::shared_ptr<Operation> flat = nullptr;
  if (isa<BinaryOp>(operation)) {
    flat = BinaryToFlat(std::dynamic_pointer_cast<BinaryOp>(operation));
    m_result.emplace_back(flat);
  } else if (isa<UnaryOp>(operation)) {
    flat = UnaryToFlat(std::dynamic_pointer_cast<UnaryOp>(operation));
    m_result.emplace_back(flat);
  } else if (isa<Variable>(operation)) {
    flat = operation;
    m_result.emplace_back(operation);
  }
	
  return flat;
}

std::shared_ptr<BinaryOp>
IRLowering::BinaryToFlat(const std::shared_ptr<BinaryOp> &operation) {

  auto lhs = flatExpr(operation->lhs);
  auto rhs = flatExpr(operation->rhs);
  m_result.emplace_back(lhs);
  m_result.emplace_back(rhs);
  return std::make_shared<BinaryOp>(lhs, operation->op, rhs);
}
std::shared_ptr<UnaryOp>
IRLowering::UnaryToFlat(const std::shared_ptr<UnaryOp> &operation) {
  auto newOperation = flatExpr(operation->operation);
  m_result.emplace_back(newOperation);
  return std::make_shared<UnaryOp>(newOperation, operation->op);
}
