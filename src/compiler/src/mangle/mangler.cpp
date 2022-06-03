/**
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#include <maple/mangle/mangler.hpp>
#include <maple/codegen/type.hpp>
#include <maple/codegen/common.hpp>

namespace maple::mangle
{

[[nodiscard]] std::string
Mangler::operator()(const ast::FunctionDecl& ast) const
{
  std::ostringstream mangled;

  mangled << "_Z";

  {
    // TODO: namespace
  }

  {
    // Add a function name.
    const auto name = ast.name.utf8();
    mangled << name.length() << name;
  }

  // Add argument type mangled names.
  for (const auto& param : *ast.params) {
    if (param.is_vararg)
      mangled << "v";
    else
      mangled << param.type->getMangledName();
  }

  return mangled.str();
}

[[nodiscard]] std::string
Mangler::operator()(const std::string_view             callee,
                    const std::vector<codegen::Value>& args) const
{
  std::ostringstream mangled;

  mangled << "_Z" << callee.length() << callee;

  for (const auto& arg : args)
    mangled << arg.getType()->getMangledName();

  return mangled.str();
}

} // namespace maple::mangle