/**
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#ifndef _18de40e3_5ff0_4b6c_833a_5f79d5726fcc
#define _18de40e3_5ff0_4b6c_833a_5f79d5726fcc

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <maple/pch/pch.hpp>
#include <maple/ast/ast.hpp>

namespace maple
{
namespace codegen
{

struct Value;

} // namespace codegen

namespace mangle
{

class Mangler : private boost::noncopyable {
public:
  // For function definition.
  [[nodiscard]] std::string operator()(const ast::FunctionDecl& ast) const;

  // For function call.
  [[nodiscard]] std::string
  operator()(const std::string_view             callee,
             const std::vector<codegen::Value>& args) const;
};

} // namespace mangle
} // namespace maple

#endif