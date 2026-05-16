#ifndef __UTF_CODEC__
#define __UTF_CODEC__

#include <string>

/**
 * @namespace utf
 * @brief UTF-8 / UTF-16 (native LE code units) / UTF-32 conversions.
 *
 * utf_tool wire format: UTF-16LE and UTF-32LE raw code unit bytes (native LE on x86_64).
 */
namespace utf
{

std::u32string utf8_to_utf32(std::string const &input);
std::string utf32_to_utf8(std::u32string const &input);

std::u32string utf16_to_utf32(std::u16string const &input);
std::u16string utf32_to_utf16(std::u32string const &input);

std::u16string utf8_to_utf16(std::string const &input);
std::string utf16_to_utf8(std::u16string const &input);

} // namespace utf

#endif // __UTF_CODEC__
