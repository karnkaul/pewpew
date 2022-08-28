#include <engine/frame.hpp>
#include <game/attachments/progress_bar.hpp>
#include <tardigrade/services.hpp>

namespace pew {
void ProgressBar::setup() {
	tg::RenderAttachment::setup();
	auto const* device = tg::locate<vf::GfxDevice const*>();
	m_border = vf::Mesh{*device};
	m_bg = vf::Mesh{*device};
	m_bar = vf::Mesh{*device};

	set_size({200.0f, 20.0f});
}

ProgressBar& ProgressBar::set_size(glm::vec2 size) {
	auto qci = vf::QuadCreateInfo{};
	qci.size = size + glm::vec2(10.0f, 10.0f);
	m_border.buffer.write(vf::Geometry::make_quad(qci));
	qci.size = size;
	m_bg.buffer.write(vf::Geometry::make_quad(qci));
	qci.origin.x = qci.size.x * 0.5f;
	m_bar.buffer.write(vf::Geometry::make_quad(qci));
	m_bar_offset_x = -qci.origin.x;
	m_size = size;
	tick({});
	return *this;
}

void ProgressBar::tick(tg::DeltaTime) {
	m_bar.storage.transform.position = m_border.storage.transform.position = m_bg.storage.transform.position = position;
	m_bar.storage.transform.position.x += m_bar_offset_x;
	m_bar.storage.transform.scale.x = std::clamp(progress, 0.0f, 1.0f);

	m_border.storage.tint = border;
	m_bg.storage.tint = bg;
	m_bar.storage.tint = bar;
}

void ProgressBar::render(tg::RenderTarget const& target) const {
	auto const& frame = static_cast<Frame const&>(target).frame;
	frame.draw(m_border);
	frame.draw(m_bg);
	frame.draw(m_bar);
}
} // namespace pew
