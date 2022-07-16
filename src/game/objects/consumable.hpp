#pragma once
#include <game/objects/drifter.hpp>

namespace pew {
class Consumable : public Drifter {
  protected:
	std::string_view m_name{"generic_consumable"};

	virtual void modulate(Player&) {}

	void onHit(Player& out) override;
};
} // namespace pew
