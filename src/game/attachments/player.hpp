#pragma once
#include <engine/animated_sprite.hpp>
#include <game/attachments/object.hpp>
#include <game/attachments/prop.hpp>
#include <game/attachments/renderer.hpp>
#include <game/controller.hpp>

namespace pew {
class Weapon;

class Player : public Prop {
  public:
	Ptr<Weapon> weapon() const { return m_weapon; }
	void translate(glm::vec2 n_xy);
	void fire();
	void take_damage(int damage);
	void score_points(std::uint64_t points);
	void heal(int points);

	static constexpr int max_hp_v = 100;

	glm::vec2 speed{500.0f, 500.0f};
	glm::vec2 n_pad{0.2f, 0.1f};
	glm::vec2 n_muzzle_offset{0.5f, 0.0f};

	int hp{max_hp_v};
	std::uint64_t score{0};

	Ptr<SpriteRenderer<>> ship{};

  private:
	void setup() override;
	void tick(tg::DeltaTime) override;

	Controller m_controller{};
	Ptr<Weapon> m_weapon{};
	glm::vec2 m_dxy{};
};
} // namespace pew
