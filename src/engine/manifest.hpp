#pragma once
#include <engine/loader.hpp>

namespace pew {
struct Manifest {
	LoadList<vf::Ttf> fonts{};
	LoadList<vf::Texture> textures{};
	LoadList<capo::Sound> sfx{};
	LoadList<vf::Sprite::Sheet> sprite_sheets{};
	LoadList<SheetAnimation> sheets{};

	static std::size_t load(Manifest& out, char const* uri);
};
} // namespace pew
