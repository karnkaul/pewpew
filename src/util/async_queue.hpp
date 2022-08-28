#pragma once
#include <condition_variable>
#include <deque>
#include <mutex>
#include <span>
#include <stop_token>

namespace pew {
template <typename Type>
class AsyncQueue {
  public:
	using value_type = Type;

	AsyncQueue() = default;
	~AsyncQueue() { clear(); }

	AsyncQueue& operator=(AsyncQueue&&) = delete;

	template <typename... Args>
		requires(std::is_constructible_v<Type, Args...>)
	void emplace(Args&&... args) {
		auto lock = std::unique_lock{m_mutex};
		m_queue.emplace_back(std::forward<Args>(args)...);
		lock.unlock();
		m_cv.notify_one();
	}

	void push(Type t) { emplace(std::move(t)); }

	void push(std::span<Type> ts) {
		auto lock = std::scoped_lock{m_mutex};
		for (auto& t : ts) { m_queue.push_back(std::move(t)); }
		ping();
	}

	bool try_pop(Type& out) {
		auto lock = std::scoped_lock{m_mutex};
		return pop(lock, out);
	}

	bool pop(Type& out, std::stop_token const& stop = {}) {
		auto lock = std::unique_lock{m_mutex};
		m_cv.wait(lock, [&] { return stop.stop_requested() || !m_queue.empty(); });
		if (m_queue.empty()) { return false; }
		return pop(lock, out);
	}

	std::size_t size() const {
		auto lock = std::scoped_lock{m_mutex};
		return m_queue.size();
	}

	bool empty() const {
		auto lock = std::scoped_lock{m_mutex};
		return m_queue.empty();
	}

	void clear() {
		auto lock = std::scoped_lock{m_mutex};
		m_queue.clear();
		ping();
	}

	void ping() { m_cv.notify_all(); }

  private:
	template <typename Lock>
	bool pop(Lock const&, Type& out) {
		if (m_queue.empty()) { return false; }
		out = std::move(m_queue.front());
		m_queue.pop_front();
		return true;
	}

	std::deque<Type> m_queue{};
	std::condition_variable m_cv{};
	mutable std::mutex m_mutex{};
};
} // namespace pew
