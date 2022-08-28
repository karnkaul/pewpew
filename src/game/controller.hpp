#pragma once
#include <glm/vec2.hpp>
#include <ktl/enum_flags/enum_flags.hpp>
#include <util/ptr.hpp>

namespace pew {
using GamepadId = int;

class Controller {
  public:
	enum class Type { eKeyboard, eGamepad };

	enum class Flag { eFire };
	using Flags = ktl::enum_flags<Flag, std::uint8_t>;

	struct State {
		glm::vec2 xy{};
		Flags flags{};
	};

	Type type() const { return m_type; }
	GamepadId gamepadId() const { return m_gamepad; }
	State update();

  private:
	Type m_type{};
	GamepadId m_gamepad{};
};
} // namespace pew
