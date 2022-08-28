#include <util/signature.hpp>
#include <functional>

namespace pew {
Signature::Signature(std::string_view str) : m_value{std::hash<std::string_view>{}(str)} {}
} // namespace pew
