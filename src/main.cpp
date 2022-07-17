#include <core/collection.hpp>
#include <core/log.hpp>
#include <engine/context.hpp>
#include <game/objects/background.hpp>
#include <game/objects/progress_bar.hpp>
#include <game/world.hpp>
#include <ktl/fixed_vector.hpp>
#include <random>

using namespace std::chrono_literals;

namespace {
std::optional<pew::Context> makeContext(int argc, char const* const argv[]) {
	auto builder = vf::Builder{};
	// builder.setExtent({1920, 1080});
	auto vf = builder.build();
	if (!vf) {
		plog::error("Failed to create vulkify instance");
		return {};
	}
	auto ret = pew::Context{pew::Env::make(argc, argv), {}, std::move(*vf)};
	ret.capoInstance = ktl::make_unique<capo::Instance>();
	ret.basis.scale = ret.basisScale(ret.vfContext.framebufferExtent());
	ret.audio = *ret.capoInstance;
	return ret;
}

[[maybe_unused]] float randomRange(float min, float max) {
	static auto eng = std::default_random_engine(std::random_device{}());
	return std::uniform_real_distribution<float>(min, max)(eng);
}

template <typename T>
[[maybe_unused]] void loadAllFrom(pew::World& world, pew::Collection<T>& out, std::string_view dir, std::string_view ext) {
	for (auto& file : pew::fileList(world.context().env, dir, ext)) {
		T t;
		if (world.load(t, file)) { out.push(std::move(t)); }
	}
}

struct DebugControls : pew::KeyListener {
	static constexpr glm::vec2 damagerSize{75.0f, 75.0f};
	static constexpr glm::vec2 consumableSize{40.0f, 40.0f};

	void operator()(vf::KeyEvent const& key) override {
		auto const zone = m_world->area() * 0.25f;
		if (key(vf::Key::eEnter, vf::Action::ePress)) {
			auto damager = m_world->spawn<pew::Damager>();
			damager->setSize(m_world->context().basis.scale * damagerSize);
			damager->quad.instance.transform.position.y = randomRange(-zone.y, zone.y);
		}
		if (key(vf::Key::eBackslash, vf::Action::ePress)) {
			auto consumable = m_world->spawn<pew::Consumable>();
			consumable->setSize(m_world->context().basis.scale * consumableSize);
			consumable->quad.instance.tint = vf::red_v;
			consumable->quad.instance.transform.position.y = randomRange(-zone.y, zone.y);
		}
		if (key(vf::Key::eW, vf::Action::eRelease, vf::Mod::eCtrl)) { m_world->context().vfContext.close(); }
		if (key(vf::Key::eP, vf::Action::eRelease, vf::Mod::eCtrl)) {
			auto const vsync = m_world->context().vfContext.vsync() == vf::VSync::eOff ? vf::VSync::eOn : vf::VSync::eOff;
			m_world->context().vfContext.setVSync(vsync);
		}
	}
};

void loadResources(pew::World& out) {
	out.load(out.resources.textures.ship, "textures/blueships1.png");
	out.load(out.resources.fonts.main, "fonts/main.ttf");
	out.load(out.resources.sfx.fire, "sfx/laserfire/01.wav");
}

void run(pew::Context context) {
	auto world = pew::World(context);
	auto debug = DebugControls{};
	loadResources(world);

	world.player()->quad.texture = world.resources.textures.ship.handle();
	world.player()->setSize(context.basis.scale * pew::layout::playerQuad);
	world.player()->weapon->mesh().gbo.write(vf::Geometry::makeQuad({context.basis.scale * pew::layout::projectileQuad}));

	world.background()->open("textures/background.png");

	world.audio().setSfxGain(0.2f);

	if constexpr (pew::debug_v) { world.attach(&debug); }

	context.vfContext.show();
	while (!context.vfContext.closing()) {
		auto frame = context.vfContext.frame();
		world.handle(frame.poll());
		world.tick(frame.dt());
		world.render(frame);
	}
}
} // namespace

int main(int argc, char* argv[]) {
	auto context = makeContext(argc, argv);
	if (!context) { return EXIT_FAILURE; }
	run(std::move(*context));
}
