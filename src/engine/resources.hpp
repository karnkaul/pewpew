#pragma once
#include <ktl/kunique_ptr.hpp>
#include <tardigrade/detail/type_id.hpp>
#include <util/ptr.hpp>
#include <util/signature.hpp>

namespace pew {
class Resources {
  public:
	class Loader;

	Resources();
	Resources(Resources&&) noexcept;
	Resources& operator=(Resources&&) noexcept;
	~Resources() noexcept;

	template <typename Type>
	Ptr<Type> add(Signature sign, Type t) {
		if (sign == Signature{}) { return {}; }
		auto model = ktl::make_unique<Model<Type>>(std::move(t));
		auto* ret = &model->t;
		add_base(sign, std::move(model));
		return ret;
	}

	template <typename Type>
	bool remove(Signature sign) {
		return remove(sign, tg::TypeId::make<Type>());
	}

	bool remove(Signature sign);

	template <typename Type>
	Ptr<Type> find(Signature sign) const {
		if (auto ret = find(sign); ret && ret->type == tg::TypeId::make<Type>()) { return &static_cast<Model<Type>*>(ret)->t; }
		return {};
	}

	template <typename Type>
	Ptr<Type> get(Signature sign) const {
		auto ret = find<Type>(sign);
		assert(ret);
		return ret;
	}

	template <typename Type>
	bool contains(Signature sign) const {
		return find<Type>(sign) != nullptr;
	}

	bool contains(Signature sign) const { return find(sign) != nullptr; }

	std::size_t size() const;
	void clear();

  private:
	struct Base {
		tg::TypeId type{};
		Base(tg::TypeId type) : type(type) {}
		virtual ~Base() = default;
	};

	template <typename T>
	struct Model : Base {
		T t;
		Model(T&& t) : Base{tg::TypeId::make<T>()}, t{std::move(t)} {}
	};

	void add_base(Signature sign, ktl::kunique_ptr<Base>&& base);
	Ptr<Base> find(Signature sign) const;
	bool remove(Signature sign, tg::TypeId type);

	struct Impl;
	ktl::kunique_ptr<Impl> m_impl{};
};
} // namespace pew
