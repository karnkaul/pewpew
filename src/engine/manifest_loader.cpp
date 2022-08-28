#include <capo/sound.hpp>
#include <engine/manifest_loader.hpp>
#include <util/async_queue.hpp>
#include <util/logger.hpp>
#include <vulkify/ttf/ttf.hpp>
#include <algorithm>
#include <atomic>
#include <thread>

namespace pew {
struct ManifestLoader::Impl {
	AsyncQueue<ktl::kfunction<void()>> queue{};
	std::atomic<std::size_t> idle{};
	std::atomic<std::size_t> completed{};
	std::vector<std::jthread> threads{};
	std::size_t total{};
};

ManifestLoader::ManifestLoader() : m_impl(ktl::make_unique<Impl>()), m_mutex(ktl::make_unique<std::mutex>()) {
	m_resources = tg::locate<Resources*>();
	m_context = tg::locate<Context*>();
}

ManifestLoader::ManifestLoader(ManifestLoader&&) noexcept = default;
ManifestLoader& ManifestLoader::operator=(ManifestLoader&&) noexcept = default;

ManifestLoader::~ManifestLoader() noexcept { m_impl->queue.clear(); }

bool ManifestLoader::can_enqueue() const {
	if (!m_impl->threads.empty()) {
		logger::warn("[AsyncLoader] Cannot enqueue manifest while loading");
		return false;
	}
	return true;
}

void ManifestLoader::enqueue(ktl::kfunction<void()> custom) {
	if (!can_enqueue() || !custom) { return; }
	do_enqueue([c = std::move(custom), m = m_mutex.get()] {
		auto lock = std::scoped_lock{*m};
		c();
	});
}

void ManifestLoader::enqueue(Manifest manifest) {
	enqueue(std::move(manifest.fonts));
	enqueue(std::move(manifest.textures));
	enqueue(std::move(manifest.sfx));
	for (auto& sheet : manifest.sprite_sheets) {
		enqueue([sheet = std::move(sheet), r = m_resources, c = m_context] {
			auto info = Loader{*c}.load(LoadInfo<SheetInfo>{.uri = std::move(sheet.uri)});
			if (!info) { return; }
			// TODO: complete

			if (auto texture = r->find<vf::Texture>(info.texture_uri)) {
				auto sprite_sheet = vf::Sprite::Sheet{};
				sprite_sheet.set_texture(texture->handle());
				r->add(std::move(sheet.uri), std::move(sprite_sheet));
			}
		});
	}
}

std::size_t ManifestLoader::load(std::uint32_t threads) {
	if (!m_impl->threads.empty()) {
		logger::warn("[AsyncLoader] Load already in progress");
		return 0;
	}
	if (m_impl->queue.empty()) { return 0; }

	m_impl->total = m_impl->queue.size();
	threads = std::clamp(threads, 1U, std::thread::hardware_concurrency());
	for (std::size_t i = 0; i < threads; ++i) {
		m_impl->threads.push_back(std::jthread{[impl = m_impl.get()](std::stop_token const& stop) {
			auto func = ktl::kfunction<void()>{};
			while (impl->queue.pop(func, stop)) {
				func();
				++impl->completed;
			}
			++impl->idle;
		}});
	}
	for (auto& thread : m_impl->threads) { thread.request_stop(); }

	return m_impl->total;
}

bool ManifestLoader::idle() const { return m_impl->idle == m_impl->threads.size(); }

std::size_t ManifestLoader::total() const { return m_impl->total; }
std::size_t ManifestLoader::completed() const { return m_impl->completed; }

float ManifestLoader::progress() const {
	if (m_impl->threads.empty()) { return -1.0f; }
	if (idle()) { return 1.0f; }
	return static_cast<float>(completed()) / static_cast<float>(total());
}

void ManifestLoader::do_enqueue(ktl::kfunction<void()>&& func) { m_impl->queue.push(std::move(func)); }
} // namespace pew
