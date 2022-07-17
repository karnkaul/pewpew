#include <game/layers.hpp>
#include <game/objects/background.hpp>
#include <game/world.hpp>

namespace pew {
bool Background::open(std::string_view uri) {
	if (!m_world->load(m_texture, uri, {.filtering = vf::Filtering::eLinear})) { return false; }
	return set(m_texture);
}

bool Background::set(vf::Texture const& texture) {
	if (!texture) { return false; }
	auto const extent = glm::vec2(texture.extent());
	m_size.y = m_world->area().y;
	m_size.x = m_size.y * extent.x / extent.y;
	auto const count = static_cast<std::size_t>(m_world->area().x / m_size.x) + 2;
	auto x = (m_size.x - m_world->area().x) * 0.5f;
	m_tiles.clear();
	m_tiles.reserve(count);
	m_speed = {};
	for (std::size_t i = 0; i < count; ++i) {
		m_tiles.push_back(vf::Mesh(m_world->vfContext(), "background_" + std::to_string(i)));
		auto& mesh = m_tiles.back();
		mesh.gbo.write(vf::Geometry::makeQuad({m_size}));
		mesh.instance.transform.position.x = x;
		x += m_size.x;
		mesh.texture = texture.handle();
	}
	return true;
}

void Background::setup() { layer = layers::backgroundLayer; }

void Background::tick(DeltaTime dt) {
	if (m_tiles.empty()) { return; }

	for (auto& mesh : m_tiles) { mesh.instance.transform.position.x -= m_speed * dt.scaled.count(); }

	auto const oob = -(m_world->area().x + m_size.x) * 0.5f;
	auto& front = m_tiles.front();
	if (front.instance.transform.position.x <= oob) {
		front.instance.transform.position.x = m_tiles.back().instance.transform.position.x + m_size.x;
		std::rotate(m_tiles.begin(), m_tiles.begin() + 1, m_tiles.end());
	}

	m_speed = std::clamp(m_speed + dt.scaled.count() * 10.0f, 0.0f, speed);
}

void Background::draw(vf::Frame const& frame) const {
	for (auto const& tile : m_tiles) { frame.draw(tile); }
}
} // namespace pew
