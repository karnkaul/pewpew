#pragma once
#include <core/ptr.hpp>
#include <glm/vec2.hpp>
#include <ktl/enum_flags/enum_flags.hpp>

namespace pew {
class Keyboard;
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

	Controller(Ptr<Keyboard const> keyboard = {}) : m_keyboard(keyboard) {}

	Type type() const { return m_type; }
	GamepadId gamepadId() const { return m_gamepadId; }
	State update();

  private:
	Type m_type{};
	Ptr<Keyboard const> m_keyboard{};
	GamepadId m_gamepadId{};
};
} // namespace pew
