#include <game/objects/drifter.hpp>
#include <game/world.hpp>

namespace pew {
namespace {
constexpr bool oob(glm::vec2 const extent, glm::vec2 const position, glm::vec2 const size) {
	auto const he = extent * 0.5f;
	auto const hs = size * 0.5f;
	auto const total = he + hs;
	return std::abs(position.x) > total.x || std::abs(position.y) > total.y;
}
} // namespace

void Drifter::setup() {
	Pawn::setup();
	auto const extent = m_world->area();
	quad.instance.transform.position.x = extent.x * 0.5f;
}

void Drifter::tick(DeltaTime dt) {
	quad.instance.transform.position += speed * dt.scaled.count();
	if (intersecting(m_world->player()->rect())) { onHit(*m_world->player()); }
	if (oob(m_world->area(), quad.instance.transform.position, size())) { destroy(); }
}
} // namespace pew
