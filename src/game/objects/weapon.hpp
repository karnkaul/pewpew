#pragma once
#include <engine/instanced.hpp>
#include <game/game_object.hpp>

namespace pew {
using namespace std::chrono_literals;

class Weapon : public GameObject {
  public:
	vf::Time fireDelay{0.5s};

	bool fire(glm::vec2 position, glm::vec2 velocity = {3000.0f, 0.0f}, vf::Time ttl = 1s);
	auto& mesh() const { return m_projectiles.mesh; }

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
	vf::Time m_fireWait{};

	void setup() override;
	void tick(vf::Time dt) override;
	void draw(vf::Frame const& frame) const override;
};
} // namespace pew
