#pragma once
#include <core/basis.hpp>
#include <engine/keyboard.hpp>
#include <game/layout.hpp>
#include <game/objects/background.hpp>
#include <game/objects/consumable.hpp>
#include <game/objects/damager.hpp>
#include <game/objects/hud.hpp>
#include <game/objects/player.hpp>
#include <game/resources.hpp>
#include <game/status_effect.hpp>
#include <ktl/kunique_ptr.hpp>
#include <concepts>
#include <span>
#include <vector>

namespace pew {
struct Context;
class Audio;

class KeyListener {
  public:
	virtual ~KeyListener() = default;

	virtual void operator()(vf::KeyEvent const& key) = 0;

  protected:
	Ptr<World> m_world{};

	friend class World;
};

class World {
  public:
	World(Context& context);

	template <std::derived_from<GameObject> T, typename... Args>
		requires(std::is_constructible_v<T, Args...>)
	Ptr<T> spawn(Args&&... args) {
		if constexpr (std::derived_from<T, Damager>) {
			return spawnImpl<T>(m_damagers, std::forward<Args>(args)...);
		} else if constexpr (std::derived_from<T, StatusEffect>) {
			return spawnImpl<T>(m_statusEffects, std::forward<Args>(args)...);
		} else {
			return spawnImpl<T>(m_gameObjects, std::forward<Args>(args)...);
		}
	}

	template <std::derived_from<UiView> T, typename... Args>
		requires(std::is_constructible_v<T, Args...>)
	Ptr<T> setView(Args&&... args) {
		auto t = spawnAndSetup<T>(std::forward<Args>(args)...);
		auto ret = t.get();
		m_uiView = std::move(t);
		return ret;
	}

	std::span<ktl::kunique_ptr<Damager> const> damagers() const { return m_damagers; }

	void attach(Ptr<KeyListener> listener);
	void detach(Ptr<KeyListener> listener);

	void handle(vf::EventQueue const& events);
	void onKey(vf::KeyEvent const& key);
	void tick(vf::Time dt);
	void render(vf::Frame const& frame) const;

	Basis const& basis() const;
	glm::vec2 extent() const;
	glm::vec2 area() const;

	vf::Context const& vfContext() const;
	Audio& audio() const;
	Context& context() const { return m_context; }
	Keyboard const& keyboard() const { return m_keyboard; }

	Ptr<Player> player() const { return m_player; }
	Ptr<Hud> hud() const { return m_hud; }
	Ptr<Background> background() const { return m_background; }

	bool load(std::string& out, std::string_view uri);
	bool load(vf::Image& out, std::string_view uri);
	bool load(vf::Texture& out, std::string_view uri, vf::TextureCreateInfo const& tci = {});
	bool load(vf::Ttf& out, std::string_view uri);
	bool load(capo::Sound& out, std::string_view uri);

	Resources resources{};
	float timeScale{1.0f};

  private:
	template <typename T, typename C, typename... Args>
	Ptr<T> spawnImpl(C& out, Args&&... args) {
		auto t = spawnAndSetup<T>(std::forward<Args>(args)...);
		auto ret = t.get();
		out.push_back(std::move(t));
		return ret;
	}

	template <typename T, typename... Args>
	ktl::kunique_ptr<T> spawnAndSetup(Args&&... args) {
		auto t = ktl::make_unique<T>(std::forward<Args>(args)...);
		setup(*t);
		return t;
	}

	template <typename C>
	void doTick(C& out, vf::Time const dt);

	void setup(GameObject& out);

	Context& m_context;
	Keyboard m_keyboard{};
	Ptr<Player> m_player{};
	Ptr<Hud> m_hud{};
	Ptr<Background> m_background{};
	std::vector<ktl::kunique_ptr<GameObject>> m_gameObjects{};
	std::vector<ktl::kunique_ptr<Damager>> m_damagers{};
	std::vector<ktl::kunique_ptr<StatusEffect>> m_statusEffects{};
	std::vector<Ptr<KeyListener>> m_keyListeners{};
	mutable std::vector<Ptr<GameObject const>> m_drawList{};
	ktl::kunique_ptr<UiView> m_uiView{};
};
} // namespace pew
