#include <engine/resources.hpp>
#include <ktl/hash_table.hpp>

namespace pew {
struct Resources::Impl {
	ktl::hash_table<Signature, ktl::kunique_ptr<Base>, Signature::Hasher> map{};
};

Resources::Resources() : m_impl(ktl::make_unique<Impl>()) {}

Resources::Resources(Resources&&) noexcept = default;
Resources& Resources::operator=(Resources&&) noexcept = default;
Resources::~Resources() noexcept = default;

bool Resources::remove(Signature sign) {
	if (auto it = m_impl->map.find(sign); it != m_impl->map.end()) {
		m_impl->map.erase(it);
		return true;
	}
	return false;
}

std::size_t Resources::size() const { return m_impl->map.size(); }
void Resources::clear() { m_impl->map.clear(); }

void Resources::add_base(Signature sign, ktl::kunique_ptr<Base>&& base) { m_impl->map.insert_or_assign(sign, std::move(base)); }

auto Resources::find(Signature sign) const -> Ptr<Base> {
	if (auto it = m_impl->map.find(sign); it != m_impl->map.end()) { return it->second.get(); }
	return {};
}

bool Resources::remove(Signature sign, tg::TypeId type) {
	if (auto it = m_impl->map.find(sign); it != m_impl->map.end() && it->second->type == type) {
		m_impl->map.erase(it);
		return true;
	}
	return false;
}
} // namespace pew
