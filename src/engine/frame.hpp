#pragma once
#include <tardigrade/render_target.hpp>
#include <vulkify/context/frame.hpp>

namespace pew {
struct Frame : tg::RenderTarget {
	vf::Frame frame{};
	vf::RenderState render_state{};
};
} // namespace pew
