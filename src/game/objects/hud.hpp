#pragma once
#include <core/framerate.hpp>
#include <game/game_object.hpp>
#include <game/objects/progress_bar.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>
#include <vulkify/graphics/primitives/text.hpp>

namespace pew {
class Hud final : public GameObject {
  public:
	vf::Rgba tint{vf::black_v};

  private:
	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;

	vf::Mesh m_border{};
	vf::Text m_score{};
	vf::Text m_fps{};
	Framerate m_framerate{};
	vf::Time m_elapsed{};
	Ptr<ProgressBar> m_hp{};
};
} // namespace pew
