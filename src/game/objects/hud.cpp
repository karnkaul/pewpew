#include <core/env.hpp>
#include <game/layers.hpp>
#include <game/objects/hud.hpp>
#include <game/world.hpp>
#include <ktl/kformat.hpp>
#include <sstream>

namespace pew {
namespace {
std::string formatElapsed(vf::Time elapsed) {
	auto const h = std::chrono::duration_cast<std::chrono::hours>(elapsed);
	auto const m = std::chrono::duration_cast<std::chrono::minutes>(elapsed) - h;
	auto const s = std::chrono::duration_cast<std::chrono::seconds>(elapsed) - h - m;
	auto str = std::stringstream{};
	str << h.count();
	str << std::setfill('0') << ':' << std::setw(2) << m.count() << ':' << std::setw(2) << s.count();
	return str.str();
}
} // namespace

void Hud::setup() {
	m_border = vf::Mesh(m_world->vfContext(), "background_border");
	m_score = vf::Text(m_world->vfContext(), "score");
	layer = layers::hudLayer;

	auto geometry = vf::Geometry{};
	auto qci = vf::QuadCreateInfo{};
	auto const extent = m_world->extent();
	qci.size = {extent.x, (1.0f - layout::nWorldArea.y) * 0.5f * extent.y};
	qci.origin.y = (m_world->area().y + qci.size.y) * 0.5f;
	geometry.add_quad(qci);
	auto const rect = vf::Rect{qci.size, qci.origin};
	qci.origin.y = -qci.origin.y;
	geometry.add_quad(qci);
	m_border.gbo.write(std::move(geometry));

	m_score.set_font(&m_world->resources.fonts.main);
	auto const height = static_cast<vf::Text::Height>(40.0f * m_world->basis().scale);
	m_score.set_height(height);
	m_score.transform().position = rect.offset;

	m_hp = m_world->spawn<ProgressBar>();
	m_hp->layer = layers::hudLayer + 1;
	auto const hpSize = glm::vec2(200.0f, 20.0f) * m_world->basis().scale;
	m_hp->position = rect.offset;
	m_hp->position.x -= hpSize.x * 2.0f;
	m_hp->setSize(hpSize);

	if constexpr (debug_v) {
		m_fps = vf::Text(m_world->vfContext(), "fps");
		m_fps.set_font(&m_world->resources.fonts.main);
		m_fps.set_height(height / 2);
		m_fps.transform().position = rect.offset;
		m_fps.transform().position.x = rect.extent.x * 0.5f - m_world->basis().scale * 100.0f;
		m_fps.set_align({.horz = vf::Text::Horz::eRight});
	}
}

void Hud::tick(DeltaTime dt) {
	m_border.instance.tint = tint;
	m_hp->nValue = m_world->player()->hp / 100.0f;

	m_score.set_string(std::to_string(m_world->player()->score));

	m_framerate.tick(dt.real);
	m_elapsed += dt.real;
	m_fps.set_string(ktl::kformat("{} FPS / {}", m_framerate.fps(), formatElapsed(m_elapsed)));
}

void Hud::draw(vf::Frame const& frame) const {
	frame.draw(m_border);
	frame.draw(m_score);
	if constexpr (debug_v) { frame.draw(m_fps); }
}
} // namespace pew
