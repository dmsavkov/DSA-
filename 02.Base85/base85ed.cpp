#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "base85ed.h"

namespace
{

constexpr char ENCODE_TABLE[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";

constexpr int PAD_DIGIT = 84;

const std::array<int8_t, 256> DECODE_TABLE = []
{
    std::array<int8_t, 256> table{};
    table.fill(-1);
    for (int i = 0; i < 85; ++i)
    {
        table[static_cast<unsigned char>(ENCODE_TABLE[i])] =
            static_cast<int8_t>(i);
    }
    return table;
}();

void encode_full_block(uint32_t value, std::vector<uint8_t> &out)
{
    std::array<uint8_t, 5> block{};
    for (int i = 4; i >= 0; --i)
    {
        block[static_cast<std::size_t>(i)] =
            static_cast<uint8_t>(ENCODE_TABLE[value % 85]);
        value /= 85;
    }
    out.insert(out.end(), block.begin(), block.end());
}

} // namespace

std::vector<uint8_t> base85::encode(std::vector<uint8_t> const &bytes)
{
    std::vector<uint8_t> out;
    out.reserve(((bytes.size() + 3) / 4) * 5);

    std::size_t offset = 0;
    while (offset + 4 <= bytes.size())
    {
        uint32_t value = (static_cast<uint32_t>(bytes[offset]) << 24) |
                         (static_cast<uint32_t>(bytes[offset + 1]) << 16) |
                         (static_cast<uint32_t>(bytes[offset + 2]) << 8) |
                         static_cast<uint32_t>(bytes[offset + 3]);
        encode_full_block(value, out);
        offset += 4;
    }

    const std::size_t tail = bytes.size() - offset;
    if (tail > 0)
    {
        uint32_t value = 0;
        for (std::size_t i = 0; i < 4; ++i)
        {
            value <<= 8;
            if (i < tail)
            {
                value |= bytes[offset + i];
            }
        }

        const std::size_t group_len = tail + 1;
        std::array<uint8_t, 5> block{};
        for (int i = 4; i >= 0; --i)
        {
            if (static_cast<std::size_t>(i) < group_len)
            {
                block[static_cast<std::size_t>(i)] =
                    static_cast<uint8_t>(ENCODE_TABLE[value % 85]);
            }
            value /= 85;
        }
        out.insert(out.end(), block.begin(), block.begin() + group_len);
    }

    return out;
}

std::vector<uint8_t> base85::decode(std::vector<uint8_t> const &b85str)
{
    std::vector<uint8_t> out;
    out.reserve(((b85str.size() + 4) / 5) * 4);

    std::ptrdiff_t ascii_len = static_cast<std::ptrdiff_t>(b85str.size());
    uint8_t const *ascii_data = b85str.data();
    uint32_t accumulator = 0;
    int group_pos = 0;

    while (ascii_len > 0 || group_pos != 0)
    {
        int this_digit = PAD_DIGIT;

        if (ascii_len > 0)
        {
            this_digit = DECODE_TABLE[static_cast<unsigned char>(*ascii_data)];
            if (this_digit < 0)
            {
                throw std::runtime_error("bad Base85 character");
            }
        }

        if (group_pos == 4 &&
                (accumulator > UINT32_MAX / 85 ||
                 accumulator * 85 > UINT32_MAX - static_cast<uint32_t>(this_digit)))
        {
            throw std::runtime_error("Base85 overflow");
        }

        accumulator = accumulator * 85 + static_cast<uint32_t>(this_digit);
        ++group_pos;

        if (group_pos == 5)
        {
            const int chunk_len = ascii_len < 1 ? 3 + static_cast<int>(ascii_len) : 4;
            if (chunk_len == 0)
            {
                throw std::runtime_error("Incomplete Base85 group");
            }

            for (int i = 0; i < chunk_len; ++i)
            {
                out.push_back(
                    static_cast<uint8_t>((accumulator >> (24 - 8 * i)) & 0xFF));
            }

            group_pos = 0;
            accumulator = 0;
        }

        --ascii_len;
        ++ascii_data;
    }

    return out;
}
