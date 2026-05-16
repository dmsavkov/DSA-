#include "utf_codec.h"

namespace utf
{

std::u32string utf8_to_utf32(std::string const &input)
{
    std::u32string output;
    size_t i = 0;

    while (i < input.length())
    {
        unsigned char lead = static_cast<unsigned char>(input[i]);
        char32_t codepoint = 0;

        if ((lead & 0x80) == 0)
        {
            codepoint = lead;
            i++;
        }
        else if ((lead & 0xE0) == 0xC0)
        {
            if (i + 1 >= input.length())
            {
                break;
            }
            unsigned char cont = static_cast<unsigned char>(input[i + 1]);
            if ((cont & 0xC0) != 0x80)
            {
                break;
            }

            codepoint = ((lead & 0x1F) << 6) | (cont & 0x3F);
            i += 2;
        }
        else if ((lead & 0xF0) == 0xE0)
        {
            if (i + 2 >= input.length())
            {
                break;
            }
            unsigned char cont1 = static_cast<unsigned char>(input[i + 1]);
            unsigned char cont2 = static_cast<unsigned char>(input[i + 2]);
            if ((cont1 & 0xC0) != 0x80 || (cont2 & 0xC0) != 0x80)
            {
                break;
            }

            codepoint = ((lead & 0x0F) << 12) | ((cont1 & 0x3F) << 6) | (cont2 & 0x3F);
            i += 3;
        }
        else if ((lead & 0xF8) == 0xF0)
        {
            if (i + 3 >= input.length())
            {
                break;
            }
            unsigned char cont1 = static_cast<unsigned char>(input[i + 1]);
            unsigned char cont2 = static_cast<unsigned char>(input[i + 2]);
            unsigned char cont3 = static_cast<unsigned char>(input[i + 3]);
            if ((cont1 & 0xC0) != 0x80 || (cont2 & 0xC0) != 0x80 || (cont3 & 0xC0) != 0x80)
            {
                break;
            }

            codepoint = ((lead & 0x07) << 18) | ((cont1 & 0x3F) << 12) |
                        ((cont2 & 0x3F) << 6) | (cont3 & 0x3F);
            i += 4;
        }
        else
        {
            i++;
            continue;
        }

        output += codepoint;
    }

    return output;
}

std::string utf32_to_utf8(std::u32string const &input)
{
    std::string output;

    for (char32_t cp : input)
    {
        if (cp <= 0x7F)
        {
            output.push_back(static_cast<char>(cp));
        }
        else if (cp <= 0x7FF)
        {
            output.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            output.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0xFFFF)
        {
            output.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            output.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            output.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0x10FFFF)
        {
            output.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            output.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            output.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            output.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }

    return output;
}

std::u32string utf16_to_utf32(std::u16string const &input)
{
    std::u32string output;
    size_t i = 0;

    while (i < input.length())
    {
        char16_t code_unit = input[i];
        char32_t codepoint = 0;

        if (code_unit < 0xD800 || code_unit >= 0xE000)
        {
            codepoint = code_unit;
            i++;
        }
        else if (code_unit >= 0xD800 && code_unit <= 0xDBFF)
        {
            if (i + 1 >= input.length())
            {
                break;
            }
            char16_t low = input[i + 1];
            if (low < 0xDC00 || low > 0xDFFF)
            {
                break;
            }

            codepoint = 0x10000 + (((code_unit - 0xD800) << 10) | (low - 0xDC00));
            i += 2;
        }
        else
        {
            i++;
            continue;
        }

        output += codepoint;
    }

    return output;
}

std::u16string utf32_to_utf16(std::u32string const &input)
{
    std::u16string output;

    for (char32_t cp : input)
    {
        if (cp <= 0xFFFF)
        {
            output.push_back(static_cast<char16_t>(cp));
        }
        else if (cp <= 0x10FFFF)
        {
            cp -= 0x10000;
            char16_t high = static_cast<char16_t>(0xD800 + (cp >> 10));
            char16_t low = static_cast<char16_t>(0xDC00 + (cp & 0x3FF));
            output.push_back(high);
            output.push_back(low);
        }
    }

    return output;
}

std::u16string utf8_to_utf16(std::string const &input)
{
    return utf32_to_utf16(utf8_to_utf32(input));
}

std::string utf16_to_utf8(std::u16string const &input)
{
    return utf32_to_utf8(utf16_to_utf32(input));
}

} // namespace utf
