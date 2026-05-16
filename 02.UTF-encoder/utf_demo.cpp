#include <bitset>
#include <iostream>
#include <string>

#include "utf_codec.h"

static void coutBinaryString(std::string const &str)
{
    for (unsigned char c : str)
    {
        std::bitset<8> bits(c);
        std::cout << bits << std::endl;
    }
    std::cout << std::endl;
}

static void coutBinaryString(std::u32string const &str)
{
    for (char32_t c : str)
    {
        std::bitset<32> bits(c);
        std::cout << bits << std::endl;
    }
    std::cout << std::endl;
}

static void coutBinaryString(std::u16string const &str)
{
    for (char16_t c : str)
    {
        std::bitset<16> bits(c);
        std::cout << bits << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    std::string hard_text = u8"Hi Г 🚀";
    std::cout << "==== ORIGINAL UTF-8 STRING ====\n";
    std::cout << "String: " << hard_text << std::endl << std::endl;
    std::cout << "Binary representation (UTF-8):\n";
    coutBinaryString(hard_text);

    std::cout << "\n==== UTF-8 TO UTF-32 ====\n";
    std::u32string utf32_text = utf::utf8_to_utf32(hard_text);
    std::cout << "Binary representation (UTF-32):\n";
    coutBinaryString(utf32_text);

    std::cout << "\n==== UTF-32 BACK TO UTF-8 ====\n";
    std::string reencoded_utf8 = utf::utf32_to_utf8(utf32_text);
    std::cout << "Re-encoded string: " << reencoded_utf8 << std::endl;
    std::cout << "Binary representation (UTF-8):\n";
    coutBinaryString(reencoded_utf8);

    std::cout << "\n==== UTF-8 TO UTF-16 ====\n";
    std::u16string utf16_text = utf::utf8_to_utf16(hard_text);
    std::cout << "Binary representation (UTF-16):\n";
    coutBinaryString(utf16_text);

    std::cout << "\n==== UTF-16 BACK TO UTF-8 ====\n";
    std::string utf16_to_utf8_result = utf::utf16_to_utf8(utf16_text);
    std::cout << "Re-encoded string: " << utf16_to_utf8_result << std::endl;
    std::cout << "Binary representation (UTF-8):\n";
    coutBinaryString(utf16_to_utf8_result);

    std::cout << "\n==== UTF-16 TO UTF-32 ====\n";
    std::u32string utf16_to_utf32 = utf::utf16_to_utf32(utf16_text);
    std::cout << "Binary representation (UTF-32):\n";
    coutBinaryString(utf16_to_utf32);

    std::cout << "\n==== UTF-32 TO UTF-16 ====\n";
    std::u16string utf32_to_utf16 = utf::utf32_to_utf16(utf32_text);
    std::cout << "Binary representation (UTF-16):\n";
    coutBinaryString(utf32_to_utf16);

    return 0;
}
