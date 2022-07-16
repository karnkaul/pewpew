#include <core/log.hpp>
#include <game/layers.hpp>
#include <game/objects/damager.hpp>
#include <game/world.hpp>

namespace pew {
void Damager::setup() {
	Drifter::setup();
	layer = layers::damagerLayer;
}

void Damager::onHit(Player& player) {
	plog::info("player damage: {} instigator: {}", damage, m_name);
	player.takeDamage(damage);
	onDestroy();
	destroy();
}
} // namespace pew
