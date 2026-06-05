#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

#include "base85ed.h"

const std::vector<std::pair<const char *, const char *>> short_cases =
{
    { "",     ""     },
    { "F#",   "1"    },
    { "F){",  "12"   },
    { "F)}j", "123"  },
    { "F)}kW","1234" }
};

static std::vector<uint8_t> cstr2v(const char *s)
{
    return std::vector<uint8_t>(
               s,
               s + std::string(s).size()
           );
}

TEST(Base85ShortsEncode, TrivialShortEncodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::encode(cstr2v(p.second)), cstr2v(p.first));
    }
}

TEST(Base85ShortsDecode, TrivialShortDecodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::decode(cstr2v(p.first)), cstr2v(p.second));
    }
}

TEST(Base85RoundTrip, VariousLengths)
{
    for (std::size_t len = 0; len <= 256; ++len)
    {
        std::vector<uint8_t> original(len);
        for (std::size_t i = 0; i < len; ++i)
        {
            original[i] = static_cast<uint8_t>((i * 17 + 3) % 256);
        }

        const auto encoded = base85::encode(original);
        const auto decoded = base85::decode(encoded);
        EXPECT_EQ(decoded, original) << "length " << len;
    }
}

TEST(Base85RoundTrip, AllZeroBlocks)
{
    for (std::size_t len :
            {
                4U, 8U, 12U, 16U
            })
    {
        const std::vector<uint8_t> original(len, 0);
        EXPECT_EQ(base85::decode(base85::encode(original)), original);
    }
}

TEST(Base85Decode, RejectsInvalidCharacter)
{
    EXPECT_THROW(base85::decode(cstr2v("F# ")), std::runtime_error);
}

TEST(Base85Decode, RejectsSingleCharacterInput)
{
    EXPECT_THROW(base85::decode(cstr2v("F")), std::runtime_error);
}
