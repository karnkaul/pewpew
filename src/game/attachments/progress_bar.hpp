#pragma once
#include <tardigrade/render_attachment.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>

namespace pew {
class ProgressBar : public tg::RenderAttachment {
  public:
	ProgressBar& set_size(glm::vec2 size);
	glm::vec2 size() const { return m_size; }

	glm::vec2 position{};
	float progress{};

	vf::Rgba bar{vf::cyan_v};
	vf::Rgba bg{vf::white_v};
	vf::Rgba border{vf::black_v};

  protected:
	void setup() override;
	void tick(tg::DeltaTime) override;
	void render(tg::RenderTarget const& target) const override;

	vf::Mesh m_bar{};
	vf::Mesh m_bg{};
	vf::Mesh m_border{};
	glm::vec2 m_size{};
	float m_bar_offset_x{};
};
} // namespace pew
