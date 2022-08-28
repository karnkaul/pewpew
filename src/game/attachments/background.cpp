#include <engine/frame.hpp>
#include <engine/loader.hpp>
#include <engine/resources.hpp>
#include <game/attachments/background.hpp>
#include <game/layers.hpp>
#include <game/manifest.hpp>
#include <game/world.hpp>
#include <tardigrade/services.hpp>

namespace pew {
bool Background::set(vf::Texture const& texture) {
	if (!texture) { return false; }
	auto const extent = glm::vec2(texture.extent());
	auto* world = static_cast<World*>(scene());
	m_size.y = world->area().y;
	m_size.x = m_size.y * extent.x / extent.y;
	auto const count = static_cast<std::size_t>(world->area().x / m_size.x) + 2;
	auto x = (m_size.x - world->area().x) * 0.5f;
	m_tiles.clear();
	m_tiles.reserve(count);
	m_speed = {};
	for (std::size_t i = 0; i < count; ++i) {
		m_tiles.push_back(vf::Mesh{*tg::locate<vf::GfxDevice const*>()});
		auto& mesh = m_tiles.back();
		mesh.buffer.write(vf::Geometry::make_quad({m_size}));
		mesh.storage.transform.position.x = x;
		x += m_size.x;
		mesh.texture = texture.handle();
	}
	return true;
}

void Background::setup() {
	tg::RenderAttachment::setup();
	auto* world = static_cast<World*>(scene());
	layer = layers::background;
	if (auto texture = tg::locate<Resources*>()->find<vf::Texture>(world->manifest.textures.background)) { set(*texture); }
}

void Background::tick(tg::DeltaTime dt) {
	if (m_tiles.empty()) { return; }

	for (auto& mesh : m_tiles) { mesh.storage.transform.position.x -= m_speed * dt.scaled.count(); }

	auto* world = static_cast<World*>(scene());
	auto const oob = -(world->area().x + m_size.x) * 0.5f;
	auto& front = m_tiles.front();
	if (front.storage.transform.position.x <= oob) {
		front.storage.transform.position.x = m_tiles.back().storage.transform.position.x + m_size.x;
		std::rotate(m_tiles.begin(), m_tiles.begin() + 1, m_tiles.end());
	}

	m_speed = std::clamp(m_speed + dt.scaled.count() * 10.0f, 0.0f, speed);
}

void Background::render(tg::RenderTarget const& target) const {
	auto const& frame = static_cast<Frame const&>(target).frame;
	for (auto const& tile : m_tiles) { frame.draw(tile); }
}
} // namespace pew
