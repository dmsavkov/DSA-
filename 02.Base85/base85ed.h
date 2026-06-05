#ifndef __BASE85ED__
#define __BASE85ED__

#include <vector>
#include <cstdint>

/**
 * @namespace base85
 * @brief Utilities for Base85 <en/de>coding of bytes
 */
namespace base85
{

/**
 * @brief Encodes bytes into a Base85.
 *
 * @param[in] bytes A vector containing the raw bytes.
 * @return std::vector<uint8_t> A vector of Base85-encoded bytes.
 *
 * @see decode()
 */
std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes); // TODO: add @throws if needed


/**
 * @brief Decodes Base85 to bytes.
 *
 * @param[in] b85str A vector containing Base85-encoded data.
 * @return std::vector<uint8_t> The decoded bytes.
 *
 * @see encode()
 */
std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str); // TODO: add @throws if needed

} // namespace base85

#endif // __BASE85ED__
