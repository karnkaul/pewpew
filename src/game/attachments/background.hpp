#pragma once
#include <tardigrade/render_attachment.hpp>
#include <util/signature.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>
#include <vulkify/graphics/texture.hpp>

namespace pew {
class Background : public tg::RenderAttachment {
  public:
	bool set(vf::Texture const& texture);

	float speed{20.0f};

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;
	void render(tg::RenderTarget const& target) const override;

	vf::Texture m_texture{};
	std::vector<vf::Mesh> m_tiles{};
	glm::vec2 m_size{};
	float m_speed{};
};
} // namespace pew
