/**
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#ifndef _2fdb7c8a_93b7_11ec_b909_0242ac120002
#define _2fdb7c8a_93b7_11ec_b909_0242ac120002

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <maple/ast/ast.hpp>
#include <maple/support/utils.hpp>
#include <maple/support/typedef.hpp>

namespace maple::parse
{

struct Parser : private boost::noncopyable {
  struct Result {
    // Since positions has a reference to input, it also holds input.
    std::string input;

    ast::Program          ast;
    PositionCache         positions;
    std::filesystem::path file;
  };

  [[nodiscard]] Result getResult()
  {
    assert(!member_moved);

    member_moved = true;

    // The reason for also returning input is that positions has a reference to
    // input.
    return {std::move(input),
            std::move(ast),
            std::move(positions),
            std::move(file)};
  }

  Parser(std::string&& input, std::filesystem::path&& file);

private:
  void parse();

  bool member_moved = false;

  std::string         input;
  InputIterator       u32_first;
  const InputIterator u32_last;

  ast::Program  ast;
  PositionCache positions;

  std::filesystem::path file;
};

} // namespace maple::parse

#endif
