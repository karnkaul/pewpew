#include <game/autopilot.hpp>
#include <game/world.hpp>
#include <glm/gtx/norm.hpp>

namespace pew {
namespace {
constexpr bool isAhead(glm::vec2 const player, glm::vec2 const damager) { return damager.x > player.x; }
constexpr bool inLineOfSight(glm::vec2 const player, vf::Rect const& damager) {
	return player.y >= damager.bottom_left().y && player.y <= damager.top_left().y;
}

struct Actions {
	float dy{};
	bool fire{};
};

struct Entry {
	vf::Rect rect{};
	glm::vec2 distance{};

	explicit operator bool() const { return rect.extent.x > 0.0f && rect.extent.y > 0.0f; }
};

bool operator<(Entry const& lhs, Entry const& rhs) {
	if (lhs.distance.x > rhs.distance.x) { return false; }
	return lhs.distance.x < rhs.distance.x || lhs.distance.y < rhs.distance.y;
}

vf::Rect getRect(Pawn const& pawn) { return vf::Rect{pawn.size(), pawn.quad.instance.transform.position}; }

Entry makeEntry(Damager const& damager, glm::vec2 const player) {
	auto ret = Entry{getRect(damager)};
	auto const& pos = damager.quad.instance.transform.position;
	if (!isAhead(player, pos)) { return {}; }
	ret.distance = pos - player;
	return ret;
}

Actions getActions(std::span<ktl::kunique_ptr<Damager> const> damagers, vf::Rect const& player, float const minDistanceY = 5.0f) {
	auto ret = Actions{};
	auto closest = Entry{};
	for (auto const& damager : damagers) {
		auto const entry = makeEntry(*damager, player.offset);
		if (!ret.fire && inLineOfSight(player.offset, entry.rect)) { ret.fire = true; }
		if (!closest || entry < closest) { closest = entry; }
	}
	if (closest && std::abs(closest.distance.y) > minDistanceY) { ret.dy = closest.distance.y > 0.0f ? 1.0f : -1.0f; }
	return ret;
}
} // namespace

void Autopilot::tick(World& world, vf::Time const dt) {
	auto& player = *world.player();
	auto& position = player.quad.instance.transform.position;
	auto const actions = getActions(world.damagers(), getRect(player));
	if (flags.test(Flag::eMove) && actions.dy != 0.0f) { position.y += player.speed.y * dt.count() * actions.dy; }
	if (flags.test(Flag::eFire) && actions.fire) { player.weapon->fire(position + player.nMuzzleOffset * player.size()); }
}
} // namespace pew
