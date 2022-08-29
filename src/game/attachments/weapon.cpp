#include <engine/audio.hpp>
#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/weapon.hpp>
#include <game/layers.hpp>
#include <game/world.hpp>
#include <tardigrade/services.hpp>

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

constexpr auto fire_sfx_v = Resources::Sig{"sfx/weapon_fire.wav"};
} // namespace

bool Weapon::fire(glm::vec2 const position, glm::vec2 const velocity, vf::Time ttl) {
	if (m_fire_wait <= 0s) {
		auto projectile = Projectile{};
		projectile.velocity.target = velocity;
		projectile.ttl = ttl;
		m_projectiles.add(position, projectile);
		if (auto sound = tg::locate<Resources*>()->load<capo::Sound>(fire_sfx_v)) { tg::locate<Audio*>()->play(*sound); }
		m_fire_wait = fire_delay;
		return true;
	}
	return false;
}

bool Weapon::consume_hit(vf::Rect const& rect) {
	auto const is_hit = [rect](auto const& e) { return rect.contains(e.instance.transform.position); };
	return std::erase_if(m_projectiles.entries, is_hit) > 0;
}

void Weapon::setup() {
	tg::RenderAttachment::setup();
	m_projectiles.mesh = *tg::locate<vf::GfxDevice const*>();
	m_projectiles.mesh.buffer.write(vf::Geometry::make_quad({layout::projectile_quad}));

	layer = layers::projectile;
}

void Weapon::tick(tg::DeltaTime dt) {
	for (auto& projectile : m_projectiles.entries) {
		projectile.t.elapsed += dt.scaled;
		projectile.instance.transform.position += projectile.t.velocity.current * dt.scaled.count();
		projectile.t.ttl -= dt.scaled;
		if (projectile.t.ttl <= 0s) { projectile.t.destroyed = true; }
		projectile.t.velocity.current = accelerate(projectile.t.velocity.current, projectile.t.velocity.target, 20.0f, dt.scaled);
	}
	std::erase_if(m_projectiles.entries, [](auto const& e) { return e.t.destroyed; });
	if (m_fire_wait > 0s) { m_fire_wait -= dt.scaled; }
}

void Weapon::render(tg::RenderTarget const& target) const {
	auto const& frame = static_cast<Frame const&>(target).frame;
	m_projectiles.draw(frame);
}
} // namespace pew
