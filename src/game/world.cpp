#include <core/log.hpp>
#include <engine/context.hpp>
#include <game/world.hpp>
#include <algorithm>
#include <fstream>

namespace pew {
namespace {
bool valid(std::string_view uri) {
	if (uri.empty()) {
		plog::warn("Empty URI");
		return false;
	}
	return true;
}

template <typename... T>
void populate(std::vector<Ptr<GameObject const>>& out, T const&... ts) {
	out.clear();
	out.reserve((... + ts.size()));
	auto fill = [&out](auto const& v) {
		for (auto const& t : v) { out.push_back(t.get()); }
	};
	(fill(ts), ...);
	std::sort(out.begin(), out.end(), [](Ptr<GameObject const> a, Ptr<GameObject const> b) { return a->layer < b->layer; });
}
} // namespace

World::World(Context& context) : m_context(context) {
	m_player = spawn<Player>();
	m_hud = spawn<Hud>();
	m_background = spawn<Background>();
}

void World::attach(Ptr<KeyListener> listener) {
	if (!listener) { return; }
	listener->m_world = this;
	m_keyListeners.push_back(listener);
}

void World::detach(Ptr<KeyListener> listener) { std::erase(m_keyListeners, listener); }

void World::handle(vf::EventQueue const& events) {
	for (auto const& event : events.events) {
		switch (event.type) {
		case vf::EventType::eKey: {
			auto const& key = event.get<vf::EventType::eKey>();
			for (auto* listener : m_keyListeners) { (*listener)(key); }
			onKey(key);
			break;
		}
		case vf::EventType::eFramebufferResize: m_context.basis.scale = m_context.basisScale(event.get<vf::EventType::eFramebufferResize>()); break;
		default: break;
		}
	}
}

void World::onKey(vf::KeyEvent const& key) {
	if (m_uiView) {
		m_uiView->onKey(key);
		return;
	}
	m_keyboard.onKey(key);
}

void World::tick(vf::Time dt) {
	auto const dlt = DeltaTime{dt, dt * timeScale};
	if (m_uiView) {
		m_uiView->tick(dlt);
		if (m_uiView->m_destroyed) { m_uiView.reset(); }
		return;
	}
	doTick(m_statusEffects, dlt);
	doTick(m_gameObjects, dlt);
	doTick(m_damagers, dlt);
}

void World::render(vf::Frame const& frame) const {
	populate(m_drawList, m_gameObjects, m_damagers);
	for (auto const* gameObject : m_drawList) { gameObject->draw(frame); }
	if (m_uiView) { m_uiView->draw(frame); }
}

Basis const& World::basis() const { return m_context.basis; }
glm::vec2 World::extent() const { return m_context.basis.space * m_context.basis.scale; }
glm::vec2 World::area() const { return layout::nWorldArea * extent(); }

vf::Context const& World::vfContext() const { return m_context.vfContext; }
Audio& World::audio() const { return m_context.audio; }

bool World::load(std::string& out, std::string_view uri) {
	auto file = std::ifstream(dataPath(m_context.env, uri));
	if (!file) {
		plog::warn("Failed to read file [{}]", uri);
		return false;
	}
	for (std::string line; std::getline(file, line); line.clear()) {
		out += std::move(line);
		out += '\n';
	}
	return true;
}

bool World::load(vf::Image& out, std::string_view uri) {
	if (!valid(uri)) { return {}; }
	return out.load(dataPath(m_context.env, uri).c_str()).has_value();
}

bool World::load(vf::Texture& out, std::string_view uri, vf::TextureCreateInfo const& tci) {
	auto image = vf::Image{};
	if (!load(image, uri)) { return {}; }
	if (out) { return out.create(image).has_value(); }
	out = vf::Texture(vfContext(), std::string(uri), image, tci);
	return true;
}

bool World::load(vf::Ttf& out, std::string_view uri) {
	auto path = dataPath(m_context.env, uri);
	if (!out) { out = vf::Ttf(m_context.vfContext, std::string(uri)); }
	if (!out.load(dataPath(m_context.env, uri).c_str())) { plog::warn("Failed to load Ttf [{}]", uri); }
	return true;
}

bool World::load(capo::Sound& out, std::string_view uri) {
	auto path = dataPath(m_context.env, uri);
	auto pcm = capo::PCM::from_file(dataPath(m_context.env, uri).c_str());
	if (!pcm) {
		plog::warn("Failed to load PCM [{}]", uri);
		return false;
	}
	out = m_context.capoInstance->make_sound(*pcm);
	return true;
}

template <typename C>
void World::doTick(C& out, DeltaTime const dt) {
	for (auto& obj : out) { static_cast<GameObject*>(obj.get())->tick(dt); }
	std::erase_if(out, [](auto const& obj) { return obj->m_destroyed; });
}

void World::setup(GameObject& out) {
	out.m_world = this;
	out.setup();
}
} // namespace pew
