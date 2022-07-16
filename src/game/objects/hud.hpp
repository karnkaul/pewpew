#pragma once
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
	void tick(vf::Time) override;
	void draw(vf::Frame const& frame) const override;

	vf::Mesh m_border{};
	vf::Text m_score{};
	Ptr<ProgressBar> m_hp{};
};
} // namespace pew
