/**
 * unicode.cpp
 *
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#include <unicode/unicode.hpp>

namespace maple::unicode
{

std::string utf32toUtf8(const char32_t utf32)
{
  std::u32string tmp{utf32};

  boost::u32_to_u8_iterator first{cbegin(tmp)}, last{cend(tmp)};

  return std::string(first, last);
}

std::string utf32toUtf8(const std::u32string_view utf32_str)
{
  boost::u32_to_u8_iterator first{cbegin(utf32_str)}, last{cend(utf32_str)};

  return std::string(first, last);
}

} // namespace maple::unicode