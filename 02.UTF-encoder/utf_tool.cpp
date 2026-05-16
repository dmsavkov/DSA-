#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "utf_codec.h"

namespace
{

constexpr char USAGE[] =
    "Usage: utfencoder <mode>\n"
    "  -8to16   --utf8-to-utf16    UTF-8 bytes  -> UTF-16LE bytes\n"
    "  -16to8   --utf16-to-utf8    UTF-16LE     -> UTF-8 bytes\n"
    "  -8to32   --utf8-to-utf32    UTF-8 bytes  -> UTF-32LE bytes\n"
    "  -32to8   --utf32-to-utf8    UTF-32LE     -> UTF-8 bytes\n"
    "  -16to32  --utf16-to-utf32   UTF-16LE     -> UTF-32LE bytes\n"
    "  -32to16  --utf32-to-utf16   UTF-32LE     -> UTF-16LE bytes\n";

std::string read_stdin_to_string()
{
    constexpr std::streamsize BUF_SIZE = 64 * 1024;
    std::string out;
    out.reserve(1024);
    std::vector<char> buf(BUF_SIZE);

    std::ios::sync_with_stdio(false);

    while (std::cin)
    {
        std::cin.read(buf.data(), BUF_SIZE);
        std::streamsize n = std::cin.gcount();
        if (n > 0)
        {
            out.append(buf.data(), static_cast<size_t>(n));
        }
        if (n < BUF_SIZE)
        {
            break;
        }
    }
    return out;
}

void write_string_to_stdout(std::string const &data)
{
    std::ios::sync_with_stdio(false);
    if (!data.empty())
    {
        std::cout.write(data.data(), static_cast<std::streamsize>(data.size()));
    }
    std::cout.flush();
}

std::u16string bytes_to_utf16(std::string const &bytes)
{
    std::u16string out;
    if (bytes.size() % 2 != 0)
    {
        return out;
    }
    out.resize(bytes.size() / 2);
    for (size_t i = 0; i < out.size(); ++i)
    {
        out[i] = static_cast<char16_t>(
                     static_cast<unsigned char>(bytes[2 * i]) |
                     (static_cast<char16_t>(static_cast<unsigned char>(bytes[2 * i + 1])) << 8));
    }
    return out;
}

std::string utf16_to_bytes(std::u16string const &units)
{
    std::string out;
    out.reserve(units.size() * 2);
    for (char16_t unit : units)
    {
        out.push_back(static_cast<char>(unit & 0xFF));
        out.push_back(static_cast<char>((unit >> 8) & 0xFF));
    }
    return out;
}

std::u32string bytes_to_utf32(std::string const &bytes)
{
    std::u32string out;
    if (bytes.size() % 4 != 0)
    {
        return out;
    }
    out.resize(bytes.size() / 4);
    for (size_t i = 0; i < out.size(); ++i)
    {
        char32_t cp = static_cast<unsigned char>(bytes[4 * i]);
        cp |= static_cast<char32_t>(static_cast<unsigned char>(bytes[4 * i + 1])) << 8;
        cp |= static_cast<char32_t>(static_cast<unsigned char>(bytes[4 * i + 2])) << 16;
        cp |= static_cast<char32_t>(static_cast<unsigned char>(bytes[4 * i + 3])) << 24;
        out[i] = cp;
    }
    return out;
}

std::string utf32_to_bytes(std::u32string const &units)
{
    std::string out;
    out.reserve(units.size() * 4);
    for (char32_t cp : units)
    {
        out.push_back(static_cast<char>(cp & 0xFF));
        out.push_back(static_cast<char>((cp >> 8) & 0xFF));
        out.push_back(static_cast<char>((cp >> 16) & 0xFF));
        out.push_back(static_cast<char>((cp >> 24) & 0xFF));
    }
    return out;
}

enum class Mode
{
    Utf8ToUtf16,
    Utf16ToUtf8,
    Utf8ToUtf32,
    Utf32ToUtf8,
    Utf16ToUtf32,
    Utf32ToUtf16,
    Unknown,
};

Mode parse_mode(std::string const &arg)
{
    if (arg == "-8to16" || arg == "--utf8-to-utf16")
    {
        return Mode::Utf8ToUtf16;
    }
    if (arg == "-16to8" || arg == "--utf16-to-utf8")
    {
        return Mode::Utf16ToUtf8;
    }
    if (arg == "-8to32" || arg == "--utf8-to-utf32")
    {
        return Mode::Utf8ToUtf32;
    }
    if (arg == "-32to8" || arg == "--utf32-to-utf8")
    {
        return Mode::Utf32ToUtf8;
    }
    if (arg == "-16to32" || arg == "--utf16-to-utf32")
    {
        return Mode::Utf16ToUtf32;
    }
    if (arg == "-32to16" || arg == "--utf32-to-utf16")
    {
        return Mode::Utf32ToUtf16;
    }
    return Mode::Unknown;
}

std::string transform(Mode mode, std::string const &input)
{
    switch (mode)
    {
    case Mode::Utf8ToUtf16:
        return utf16_to_bytes(utf::utf8_to_utf16(input));
    case Mode::Utf16ToUtf8:
        return utf::utf16_to_utf8(bytes_to_utf16(input));
    case Mode::Utf8ToUtf32:
        return utf32_to_bytes(utf::utf8_to_utf32(input));
    case Mode::Utf32ToUtf8:
        return utf::utf32_to_utf8(bytes_to_utf32(input));
    case Mode::Utf16ToUtf32:
        return utf32_to_bytes(utf::utf16_to_utf32(bytes_to_utf16(input)));
    case Mode::Utf32ToUtf16:
        return utf16_to_bytes(utf::utf32_to_utf16(bytes_to_utf32(input)));
    default:
        return {};
    }
}

} // namespace

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << USAGE;
        return 1;
    }

    Mode const mode = parse_mode(argv[1]);
    if (mode == Mode::Unknown)
    {
        std::cerr << "Unknown option <" << argv[1] << ">\n" << USAGE;
        return 1;
    }

    write_string_to_stdout(transform(mode, read_stdin_to_string()));
    return 0;
}
