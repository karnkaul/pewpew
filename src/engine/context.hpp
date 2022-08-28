#pragma once
#include <capo/capo.hpp>
#include <vulkify/vulkify.hpp>

#include <engine/audio.hpp>

namespace pew {
struct Context {
	vf::Context vf_context;
	ktl::kunique_ptr<capo::Instance> capo_instance{};

	Audio audio{};
};
} // namespace pew
