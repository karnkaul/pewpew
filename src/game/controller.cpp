#include <engine/keyboard.hpp>
#include <game/controller.hpp>
#include <glm/gtx/norm.hpp>
#include <vulkify/instance/gamepad.hpp>

namespace pew {
namespace {
constexpr float debounce(float const axis, float const epsilon = 0.05f) { return std::abs(axis) > epsilon ? axis : 0.0f; }

Controller::State keyboardState(Ptr<Keyboard const> const keyboard) {
	auto ret = Controller::State{};
	if (keyboard) {
		if (keyboard->pressed(vf::Key::eW) || keyboard->pressed(vf::Key::eUp)) { ret.xy.y += 1.0f; }
		if (keyboard->pressed(vf::Key::eS) || keyboard->pressed(vf::Key::eDown)) { ret.xy.y -= 1.0f; }
		if (keyboard->pressed(vf::Key::eD) || keyboard->pressed(vf::Key::eRight)) { ret.xy.x += 1.0f; }
		if (keyboard->pressed(vf::Key::eA) || keyboard->pressed(vf::Key::eLeft)) { ret.xy.x -= 1.0f; }
		if (keyboard->pressed(vf::Key::eSpace)) { ret.flags.set(Controller::Flag::eFire); }
		if (ret.xy.x != 0.0f || ret.xy.y != 0.0f) { ret.xy = glm::normalize(ret.xy); }
	}
	return ret;
}

Controller::State gamepadState(vf::Gamepad::Id const id) {
	auto ret = Controller::State{};
	if (auto gamepad = vf::Gamepad{id}) {
		ret.xy.x = debounce(gamepad(vf::GamepadAxis::eLeftX));
		ret.xy.y = debounce(-gamepad(vf::GamepadAxis::eLeftY));
		if (gamepad(vf::GamepadButton::eA)) { ret.flags.set(Controller::Flag::eFire); }
	}
	return ret;
}
} // namespace

Controller::State Controller::state() const {
	return m_type.visit(ktl::koverloaded{
		[](Ptr<Keyboard const> const kb) { return keyboardState(kb); },
		[](GamepadId const gp) { return gamepadState(gp); },
	});
}
} // namespace pew
