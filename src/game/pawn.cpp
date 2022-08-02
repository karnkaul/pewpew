#include <game/pawn.hpp>
#include <game/world.hpp>

namespace pew {
void Pawn::setup() { quad = vf::Mesh(m_world->vfContext(), "pawn"); }

Pawn& Pawn::setSize(glm::vec2 size) {
	m_size = size;
	quad.gbo.write(vf::Geometry::make_quad({m_size}));
	return *this;
}
} // namespace pew
