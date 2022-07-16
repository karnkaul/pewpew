#include <core/log.hpp>
#include <engine/audio.hpp>
#include <game/objects/weapon.hpp>
#include <game/world.hpp>

namespace pew {
namespace {
constexpr glm::vec2 accelerate(glm::vec2 current, glm::vec2 const target, float const dv, vf::Time const dt) {
	bool const dx = current.x < target.x;
	bool const dy = current.y < target.y;
	if (!dx && !dy) { return current; }
	if (dx) { current.x += (dv * dt.count() * 1000.0f); }
	if (dy) { current.y += (dv * dt.count() * 1000.0f); }
	return current;
}
} // namespace

void Weapon::setup() { m_projectiles.mesh = {m_world->vfContext(), "weapon"}; }

bool Weapon::fire(glm::vec2 const position, glm::vec2 const velocity, vf::Time ttl) {
	if (m_fireWait <= 0s) {
		auto projectile = Projectile{};
		projectile.velocity.target = velocity;
		projectile.ttl = ttl;
		m_projectiles.add(position, projectile);
		m_world->audio().play(m_world->resources.sfx.fire);
		m_fireWait = fireDelay;
		return true;
	}
	return false;
}

void Weapon::tick(vf::Time dt) {
	for (auto& projectile : m_projectiles.entries) {
		projectile.t.elapsed += dt;
		projectile.instance.transform.position += projectile.t.velocity.current * dt.count();
		projectile.t.ttl -= dt;
		if (projectile.t.ttl <= 0s) { projectile.t.destroyed = true; }
		projectile.t.velocity.current = accelerate(projectile.t.velocity.current, projectile.t.velocity.target, 20.0f, dt);

		for (auto& damager : m_world->damagers()) {
			if (damager->intersecting(projectile.instance.transform.position)) {
				m_world->player()->destroy(*damager);
				projectile.t.destroyed = true;
			}
		}
	}
	std::erase_if(m_projectiles.entries, [](auto const& e) { return e.t.destroyed; });
	if (m_fireWait > 0s) { m_fireWait -= dt; }
}

void Weapon::draw(vf::Frame const& frame) const { m_projectiles.draw(frame); }
} // namespace pew
