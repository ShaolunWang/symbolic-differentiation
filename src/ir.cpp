#include "ir.h"
#include "spdlog/spdlog.h"
#include <memory>

void IRLowering::flatten() {
  // while the worklist isn't empty, we can keep flattening the code
  for (const std::shared_ptr<Operation> &operation : m_workList) {
    m_result.emplace_back(flatExpr(operation));
  }
}
std::shared_ptr<Operation>
IRLowering::fromSymbolMap(const std::shared_ptr<Operation> &operation) {

  // if we already have it, we replace the current operation
  // with the one we have in the symbol map
  if (m_symbolMap.contains(operation->_name)) {
    spdlog::info("SymbolMap: Hit {}", operation->_name);
    return m_symbolMap[operation->_name];
  }
  // otherwise, we add it back into the symbol map
  spdlog::info("SymbolMap: Miss {}", operation->_name);
  m_symbolMap.emplace(operation->_name, operation);
  return operation;
}

std::shared_ptr<Operation>
IRLowering::flatExpr(const std::shared_ptr<Operation> &operation) {
  std::shared_ptr<Operation> flat = nullptr;
  if (isa<BinaryOp>(operation)) {
    flat = fromSymbolMap(
        BinaryToFlat(std::dynamic_pointer_cast<BinaryOp>(operation)));
  } else if (isa<UnaryOp>(operation)) {
    flat = fromSymbolMap(
        UnaryToFlat(std::dynamic_pointer_cast<UnaryOp>(operation)));
  } else if (isa<Variable>(operation)) {
    flat = fromSymbolMap(operation);
  }

  return flat;
}

std::shared_ptr<BinaryOp>
IRLowering::BinaryToFlat(const std::shared_ptr<BinaryOp> &operation) {
  spdlog::info("operation: {}", operation->dump());
  auto rhs = flatExpr(operation->rhs);
  spdlog::info("rhs: {}", rhs->dump());
  auto lhs = flatExpr(operation->lhs);
  spdlog::info("lhs: {}", lhs->dump());

  m_result.emplace_back(rhs);
  m_result.emplace_back(lhs);

  return std::make_shared<BinaryOp>(lhs, operation->op, rhs);
}
std::shared_ptr<UnaryOp>
IRLowering::UnaryToFlat(const std::shared_ptr<UnaryOp> &operation) {
  auto newOperation = flatExpr(operation->operation);
  m_result.emplace_back(newOperation);
  return std::make_shared<UnaryOp>(newOperation, operation->op);
}
