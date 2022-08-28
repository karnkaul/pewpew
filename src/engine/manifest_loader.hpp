#pragma once
#include <engine/manifest.hpp>
#include <engine/resources.hpp>
#include <ktl/async/kfunction.hpp>
#include <ktl/kunique_ptr.hpp>
#include <tardigrade/services.hpp>
#include <mutex>

namespace pew {
class ManifestLoader {
  public:
	ManifestLoader();
	ManifestLoader(ManifestLoader&&) noexcept;
	ManifestLoader& operator=(ManifestLoader&&) noexcept;
	~ManifestLoader() noexcept;

	bool can_enqueue() const;

	template <typename T>
	void enqueue(LoadList<T> list) {
		if (!can_enqueue()) { return; }
		for (auto& info : list) {
			if (info.uri.empty()) { continue; }
			auto const func = [r = m_resources, c = m_context, m = m_mutex.get(), info = info] {
				auto t = Loader{*c}.load(info);
				auto lock = std::scoped_lock{*m};
				r->add(Signature{info.uri}, std::move(t));
			};
			do_enqueue(func);
		}
	}

	void enqueue(ktl::kfunction<void()> custom);
	void enqueue(Manifest manifest);

	std::size_t load(std::uint32_t threads = 2);

	bool idle() const;
	std::size_t total() const;
	std::size_t completed() const;
	std::size_t pending() const { return total() - completed(); }
	float progress() const;

  private:
	void do_enqueue(ktl::kfunction<void()>&& func);

	struct Impl;
	ktl::kunique_ptr<Impl> m_impl{};
	ktl::kunique_ptr<std::mutex> m_mutex{};
	Ptr<Resources> m_resources{};
	Ptr<Context> m_context{};
};
} // namespace pew
