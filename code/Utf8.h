// Utf8.h

#ifndef UTF8_H_
#define UTF8_H_

#include <cstddef>
#include <string>

namespace Utf8 {
  // Skip to the next unicode code point after pos in utf8.
  // Return the string length when there are no more code points.
  std::size_t NextCodePoint(const std::string &str, std::size_t pos);

  // Returns the start of the unicode code point at pos in utf8.
  std::size_t CodePointStart(const std::string &str, std::size_t pos);

  // Decodes a unicode code point in utf8.
  // Invalid codepoints are converted to 0xFFFFFFFF.
  char32_t DecodeCodePoint(const std::string &str, std::size_t pos);
}

#endif

