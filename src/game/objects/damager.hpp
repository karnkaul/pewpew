#pragma once
#include <game/objects/drifter.hpp>

namespace pew {
class Damager : public Drifter {
  public:
	float damage{10.0f};
	int points{10};

  protected:
	std::string_view m_name{"generic_damager"};

	virtual void onDestroy() {}

	void setup() override;
	void onHit(Player& out) override;
};
} // namespace pew
