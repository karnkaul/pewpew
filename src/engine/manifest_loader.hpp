#pragma once
#include <engine/manifest.hpp>
#include <engine/resources.hpp>
#include <ktl/async/kfunction.hpp>
#include <ktl/kunique_ptr.hpp>

namespace pew {
class ManifestLoader {
  public:
	ManifestLoader(Context const& context, Resources& out_resources);
	ManifestLoader(ManifestLoader&&) noexcept;
	ManifestLoader& operator=(ManifestLoader&&) noexcept;
	~ManifestLoader() noexcept;

	bool can_enqueue() const;

	template <typename T>
	void enqueue(LoadList<T> list) {
		if (!can_enqueue()) { return; }
		for (auto& uri : list) {
			if (uri.empty()) { continue; }
			enqueue([r = m_resources, u = std::move(uri)] { r->load<T>(u); });
		}
	}

	void enqueue(Manifest manifest);

	std::size_t load(std::uint32_t threads = 2);

	bool idle() const;
	std::size_t total() const;
	std::size_t completed() const;
	std::size_t pending() const { return total() - completed(); }
	float progress() const;

  private:
	void enqueue(ktl::kfunction<void()>&& func);

	struct Impl;
	ktl::kunique_ptr<Impl> m_impl{};
	Ptr<Resources> m_resources{};
};
} // namespace pew
