#include <game/objects/progress_bar.hpp>
#include <game/world.hpp>

namespace pew {
void ProgressBar::setup() {
	m_border = vf::Mesh(m_world->vfContext(), "progress_border");
	m_bg = vf::Mesh(m_world->vfContext(), "progress_cover");
	m_bar = vf::Mesh(m_world->vfContext(), "progress_bar");

	setSize({200.0f, 20.0f});
}

ProgressBar& ProgressBar::setSize(glm::vec2 size) {
	auto qci = vf::QuadCreateInfo{};
	qci.size = size + glm::vec2(10.0f, 10.0f) * m_world->basis().scale;
	m_border.gbo.write(vf::Geometry::makeQuad(qci));
	qci.size = size;
	m_bg.gbo.write(vf::Geometry::makeQuad(qci));
	qci.origin.x = qci.size.x * 0.5f;
	m_bar.gbo.write(vf::Geometry::makeQuad(qci));
	m_barOffsetX = -qci.origin.x;
	m_size = size;
	tick({});
	return *this;
}

void ProgressBar::tick(DeltaTime) {
	m_bar.instance.transform.position = m_border.instance.transform.position = m_bg.instance.transform.position = position;
	m_bar.instance.transform.position.x += m_barOffsetX;
	m_bar.instance.transform.scale.x = std::clamp(nValue, 0.0f, 1.0f);

	m_border.instance.tint = border;
	m_bg.instance.tint = bg;
	m_bar.instance.tint = bar;
}

void ProgressBar::draw(vf::Frame const& frame) const {
	frame.draw(m_border);
	frame.draw(m_bg);
	frame.draw(m_bar);
}
} // namespace pew
