#include <engine/context.hpp>
#include <game/attachments/background.hpp>
#include <game/attachments/hud.hpp>
#include <game/attachments/player.hpp>
#include <game/world.hpp>
#include <util/logger.hpp>
#include <algorithm>
#include <fstream>

namespace pew {
void World::setup() {
	manifest = *tg::locate<Manifest<Signature>*>();
	auto* background = spawn<Background>();
	m_background = background->id();

	auto* player = spawn<Player>();
	m_player = player->id();

	auto* hud = spawn<Hud>();
	m_hud = hud->id();
}

Ptr<Background> World::background() const {
	if (auto entity = find(m_background)) {
		auto ret = entity->find<Background>();
		return ret;
	}
	return {};
}

Ptr<Player> World::player() const {
	if (auto entity = find(m_player)) { return entity->find<Player>(); }
	return {};
}

glm::vec2 World::area() const { return layout::n_world_area * extent_v; }
} // namespace pew
