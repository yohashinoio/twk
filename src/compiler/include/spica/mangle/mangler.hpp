/**
 * These codes are licensed under LICNSE_NAME License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#ifndef _18de40e3_5ff0_4b6c_833a_5f79d5726fcc
#define _18de40e3_5ff0_4b6c_833a_5f79d5726fcc

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <spica/pch/pch.hpp>
#include <spica/ast/ast.hpp>

namespace spica
{

namespace codegen
{

struct NsHierarchy;
struct CGContext;
struct Value;
struct Type;

namespace mangle
{

constexpr const char* ellipsis = "z";

constexpr const char* prefix = "_Z";

using TemplateArguments = std::vector<std::shared_ptr<Type>>;

struct Mangler : private boost::noncopyable {
  [[nodiscard]] std::string mangleFunction(CGContext&               ctx,
                                           const ast::FunctionDecl& decl) const;

  [[nodiscard]] std::string
  mangleFunctionTemplate(CGContext&               ctx,
                         const NsHierarchy&       space,
                         const ast::FunctionDecl& decl,
                         const TemplateArguments& template_args) const;

  [[nodiscard]] std::vector<std::string>
  mangleFunctionCall(CGContext&               ctx,
                     const std::string_view   callee,
                     const std::deque<Value>& args) const;

  [[nodiscard]] std::vector<std::string>
  mangleFunctionTemplateCall(CGContext&               ctx,
                             const std::string_view   callee,
                             const std::deque<Value>& args) const;

  // The mangled this pointer type is inserted automatically
  [[nodiscard]] std::vector<std::string>
  mangleMethodCall(CGContext&               ctx,
                   const std::string_view   callee,
                   const std::string&       class_name,
                   const std::deque<Value>& args,
                   const Accessibility      accessibility) const;

  // Return results stored in order of priority
  [[nodiscard]] std::vector<std::string>
  mangleConstructorCall(CGContext& ctx, const std::deque<Value>& args) const;

  // The mangled this pointer type is inserted automatically
  [[nodiscard]] std::vector<std::string>
  mangleDestructorCall(CGContext& ctx, const std::string& class_name) const;

private:
  [[nodiscard]] std::string
  mangleTemplateArguments(CGContext& ctx, const TemplateArguments& args) const;

  [[nodiscard]] std::string mangleFunctionName(const std::string& name) const;

  // Return results stored in order of priority
  [[nodiscard]] std::vector<std::string>
  mangleNamespaceHierarchy(const NsHierarchy& namespaces) const;

  [[nodiscard]] std::string
  mangleThisPointer(CGContext& ctx, const std::string& class_name) const;

  [[nodiscard]] std::string mangleArgs(CGContext&               ctx,
                                       const std::deque<Value>& args) const;

  [[nodiscard]] std::string
  mangleParams(CGContext& ctx, const ast::ParameterList& params) const;
};

} // namespace mangle

} // namespace codegen

} // namespace spica

#endif
