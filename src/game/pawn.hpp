#pragma once
#include <game/game_object.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>

namespace pew {
class Pawn : public GameObject {
  public:
	vf::Mesh quad{};

	glm::vec2 size() const { return m_size; }
	Pawn& setSize(glm::vec2 size);

	vf::Rect rect() const { return {{m_size, quad.instance.transform.position}}; }

	bool intersecting(glm::vec2 point) const;
	bool intersecting(vf::Rect rect) const;

  protected:
	void setup() override;
	void draw(vf::Frame const& frame) const override { frame.draw(quad); }

  private:
	glm::vec2 m_size{};
};
} // namespace pew
