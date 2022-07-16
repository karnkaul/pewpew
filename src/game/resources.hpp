#pragma once
#include <capo/sound.hpp>
#include <vulkify/ttf/ttf.hpp>

namespace pew {
struct Resources {
	struct {
		vf::Ttf main{};
	} fonts;

	struct {
		vf::Texture ship{};
	} textures;

	struct {
		capo::Sound fire{};
	} sfx;
};
} // namespace pew
