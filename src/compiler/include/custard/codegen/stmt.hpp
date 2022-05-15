/**
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#ifndef _10b4dce2_bc80_11ec_8422_0242ac120002
#define _10b4dce2_bc80_11ec_8422_0242ac120002

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <custard/pch/pch.hpp>
#include <custard/codegen/codegen.hpp>
#include <custard/codegen/common.hpp>

namespace custard::codegen
{

struct InitializerList {
  [[nodiscard]] const Value& operator[](const std::size_t idx) const
  {
    return initializer_list.at(idx);
  }

  template <typename... Args>
  void emplace_back(Args&&... args)
  {
    initializer_list.emplace_back(std::forward<Args>(args)...);
  }

  [[nodiscard]] std::size_t size() const noexcept
  {
    return initializer_list.size();
  }

  // Returns true if all types are the same.
  [[nodiscard]] bool verify() const
  {
    for (auto it = cbegin(initializer_list), last = cend(initializer_list);
         it != last;) {
      const auto tmp = it;

      ++it;
      if (it == last)
        return true;

      if (tmp->getType() != it->getType())
        return false;
    }

    return true;
  }

  // Return nullptr on failure.
  [[nodiscard]] llvm::Type* getType() const
  {
    if (!verify())
      return nullptr;

    if (initializer_list.front().getType()->isIntegerTy()) {
      const auto max_bitwidth
        = std::max_element(cbegin(initializer_list),
                           cend(initializer_list),
                           [](const Value& a, const Value& b) {
                             return a.getType()->getIntegerBitWidth()
                                    < b.getType()->getIntegerBitWidth();
                           });

      return max_bitwidth->getType();
    }

    return initializer_list.front().getType();
  }

  [[nodiscard]] const SignKindStack& createFrontSignKindStack() const
  {
    return initializer_list.front().getSignInfo();
  }

private:
  std::vector<Value> initializer_list;
};

void createStatement(CGContext&        ctx,
                     SymbolTable&      scope,
                     const ast::Stmt&  statement,
                     llvm::AllocaInst* retvar,
                     llvm::BasicBlock* end_bb,
                     llvm::BasicBlock* break_bb,
                     llvm::BasicBlock* continue_bb);

} // namespace custard::codegen

#endif