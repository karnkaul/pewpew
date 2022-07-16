#pragma once
#include <capo/capo.hpp>
#include <vulkify/vulkify.hpp>

#include <core/basis.hpp>
#include <core/env.hpp>
#include <engine/audio.hpp>

namespace pew {
struct Context {
	Env env{};
	ktl::kunique_ptr<capo::Instance> capoInstance{};
	vf::Context vfContext;

	Audio audio{};
	Basis basis{};

	constexpr float basisScale(glm::vec2 framebufferSize) const {
		auto const x = framebufferSize.x / basis.space.x;
		auto const y = framebufferSize.y / basis.space.y;
		return std::min(x, y);
	}
};
} // namespace pew