#pragma once
#include <engine/frame.hpp>
#include <game/attachments/object.hpp>
#include <tardigrade/render_attachment.hpp>
#include <tardigrade/services.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>

namespace pew {
template <std::derived_from<vf::Primitive> Type>
class Renderer : public tg::RenderAttachment {
  public:
	Type& get() { return m_t; }
	Type const& get() const { return m_t; }

  protected:
	void setup() override {
		tg::RenderAttachment::setup();
		m_t = Type{*tg::locate<vf::GfxDevice const*>()};
	}

	void render(tg::RenderTarget const& target) const override {
		auto const& frame = static_cast<Frame const&>(target).frame;
		frame.draw(m_t);
	}

	Type m_t{};
};

template <std::derived_from<vf::Mesh> Type = vf::Mesh>
class MeshRenderer : public Renderer<Type> {
  protected:
	void tick(tg::DeltaTime) override {
		if (auto* object = this->entity()->template find<Object>()) { this->get().instance().transform = object->transform; }
	}
};

template <std::derived_from<vf::Sprite> Type = vf::Sprite>
class SpriteRenderer : public Renderer<Type> {
  protected:
	void tick(tg::DeltaTime) override {
		if (auto* object = this->entity()->template find<Object>()) {
			this->get().transform() = object->transform;
			if (object->size != this->get().size()) { this->get().set_size(object->size); }
		}
	}
};
} // namespace pew
