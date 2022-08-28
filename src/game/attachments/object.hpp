#pragma once
#include <tardigrade/attachment.hpp>
#include <vulkify/core/rect.hpp>
#include <vulkify/core/transform.hpp>

namespace pew {
class Object : public tg::Attachment {
  public:
	vf::Transform transform{};
	glm::vec2 size{100.0f};

	vf::Rect bounds() const { return {{size, transform.position}}; }

	void setup() override {}
};
} // namespace pew
