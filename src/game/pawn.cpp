#include <game/pawn.hpp>
#include <game/world.hpp>

namespace pew {
namespace {
constexpr bool intersects(vf::Rect const rect, glm::vec2 const point) {
	auto const he = rect.extent * 0.5f;
	auto const bl = rect.offset - he;
	auto const tr = rect.offset + he;
	return point.x >= bl.x && point.x <= tr.x && point.y >= bl.y && point.y <= tr.y;
}

constexpr bool intersects(vf::Rect const a, vf::Rect const b) {
	return intersects(a, b.topLeft()) || intersects(a, b.topRight()) || intersects(a, b.bottomLeft()) || intersects(a, b.bottomRight());
}
} // namespace

void Pawn::setup() { quad = vf::Mesh(m_world->vfContext(), "pawn"); }

bool Pawn::intersecting(glm::vec2 point) const { return intersects(rect(), point); }

bool Pawn::intersecting(vf::Rect other) const {
	auto const self = rect();
	return intersects(other, self) || intersects(self, other);
}

Pawn& Pawn::setSize(glm::vec2 size) {
	m_size = size;
	quad.gbo.write(vf::Geometry::makeQuad({m_size}));
	return *this;
}
} // namespace pew
