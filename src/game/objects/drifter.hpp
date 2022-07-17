#pragma once
#include <game/pawn.hpp>

namespace pew {
class Player;

class Drifter : public Pawn {
  public:
	glm::vec2 speed{-100.0f, 0.0f};

	void setup() override;
	void tick(DeltaTime dt) override;

  private:
	virtual void onHit(Player&) = 0;
};
} // namespace pew
