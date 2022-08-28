#pragma once
#include <engine/framerate.hpp>
#include <tardigrade/render_attachment.hpp>
#include <util/ptr.hpp>
#include <vulkify/graphics/primitives/text.hpp>

namespace pew {
class ProgressBar;

class Hud : public tg::RenderAttachment {
  public:
	vf::Rgba tint{vf::black_v};

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;
	void render(tg::RenderTarget const& target) const override;

	vf::Mesh m_border{};
	vf::Text m_score{};
	vf::Text m_fps{};
	Framerate m_framerate{};
	vf::Time m_elapsed{};
	Ptr<ProgressBar> m_hp{};
};
} // namespace pew
