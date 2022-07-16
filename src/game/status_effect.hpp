#pragma once
#include <game/game_object.hpp>

namespace pew {
using namespace std::chrono_literals;

class StatusEffect : public GameObject {
  public:
	vf::Time ttl{2s};

	vf::Time remain() const { return m_elapsed - ttl; }

  protected:
	virtual void apply(vf::Time dt) = 0;
	virtual void unapply(vf::Time dt) = 0;

  private:
	void setup() override {}

	void tick(vf::Time dt) override final {
		if ((m_elapsed += dt) >= ttl) {
			unapply(dt);
			destroy();
		} else {
			apply(dt);
		}
	}

	void draw(vf::Frame const&) const override {}

	vf::Time m_elapsed{};
};
} // namespace pew
