#pragma once
#include <engine/resources.hpp>

namespace pew {
template <typename T>
using LoadList = std::vector<std::string>;

struct Manifest {
	LoadList<vf::Ttf> fonts{};
	LoadList<vf::Texture> textures{};
	LoadList<capo::Sound> sfx{};
	LoadList<vf::Sprite::Sheet> sheets{};

	static std::size_t load(Manifest& out, char const* uri);
};
} // namespace pew
