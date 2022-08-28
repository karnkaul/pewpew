#pragma once
#include <engine/loader.hpp>
#include <engine/resources.hpp>
#include <ktl/async/kfunction.hpp>
#include <ktl/kunique_ptr.hpp>
#include <tardigrade/services.hpp>
#include <mutex>

namespace pew {
template <typename T>
using ResourceList = std::vector<LoadInfo<T>>;

class AsyncLoader {
  public:
	AsyncLoader();
	AsyncLoader(AsyncLoader&&) noexcept;
	AsyncLoader& operator=(AsyncLoader&&) noexcept;
	~AsyncLoader() noexcept;

	bool can_enqueue() const;

	template <typename T>
	void enqueue(ResourceList<T> const& list) {
		if (!can_enqueue()) { return; }
		for (auto const& info : list) {
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
