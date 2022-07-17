#include <game/layers.hpp>
#include <game/objects/damager.hpp>
#include <game/objects/player.hpp>
#include <game/status_effects/time_dilator.hpp>
#include <game/world.hpp>

namespace pew {
namespace {
constexpr glm::vec2 clamp(glm::vec2 const v, glm::vec2 const a, glm::vec2 const b) { return {std::clamp(v.x, a.x, b.x), std::clamp(v.y, a.y, b.y)}; }
} // namespace

Player& Player::takeDamage(float damage) {
	if (damage <= 0.0f) { return *this; }
	hp = std::max(hp - damage, 0.0f);
	// TODO: animation, invincibility
	m_world->spawn<TimeDilator>();
	return *this;
}

Player& Player::destroy(Damager& out) {
	score += out.points;
	out.destroy();
	return *this;
}

void Player::setup() {
	Pawn::setup();

	auto const x = m_world->area().x * (1.0f - nPad.x);
	quad.instance.transform.position.x = -x;

	controller = Controller(&m_world->keyboard());
	weapon = m_world->spawn<Weapon>();
	layer = layers::playerLayer;
}

void Player::tick(DeltaTime dt) {
	auto const state = controller.state();
	auto const pos = quad.instance.transform.position + state.xy * speed * dt.scaled.count();
	auto const ha = m_world->area() * 0.5f * (1.0f - nPad);
	quad.instance.transform.position = clamp(pos, -ha, ha);

	if (state.flags.test(Controller::Flag::eFire)) {
		auto const offset = size() * nMuzzleOffset;
		assert(weapon);
		weapon->fire(quad.instance.transform.position + offset);
	}
}
} // namespace pew
