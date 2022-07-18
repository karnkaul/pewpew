#include <core/log.hpp>
#include <engine/keyboard.hpp>
#include <game/controller.hpp>
#include <glm/gtx/norm.hpp>
#include <vulkify/instance/gamepad.hpp>

namespace pew {
namespace {
constexpr float debounce(float const axis, float const epsilon = 0.1f) { return std::abs(axis) > epsilon ? axis : 0.0f; }

constexpr bool nonEmpty(Controller::State const& state) { return state.xy.x != 0.0f || state.xy.y != 0.0f || state.flags.any(); }

bool setState(vf::Gamepad::Id const id, Controller::State& out) {
	if (auto gamepad = vf::Gamepad{id}) {
		out.xy.x = debounce(gamepad(vf::GamepadAxis::eLeftX));
		out.xy.y = debounce(-gamepad(vf::GamepadAxis::eLeftY));
		if (gamepad(vf::GamepadButton::eA)) { out.flags.set(Controller::Flag::eFire); }
		return nonEmpty(out);
	}
	return false;
}

bool setState(Ptr<Keyboard const> const keyboard, Controller::State& out) {
	if (keyboard) {
		if (keyboard->pressed(vf::Key::eW) || keyboard->pressed(vf::Key::eUp)) { out.xy.y += 1.0f; }
		if (keyboard->pressed(vf::Key::eS) || keyboard->pressed(vf::Key::eDown)) { out.xy.y -= 1.0f; }
		if (keyboard->pressed(vf::Key::eD) || keyboard->pressed(vf::Key::eRight)) { out.xy.x += 1.0f; }
		if (keyboard->pressed(vf::Key::eA) || keyboard->pressed(vf::Key::eLeft)) { out.xy.x -= 1.0f; }
		if (keyboard->pressed(vf::Key::eSpace)) { out.flags.set(Controller::Flag::eFire); }
		if (out.xy.x != 0.0f || out.xy.y != 0.0f) { out.xy = glm::normalize(out.xy); }
		return nonEmpty(out);
	}
	return false;
}
} // namespace

Controller::State Controller::update() {
	auto ret = State{};
	auto const map = vf::Gamepad::map();
	for (auto id = vf::Gamepad::Id{}; id < vf::Gamepad::max_id_v; ++id) {
		if (map.map[id] && setState(id, ret)) {
			if (m_gamepadId != id) { plog::info("Controller changed: Gamepad{}", m_gamepadId); }
			m_gamepadId = id;
			m_type = Type::eGamepad;
			return ret;
		}
	}
	if (setState(m_keyboard, ret)) {
		if (m_type != Type::eKeyboard) { plog::info("Controller changed: Keyboard"); }
		m_type = Type::eKeyboard;
	}
	return ret;
}
} // namespace pew
