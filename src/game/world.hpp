#pragma once
#include <game/layout.hpp>
#include <game/manifest.hpp>
#include <ktl/kunique_ptr.hpp>
#include <tardigrade/scene.hpp>
#include <util/ptr.hpp>
#include <util/signature.hpp>
#include <concepts>
#include <span>
#include <vector>

namespace pew {
class Player;
class Background;
class Hud;

class World : public tg::Scene {
  public:
	static constexpr auto extent_v = glm::vec2{1280.0f, 720.0f};

	Ptr<Background> background() const;
	Ptr<Hud> hud() const;
	Ptr<Player> player() const;

	glm::vec2 area() const;

	Manifest<Signature> manifest{};

  private:
	void setup() override;

	tg::Entity::Id m_background{};
	tg::Entity::Id m_hud{};
	tg::Entity::Id m_player{};
};
} // namespace pew
