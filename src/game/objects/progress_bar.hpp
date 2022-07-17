#pragma once
#include <game/game_object.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>

namespace pew {
class ProgressBar : public GameObject {
  public:
	ProgressBar& setSize(glm::vec2 size);
	glm::vec2 size() const { return m_size; }

	glm::vec2 position{};
	float nValue{};

	vf::Rgba bar{vf::cyan_v};
	vf::Rgba bg{vf::white_v};
	vf::Rgba border{vf::black_v};

  protected:
	void setup() override;
	void tick(DeltaTime) override;
	void draw(vf::Frame const& frame) const override;

	vf::Mesh m_bar{};
	vf::Mesh m_bg{};
	vf::Mesh m_border{};
	glm::vec2 m_size{};
	float m_barOffsetX{};
};
} // namespace pew
