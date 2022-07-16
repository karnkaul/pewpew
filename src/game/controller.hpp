#pragma once
#include <core/ptr.hpp>
#include <glm/vec2.hpp>
#include <ktl/either.hpp>
#include <ktl/enum_flags/enum_flags.hpp>

namespace pew {
class Keyboard;
using GamepadId = int;

class Controller {
  public:
	using Type = ktl::either<Ptr<Keyboard const>, GamepadId>;

	enum class Flag { eFire };
	using Flags = ktl::enum_flags<Flag, std::uint8_t>;

	struct State {
		glm::vec2 xy{};
		Flags flags{};
	};

	Controller(Type type = {}) : m_type(type) {}

	Type type() const { return m_type; }
	State state() const;

  private:
	Type m_type{};
};
} // namespace pew
