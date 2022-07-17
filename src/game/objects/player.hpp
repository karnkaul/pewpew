#pragma once
#include <game/controller.hpp>
#include <game/objects/weapon.hpp>
#include <game/pawn.hpp>

namespace pew {
class Damager;

class Player final : public Pawn {
  public:
	Controller controller{};
	glm::vec2 speed{500.0f, 500.0f};
	glm::vec2 nMuzzleOffset{0.5f, 0.0f};
	glm::vec2 nPad{0.2f, 0.1f};
	float hp{100.0f};
	int score{0};

	Ptr<Weapon> weapon{};

	Player& takeDamage(float damage);
	Player& destroy(Damager& out);

  private:
	void setup() override;
	void tick(DeltaTime dt) override;
};
} // namespace pew
