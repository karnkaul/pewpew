#include <engine/context.hpp>
#include <engine/frame.hpp>
#include <engine/manifest_loader.hpp>
#include <engine/resources.hpp>
#include <game/attachments/background.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/progress_bar.hpp>
#include <game/attachments/weapon.hpp>
#include <game/world.hpp>
#include <glm/gtx/norm.hpp>
#include <tardigrade/tardigrade.hpp>
#include <util/collection.hpp>
#include <util/defines.hpp>
#include <util/io.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>
#include <algorithm>
#include <filesystem>
#include <thread>

namespace pew {
class DriftLeft : public Prop {
	bool oob() const {
		auto const* world = static_cast<World const*>(scene());
		return object->transform.position.x < -0.5f * world->area().x;
	}

	void tick(tg::DeltaTime dt) override {
		object->transform.position.x -= 100.0f * dt.real.count();
		if (oob()) { entity()->destroy(); }
	}
};

class Damager : public Prop {
  public:
	Ptr<SpriteRenderer<>> sprite{};
	std::uint64_t points{10};
	int damage{10};
	int hp{2};

	std::uint64_t take_damage(int damage) {
		if ((hp -= damage) <= 0) {
			explode();
			return points;
		}
		return 0;
	}

	void explode() {
		entity()->destroy();
		// explosion
	}

  protected:
	void setup() override {
		Prop::setup();

		sprite = entity()->find_or_attach<SpriteRenderer<>>();

		object->size = {100.0f, 100.0f};
	}

	void tick(tg::DeltaTime) override {
		auto const* world = static_cast<World const*>(scene());
		auto player = world->player();
		if (player->weapon()->consume_hit(object->bounds())) { player->score_points(take_damage(player->weapon()->damage)); }
		if (player->object->bounds().intersects(object->bounds())) {
			player->take_damage(damage);
			explode();
		}
	}
};

class Powerup : public Prop {
  public:
	Ptr<SpriteRenderer<>> sprite{};

  protected:
	void setup() override {
		Prop::setup();

		sprite = entity()->find_or_attach<SpriteRenderer<>>();
		entity()->attach<DriftLeft>();

		object->size = {50.0f, 50.0f};
	}

	void tick(tg::DeltaTime) override {
		auto const* world = static_cast<World const*>(scene());
		auto player = world->player();
		if (player->object->bounds().intersects(object->bounds())) {
			on_hit(player);
			entity()->destroy();
		}
	}

	virtual void on_hit(Ptr<Player> player) = 0;
};

class HealthPowerup : public Powerup {
  public:
	int points{10};

	void on_hit(Ptr<Player> player) override { player->heal(points); }
};

class ObjectFactory : public tg::TickAttachment {
  public:
	struct Ahead {
		Ptr<Object> object{};
		float sqr_dist{};

		explicit operator bool() const { return object != nullptr; }
	};

	template <std::derived_from<tg::Attachment> T, typename... Args>
	Ptr<T> spawn(Args&&... args) {
		auto entity = scene()->spawn();
		auto* ret = entity->template attach<T>(std::forward<Args>(args)...);
		m_entries.push_back({entity->id(), entity->find_or_attach<Object>()});
		return ret;
	}

	Ahead next_ahead(glm::vec2 position) const {
		auto ret = Ahead{};
		for (auto const& entry : m_entries) {
			if (!scene()->contains(entry.entity) || entry.object->transform.position.x < position.x) { continue; }
			auto const sqr_dist = glm::length2(entry.object->transform.position - position);
			if (!ret || ret.sqr_dist > sqr_dist) { ret = {entry.object, sqr_dist}; }
		}
		return ret;
	}

	bool x_hit(float y) const {
		for (auto const& entry : m_entries) {
			if (scene()->contains(entry.entity)) {
				auto const bounds = entry.object->bounds();
				if (y >= bounds.bottom_left().y && y <= bounds.top_right().y) { return true; }
			}
		}
		return false;
	}

  private:
	void setup() override {}

	void tick(tg::DeltaTime) override {
		std::erase_if(m_entries, [s = scene()](Entry const& e) { return !s->contains(e.entity); });
	}

	struct Entry {
		tg::Entity::Id entity{};
		Ptr<Object> object{};
	};

	std::vector<Entry> m_entries{};
};

class Autopilot : public tg::TickAttachment {
  public:
	Ptr<ObjectFactory> damager_factory{};
	Ptr<ObjectFactory> powerup_factory{};

  protected:
	static constexpr float y_dir(glm::vec2 const player, glm::vec2 const damager, float const epsilon = 5.0f) {
		auto const dir = damager.y - player.y;
		if (std::abs(dir) > epsilon) { return dir < 0.0f ? -1.0f : 1.0f; }
		return 0.0f;
	}

	void setup() override {}

	void tick(tg::DeltaTime) override {
		if (!damager_factory || !powerup_factory) { return; }
		auto const* world = static_cast<World const*>(scene());
		auto player = world->player();
		auto const& player_pos = player->object->transform.position;
		auto const damager = damager_factory->next_ahead(player_pos);
		auto const powerup = powerup_factory->next_ahead(player_pos);
		auto dy = 0.0f;
		if (powerup && (!damager || damager.sqr_dist > powerup.sqr_dist)) {
			dy = y_dir(player_pos, powerup.object->transform.position);
		} else if (damager) {
			dy = y_dir(player_pos, damager.object->transform.position);
		}
		player->translate({0.0f, dy});
		if (damager_factory->x_hit(player_pos.y)) { player->fire(); }
	}
};

class Debug : public tg::TickAttachment {
	void setup() override {
		auto* damager_factory = entity()->scene()->spawn();
		m_damager_factory = damager_factory->attach<ObjectFactory>();
		auto* powerup_factory = entity()->scene()->spawn();
		m_powerup_factory = powerup_factory->attach<ObjectFactory>();

		auto* autopilot = entity()->scene()->spawn();
		m_autopilot = autopilot->attach<Autopilot>();
		m_autopilot->damager_factory = m_damager_factory;
		m_autopilot->powerup_factory = m_powerup_factory;
	}

	void tick(tg::DeltaTime) override {
		auto* world = static_cast<World*>(scene());
		auto const zone = 0.25f * world->area();
		if (vf::keyboard::released(vf::Key::eEscape)) { tg::locate<Context*>()->vf_context.close(); }
		if ((vf::keyboard::held(vf::Key::eLeftControl) || vf::keyboard::held(vf::Key::eRightControl)) && vf::keyboard::pressed(vf::Key::eW)) {
			tg::locate<Context*>()->vf_context.close();
		}

		if (vf::keyboard::released(vf::Key::eBackslash)) { world->player()->entity()->set_active(!world->player()->entity()->active()); }

		if (vf::keyboard::pressed(vf::Key::eEnter)) {
			if (vf::keyboard::held(vf::Key::eLeftControl)) {
				auto* powerup = m_powerup_factory->spawn<HealthPowerup>();
				powerup->object->transform.position.x = 0.5f * world->area().x;
				powerup->object->transform.position.y = pew::util::random_range(-zone.y, zone.y);
			} else {
				auto* damager = m_damager_factory->spawn<Damager>();
				damager->entity()->attach<DriftLeft>();
				damager->object->transform.position.x = 0.5f * world->area().x;
				damager->object->transform.position.y = pew::util::random_range(-zone.y, zone.y);
			}
		}

		if (vf::keyboard::pressed(vf::Key::eR)) { m_autopilot->entity()->set_active(!m_autopilot->entity()->active()); }
	}

	Ptr<ObjectFactory> m_damager_factory{};
	Ptr<ObjectFactory> m_powerup_factory{};
	Ptr<Autopilot> m_autopilot{};
};

namespace fs = std::filesystem;

fs::path find_data(fs::path start) {
	while (!start.empty() && start.parent_path() != start) {
		auto ret = start / "data";
		if (fs::is_directory(ret)) { return ret; }
		start = start.parent_path();
	}
	return {};
}

std::optional<Context> make_context(char const* arg0) {
	auto builder = vf::Builder{};
	// builder.setExtent({1920, 1080});
	builder.set_extent(World::extent_v).set_flag(vf::WindowFlag::eResizable);
	auto vf = builder.build();
	if (!vf) {
		logger::error("Failed to create vulkify instance");
		return {};
	}
	vf->camera().view.set_extent(World::extent_v);
	vf->lock_aspect_ratio(true);
	auto ret = Context{std::move(*vf)};

	io::mount_dir(find_data(arg0).string().c_str());
	ret.capo_instance = capo::Instance::make();
	ret.audio = *ret.capo_instance;
	return ret;
}

class InitLoader : public tg::TickAttachment {
	void setup() override {
		auto manifest = Manifest{};
		if (auto count = Manifest::load(manifest, "manifest.txt"); count > 0) { logger::info("[Manifest] Loaded [{}] entries", count); }

		m_loader.enqueue(std::move(manifest));

		auto const count = m_loader.load(1);
		logger::debug("Loading {} resources", count);

		m_progress = entity()->attach<ProgressBar>();
	}

	void tick(tg::DeltaTime) override {
		if (m_loader.idle()) {
			auto& world = tg::locate<tg::Director*>()->enqueue<World>();
			world.spawn<Debug>();
			return;
		}
		auto const current = m_loader.progress();
		if (!vf::FloatEq{}(m_progress->progress, current)) { m_progress->progress = current; }
	}

	ManifestLoader m_loader{};
	Ptr<ProgressBar> m_progress{};
};

void run(Context context) {
	auto services = tg::Services::Scoped{};
	tg::Services::provide(&context);
	tg::Services::provide(&context.vf_context.device());
	auto audio = tg::ServiceProvider<Audio>{*context.capo_instance};
	auto resources = tg::ServiceProvider<Resources>{};

	audio.set_sfx_gain(0.2f);
	auto director = tg::Director::Service{};
	auto& scene = director.enqueue<tg::Scene>();
	scene.spawn<InitLoader>();

	context.vf_context.show();
	while (!context.vf_context.closing()) {
		auto frame = Frame{};
		frame.frame = context.vf_context.frame();
		director.tick(frame.frame.dt());
		director.render(frame);
	}
}
} // namespace pew

#include <charconv>

// whitespace - ignore all
// string / char* - until whitespace / specified length

namespace {
namespace detail {
constexpr bool is_space(char c) { return c == ' ' || c == '\n' || c == '\r' || c == '\t'; }

constexpr std::string_view until_space(std::string_view& out_text) {
	std::size_t i = 0;
	for (; i < out_text.size() && !is_space(out_text[i]); ++i) {}
	auto ret = out_text.substr(0, i);
	out_text = i < out_text.size() ? out_text.substr(i) : std::string_view{};
	return ret;
}

using Scan = bool (*)(void*, std::string_view&);

template <typename T>
struct MakeScan {
	constexpr Scan operator()() const {
		return [](void* out_arg, std::string_view& out_str) {
			auto& out_t = *static_cast<T*>(out_arg);
			auto const [ptr, ec] = std::from_chars(out_str.data(), out_str.data() + out_str.size(), out_t);
			if (ec != std::errc()) { return false; }
			out_str = out_str.substr(static_cast<std::size_t>(ptr - out_str.data()));
			return true;
		};
	}
};

template <std::size_t N>
struct MakeScan<char[N]> {
	constexpr Scan operator()() const {
		return [](void* out_arg, std::string_view& out_str) {
			auto out_buf = static_cast<char*>(out_arg);
			auto const str = until_space(out_str);
			std::size_t i = 0;
			for (; i < str.size() && i + 1 < N; ++i) { out_buf[i] = str[i]; }
			out_buf[i] = '\0';
			return true;
		};
	}
};

template <>
struct MakeScan<std::string> {
	constexpr Scan operator()() const {
		return [](void* out_arg, std::string_view& out_str) {
			*static_cast<std::string*>(out_arg) = until_space(out_str);
			return true;
		};
	}
};

struct ScanArg {
	void* out_arg{};
	Scan scan{};

	template <typename T>
	static constexpr ScanArg make(T& out) {
		return {&out, MakeScan<T>{}()};
	}
};

template <std::size_t Capacity>
struct ScanArgs {
	static constexpr std::size_t size_v = Capacity;

	ScanArg args[Capacity]{};
	std::size_t scanned{};

	template <typename... Args>
	constexpr ScanArgs(Args&... out_args) : args{ScanArg::make(out_args)...} {}

	constexpr ScanArg next() {
		assert(scanned < Capacity);
		return args[scanned++];
	}
};
} // namespace detail

template <char Token = '$', typename... Args>
constexpr std::size_t scan(std::string_view text, std::string_view fmt, Args&... out_args) {
	constexpr auto trim = [](std::string_view in) {
		while (!in.empty() && detail::is_space(in[0])) { in = in.substr(1); }
		return in;
	};
	auto args = detail::ScanArgs<sizeof...(Args)>{out_args...};
	while (!fmt.empty()) {
		if (text.empty()) { return args.scanned; }
		char const ch = fmt[0];
		if (detail::is_space(ch)) {
			if (!detail::is_space(text[0])) { return args.scanned; }
			fmt = trim(fmt);
			text = trim(text);
			continue;
		}
		if (ch == Token) {
			if (args.scanned >= args.size_v) { return args.scanned; }
			auto arg = args.next();
			if (!arg.scan(arg.out_arg, text)) { return args.scanned; }
			fmt = fmt.substr(1);
			continue;
		}
		if (text[0] != ch) { return args.scanned; }
		text = text.substr(1);
		fmt = fmt.substr(1);
	}
	return args.scanned;
}
} // namespace

int main(int argc, char* argv[]) {
	{
		auto const buffer = std::string_view{"hello 42!"};
		auto i = int{};
		char x[8]{};
		[[maybe_unused]] auto const res = scan(buffer, "$ $!", x, i);
		logger::info("x: {}\ni: {}", x, i);
	}
	if (argc < 1) { return EXIT_FAILURE; }
	logger::info("Using vulkify v{}.{}.{}", vf::version_v.major, vf::version_v.minor, vf::version_v.patch);
	auto io_inst = pew::io::Instance(argv[0]);
	auto context = pew::make_context(argv[0]);
	if (!context) { return EXIT_FAILURE; }
	run(std::move(*context));
}
