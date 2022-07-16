#include <core/log.hpp>
#include <game/objects/consumable.hpp>

namespace pew {
void Consumable::onHit(Player& out) {
	plog::info("player status effect: {}", m_name);
	modulate(out);
	destroy();
}
} // namespace pew
