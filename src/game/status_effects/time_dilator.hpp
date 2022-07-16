#pragma once
#include <game/status_effect.hpp>

namespace pew {
class TimeDilator : public StatusEffect {
  public:
	float timeScale{0.33f};

  private:
	void apply(vf::Time) override;
	void unapply(vf::Time) override;
};
} // namespace pew
