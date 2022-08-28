#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/hud.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/progress_bar.hpp>
#include <game/layers.hpp>
#include <game/world.hpp>
#include <ktl/kformat.hpp>
#include <tardigrade/services.hpp>
#include <util/defines.hpp>
#include <vulkify/ttf/ttf.hpp>
#include <iomanip>
#include <sstream>

namespace pew {
namespace {
std::string format_elapsed(vf::Time elapsed) {
	auto const h = std::chrono::duration_cast<std::chrono::hours>(elapsed);
	auto const m = std::chrono::duration_cast<std::chrono::minutes>(elapsed) - h;
	auto const s = std::chrono::duration_cast<std::chrono::seconds>(elapsed) - h - m;
	auto str = std::stringstream{};
	str << h.count();
	str << std::setfill('0') << ':' << std::setw(2) << m.count() << ':' << std::setw(2) << s.count();
	return str.str();
}

auto const main_font = Signature{"fonts/main.otf"};
} // namespace

void Hud::setup() {
	tg::RenderAttachment::setup();
	auto const* device = tg::locate<vf::GfxDevice const*>();
	auto const* resources = tg::locate<Resources*>();
	m_border = vf::Mesh{*device};
	m_score = vf::Text{*device};
	layer = layers::hud;

	auto* world = static_cast<World*>(scene());
	auto geometry = vf::Geometry{};
	auto qci = vf::QuadCreateInfo{};
	auto const extent = World::extent_v;
	qci.size = {extent.x, (1.0f - layout::n_world_area.y) * 0.5f * extent.y};
	qci.origin.y = (world->area().y + qci.size.y) * 0.5f;
	geometry.add_quad(qci);
	auto const rect = vf::Rect{qci.size, qci.origin};
	qci.origin.y = -qci.origin.y;
	geometry.add_quad(qci);
	m_border.buffer.write(std::move(geometry));

	auto* ttf = resources->find<vf::Ttf>(main_font);
	if (ttf) { m_score.set_ttf(ttf->handle()); }
	auto const height = vf::Text::Height{40};
	m_score.set_height(height);
	m_score.transform().position = rect.offset;

	m_hp = entity()->attach<ProgressBar>();
	m_hp->layer = layers::hud + 1;
	auto const hp_size = glm::vec2(200.0f, 20.0f);
	m_hp->position = rect.offset;
	m_hp->position.x -= hp_size.x * 2.0f;
	m_hp->set_size(hp_size);

	if constexpr (debug_v) {
		m_fps = vf::Text{*device};
		if (ttf) { m_fps.set_ttf(ttf->handle()); }
		m_fps.set_height(vf::Text::Height{static_cast<std::uint32_t>(height) / 2});
		m_fps.transform().position = rect.offset;
		m_fps.transform().position.x = rect.extent.x * 0.5f - 100.0f;
		m_fps.set_align({.horz = vf::Text::Horz::eRight});
	}
}

void Hud::tick(tg::DeltaTime dt) {
	auto* world = static_cast<World*>(scene());
	m_border.storage.tint = tint;
	m_hp->progress = static_cast<float>(world->player()->hp) / static_cast<float>(Player::max_hp_v);

	m_score.set_string(std::to_string(world->player()->score));

	m_framerate.tick(dt.real);
	m_elapsed += dt.real;
	m_fps.set_string(ktl::kformat("{} FPS / {}", m_framerate.fps(), format_elapsed(m_elapsed)));
}

void Hud::render(tg::RenderTarget const& target) const {
	auto const& frame = static_cast<Frame const&>(target).frame;
	frame.draw(m_border);
	frame.draw(m_score);
	if constexpr (debug_v) { frame.draw(m_fps); }
}
} // namespace pew
