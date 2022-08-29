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
	Ptr<Context const> context{};
};

ManifestLoader::ManifestLoader(Context const& context, Resources& out_resources) : m_impl(ktl::make_unique<Impl>()), m_resources(&out_resources) {
	m_impl->context = &context;
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

void ManifestLoader::enqueue(Manifest manifest) {
	// independent
	enqueue<vf::Ttf>(std::move(manifest.fonts));
	enqueue<vf::Texture>(std::move(manifest.textures));
	enqueue<capo::Sound>(std::move(manifest.sfx));

	// dependent
	enqueue<vf::Sprite::Sheet>(std::move(manifest.sheets));
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

void ManifestLoader::enqueue(ktl::kfunction<void()>&& func) { m_impl->queue.push(std::move(func)); }
} // namespace pew
