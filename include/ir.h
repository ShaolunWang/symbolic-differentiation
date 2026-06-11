/**
 * @file IR lowering
 * @brief AST flatening into executable computations
 */

#pragma once
#include "operations.h"
#include <utility>
#include <vector>

/**
 * @class IR
 * @brief IR class flattens the AST
 *
 */
class IRLowering {
public:
  // singleton

  explicit IRLowering(std::vector<std::shared_ptr<Operation>> operationList)
      : m_workList{std::move(operationList)} {}

  IRLowering(IRLowering &&) = delete;
  IRLowering &operator=(const IRLowering &) = delete;
  IRLowering &operator=(IRLowering &&) = delete;

  // flatten entry point
  // this only converts the AST into a vector of operations
  void flatten();
  [[nodiscard]] std::vector<std::shared_ptr<Operation>> getOpList() const {
    return m_result;
  }

private:
  std::vector<std::shared_ptr<Operation>> m_workList;
  std::vector<std::shared_ptr<Operation>> m_result;

  std::shared_ptr<Operation>
  flatExpr(const std::shared_ptr<Operation> &operation);
  std::shared_ptr<BinaryOp>
  BinaryToFlat(const std::shared_ptr<BinaryOp> &operation);
  std::shared_ptr<UnaryOp>
  UnaryToFlat(const std::shared_ptr<UnaryOp> &operation);
};
