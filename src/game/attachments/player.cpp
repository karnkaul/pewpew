#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/weapon.hpp>
#include <game/layers.hpp>
#include <game/world.hpp>
#include <tardigrade/services.hpp>

namespace pew {
namespace {
auto const ship_sprite_sheet = Signature{"sheets/player.txt"};
}

void Player::take_damage(int damage) {
	if (damage > 0) { hp = std::clamp(hp - damage, 0, hp); }
}

void Player::score_points(std::uint64_t points) { score += points; }

void Player::heal(int points) { hp = std::clamp(hp + points, hp, max_hp_v); }

void Player::setup() {
	Prop::setup();

	auto* world = static_cast<World*>(scene());
	auto const x = world->area().x * (1.0f - n_pad.x);

	object->transform.position.x = -x;
	object->size = layout::player_quad;

	if (m_weapon = entity()->find<Weapon>(); !m_weapon) { m_weapon = entity()->attach<Weapon>(); }

	ship = entity()->attach<SpriteRenderer<>>();
	ship->get().set_sheet(tg::locate<Resources*>()->find<vf::Sprite::Sheet>(ship_sprite_sheet));
}

void Player::translate(glm::vec2 n_xy) { m_dxy += n_xy; }

void Player::fire() {
	auto const offset = ship->get().size() * n_muzzle_offset;
	m_weapon->fire(ship->get().transform().position + offset);
}

void Player::tick(tg::DeltaTime dt) {
	auto* world = static_cast<World*>(scene());
	auto const state = m_controller.update();
	auto const pos = object->transform.position + (m_dxy + state.xy) * speed * dt.scaled.count();
	auto const ha = world->area() * 0.5f * (1.0f - n_pad);
	object->transform.position = glm::clamp(pos, -ha, ha);

	if (state.flags.test(Controller::Flag::eFire)) { fire(); }

	m_dxy = {};
}
} // namespace pew
