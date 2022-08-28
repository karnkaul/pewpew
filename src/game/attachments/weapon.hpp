#pragma once
#include <engine/instanced.hpp>
#include <tardigrade/render_attachment.hpp>

namespace pew {
using namespace std::chrono_literals;

class Weapon : public tg::RenderAttachment {
  public:
	vf::Time fire_delay{0.5s};
	int damage{1};

	bool fire(glm::vec2 position, glm::vec2 velocity = {3000.0f, 0.0f}, vf::Time ttl = 1s);
	auto& mesh() const { return m_projectiles.mesh; }

	bool consume_hit(vf::Rect const& rect);

  private:
	struct Projectile {
		struct {
			glm::vec2 current;
			glm::vec2 target;
		} velocity{};
		vf::Time ttl{};
		vf::Time elapsed{};
		bool destroyed{};
	};

	Instanced<Projectile> m_projectiles{};
	vf::Time m_fire_wait{};

	void setup() override;
	void tick(tg::DeltaTime dt) override;
	void render(tg::RenderTarget const& target) const override;
};
} // namespace pew
