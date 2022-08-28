#pragma once
#include <cstdint>
#include <string_view>

namespace pew {
class Signature {
  public:
	using value_type = std::size_t;

	constexpr Signature(std::size_t value = {}) : m_value(value) {}
	Signature(std::string_view str);
	Signature(char const* str) : Signature(std::string_view{str}) {}

	Signature(bool) = delete;

	bool operator==(Signature const&) const = default;
	explicit operator bool() const { return m_value != 0; }

	struct Hasher {
		constexpr std::size_t operator()(Signature const& s) const { return s.m_value; }
	};

  private:
	std::size_t m_value{};
};
} // namespace pew
