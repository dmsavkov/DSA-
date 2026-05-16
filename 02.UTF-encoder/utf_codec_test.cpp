#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "utf_codec.h"

namespace
{

std::u32string make_u32(std::vector<char32_t> const &cps)
{
    return std::u32string(cps.begin(), cps.end());
}

void expect_u32_eq(std::u32string const &actual, std::vector<char32_t> const &expected)
{
    EXPECT_EQ(actual, make_u32(expected));
}

std::u16string make_u16(std::vector<char16_t> const &units)
{
    return std::u16string(units.begin(), units.end());
}

std::string utf16_le_bytes(std::u16string const &units)
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

std::u16string utf16_from_le_bytes(std::string const &bytes)
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

std::string utf32_le_bytes(std::u32string const &units)
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

std::u32string utf32_from_le_bytes(std::string const &bytes)
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

} // namespace

TEST(Utf8Codec, EmptyRoundtrip)
{
    std::string s;
    EXPECT_EQ(utf::utf32_to_utf8(utf::utf8_to_utf32(s)), s);
    expect_u32_eq(utf::utf8_to_utf32(s), {});
}

TEST(Utf8Codec, AsciiRoundtrip)
{
    std::string s = "hello";
    expect_u32_eq(utf::utf8_to_utf32(s), {0x68, 0x65, 0x6C, 0x6C, 0x6F});
    EXPECT_EQ(utf::utf32_to_utf8(utf::utf8_to_utf32(s)), s);
}

TEST(Utf8Codec, CyrillicRoundtrip)
{
    std::string s = u8"\xD0\x93";
    expect_u32_eq(utf::utf8_to_utf32(s), {0x0413});
    EXPECT_EQ(utf::utf32_to_utf8(utf::utf8_to_utf32(s)), s);
}

TEST(Utf8Codec, EmojiRoundtrip)
{
    std::string s = u8"\xF0\x9F\x9A\x80";
    expect_u32_eq(utf::utf8_to_utf32(s), {0x1F680});
    EXPECT_EQ(utf::utf32_to_utf8(utf::utf8_to_utf32(s)), s);
}

TEST(Utf8Codec, MixedStringCodepoints)
{
    std::string s = u8"Hi \xD0\x93 \xF0\x9F\x9A\x80";
    expect_u32_eq(utf::utf8_to_utf32(s), {0x48, 0x69, 0x20, 0x0413, 0x20, 0x1F680});
    EXPECT_EQ(utf::utf32_to_utf8(utf::utf8_to_utf32(s)), s);
}

TEST(Utf16Codec, BmpRoundtrip)
{
    std::u32string u32 = make_u32({0x0413});
    std::u16string u16 = utf::utf32_to_utf16(u32);
    EXPECT_EQ(u16, std::u16string(1, static_cast<char16_t>(0x0413)));
    expect_u32_eq(utf::utf16_to_utf32(u16), {0x0413});
}

TEST(Utf16Codec, SurrogateRoundtrip)
{
    std::u32string u32 = make_u32({0x1F680});
    std::u16string u16 = utf::utf32_to_utf16(u32);
    ASSERT_EQ(u16.size(), 2U);
    EXPECT_EQ(u16[0], static_cast<char16_t>(0xD83D));
    EXPECT_EQ(u16[1], static_cast<char16_t>(0xDE80));
    expect_u32_eq(utf::utf16_to_utf32(u16), {0x1F680});
}

TEST(Utf16Codec, Utf32Utf16Symmetry)
{
    std::u32string u32 = make_u32({0x48, 0x1F680, 0xFFFF});
    EXPECT_EQ(utf::utf16_to_utf32(utf::utf32_to_utf16(u32)), u32);
}

TEST(UtfPipeline, Utf8Utf16Roundtrip)
{
    std::string s = u8"Hi \xD0\x93 \xF0\x9F\x9A\x80";
    EXPECT_EQ(utf::utf16_to_utf8(utf::utf8_to_utf16(s)), s);
}

TEST(UtfPipeline, Utf8Utf32Roundtrip)
{
    std::string s = u8"Hi \xD0\x93 \xF0\x9F\x9A\x80";
    EXPECT_EQ(utf::utf32_to_utf8(utf::utf8_to_utf32(s)), s);
}

TEST(UtfPipeline, Utf8Utf32Utf16Roundtrip)
{
    std::string s = u8"\xF0\x9F\x9A\x80";
    std::u16string via32 = utf::utf32_to_utf16(utf::utf8_to_utf32(s));
    EXPECT_EQ(utf::utf16_to_utf8(via32), s);
}

TEST(UtfPipeline, Utf8Utf16Utf32Roundtrip)
{
    std::string s = u8"Hi \xD0\x93";
    std::u32string via16 = utf::utf16_to_utf32(utf::utf8_to_utf16(s));
    EXPECT_EQ(utf::utf32_to_utf8(via16), s);
}

TEST(UtfWireFormat, Utf8Utf32LeRoundtrip)
{
    std::string utf8 = u8"Hi \xD0\x93 \xF0\x9F\x9A\x80";
    std::u32string u32 = utf::utf8_to_utf32(utf8);
    EXPECT_EQ(utf::utf32_to_utf8(utf32_from_le_bytes(utf32_le_bytes(u32))), utf8);
}

TEST(UtfWireFormat, Utf8Utf16LeRoundtrip)
{
    std::string utf8 = u8"Hi \xD0\x93 \xF0\x9F\x9A\x80";
    std::u16string u16 = utf::utf8_to_utf16(utf8);
    EXPECT_EQ(utf::utf16_to_utf8(utf16_from_le_bytes(utf16_le_bytes(u16))), utf8);
}

TEST(UtfWireFormat, Utf32LeUtf16LePipeline)
{
    std::string utf8 = u8"\xF0\x9F\x9A\x80";
    std::string u32le = utf32_le_bytes(utf::utf8_to_utf32(utf8));
    std::u16string u16 = utf::utf32_to_utf16(utf32_from_le_bytes(u32le));
    EXPECT_EQ(utf::utf32_to_utf8(utf::utf16_to_utf32(u16)), utf8);
}

TEST(Utf8Edge, InvalidLeadByteSkipped)
{
    std::string s = "\xFF\x41";
    expect_u32_eq(utf::utf8_to_utf32(s), {0x41});
}

TEST(Utf8Edge, TruncatedSequenceStops)
{
    std::string s = "\xE0\x80";
    expect_u32_eq(utf::utf8_to_utf32(s), {});
}

TEST(Utf16Edge, OrphanLowSurrogateSkipped)
{
    std::u16string u16 = make_u16({0xDC00, 0x0041});
    expect_u32_eq(utf::utf16_to_utf32(u16), {0x41});
}

TEST(Utf16Edge, TruncatedHighSurrogateStops)
{
    std::u16string u16 = make_u16({0xD800});
    expect_u32_eq(utf::utf16_to_utf32(u16), {});
}

TEST(Utf16Edge, InvalidLowAfterHighStops)
{
    std::u16string u16 = make_u16({0xD800, 0x0041});
    expect_u32_eq(utf::utf16_to_utf32(u16), {});
}
