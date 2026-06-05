#ifndef __BASE85ED__
#define __BASE85ED__

#include <cstdint>
#include <vector>

namespace base85 {

std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes);

std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str);

} // namespace base85

#endif
