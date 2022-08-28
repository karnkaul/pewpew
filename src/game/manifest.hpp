#pragma once

namespace pew {
template <typename T>
struct Manifest {
	struct {
		T main{};
	} fonts;

	struct {
		T ship{};
		T background{};
	} textures;

	struct {
		T ship{};
	} sprite_sheets{};

	struct {
		T fire{};
	} sfx;
};
} // namespace pew
