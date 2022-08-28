#include <game/controller.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>
#include <vulkify/instance/gamepad.hpp>
#include <vulkify/instance/keyboard.hpp>

namespace pew {
namespace {
constexpr float debounce(float const axis, float const epsilon = 0.1f) { return std::abs(axis) > epsilon ? axis : 0.0f; }

constexpr bool non_empty(Controller::State const& state) { return state.xy.x != 0.0f || state.xy.y != 0.0f || state.flags.any(); }

bool set_state(vf::Gamepad::Id const id, Controller::State& out) {
	if (auto gamepad = vf::Gamepad{id}) {
		out.xy.x = debounce(gamepad(vf::GamepadAxis::eLeftX));
		out.xy.y = debounce(gamepad(vf::GamepadAxis::eLeftY));
		if (gamepad(vf::GamepadButton::eA)) { out.flags.set(Controller::Flag::eFire); }
		return non_empty(out);
	}
	return false;
}

bool set_state(Controller::State& out) {
	if (vf::keyboard::held(vf::Key::eW) || vf::keyboard::held(vf::Key::eUp)) { out.xy.y += 1.0f; }
	if (vf::keyboard::held(vf::Key::eS) || vf::keyboard::held(vf::Key::eDown)) { out.xy.y -= 1.0f; }
	if (vf::keyboard::held(vf::Key::eD) || vf::keyboard::held(vf::Key::eRight)) { out.xy.x += 1.0f; }
	if (vf::keyboard::held(vf::Key::eA) || vf::keyboard::held(vf::Key::eLeft)) { out.xy.x -= 1.0f; }
	if (vf::keyboard::held(vf::Key::eSpace)) { out.flags.set(Controller::Flag::eFire); }
	if (out.xy.x != 0.0f || out.xy.y != 0.0f) { out.xy = glm::normalize(out.xy); }
	return non_empty(out);
}
} // namespace

Controller::State Controller::update() {
	auto ret = State{};
	auto const map = vf::Gamepad::map();
	for (auto id = vf::Gamepad::Id{}; id < vf::Gamepad::max_id_v; ++id) {
		if (map.map[id] && set_state(id, ret)) {
			if (m_gamepad != id) { logger::info("Controller changed: Gamepad_{}", m_gamepad); }
			m_gamepad = id;
			m_type = Type::eGamepad;
			return ret;
		}
	}
	if (set_state(ret)) {
		if (m_type != Type::eKeyboard) { logger::info("Controller changed: Keyboard"); }
		m_type = Type::eKeyboard;
	}
	return ret;
}
} // namespace pew
