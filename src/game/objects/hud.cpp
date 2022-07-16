#include <game/layers.hpp>
#include <game/objects/hud.hpp>
#include <game/world.hpp>
#include <ktl/kformat.hpp>

namespace pew {
void Hud::setup() {
	m_border = vf::Mesh(m_world->vfContext(), "background_border");
	m_score = vf::Text(m_world->vfContext(), "score");
	layer = layers::hudLayer;

	auto geometry = vf::Geometry{};
	auto qci = vf::QuadCreateInfo{};
	auto const extent = m_world->extent();
	qci.size = {extent.x, (1.0f - layout::nWorldArea.y) * 0.5f * extent.y};
	qci.origin.y = (m_world->area().y + qci.size.y) * 0.5f;
	geometry.addQuad(qci);
	auto const rect = vf::Rect{qci.size, qci.origin};
	qci.origin.y = -qci.origin.y;
	geometry.addQuad(qci);
	m_border.gbo.write(std::move(geometry));

	m_score.setFont(&m_world->resources.fonts.main);
	float const height = 40.0f * m_world->basis().scale;
	m_score.setHeight(static_cast<vf::Text::Height>(height));
	m_score.transform().position = rect.offset;

	m_hp = m_world->spawn<ProgressBar>();
	m_hp->layer = layers::hudLayer + 1;
	auto const hpSize = glm::vec2(200.0f, 20.0f) * m_world->basis().scale;
	m_hp->position = rect.offset;
	m_hp->position.x -= hpSize.x * 2.0f;
	m_hp->setSize(hpSize);
}

void Hud::tick(vf::Time) {
	m_border.instance.tint = tint;
	m_hp->nValue = m_world->player()->hp / 100.0f;

	m_score.setString(std::to_string(m_world->player()->score));
}

void Hud::draw(vf::Frame const& frame) const {
	frame.draw(m_border);
	frame.draw(m_score);
}
} // namespace pew
