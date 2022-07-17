#pragma once
#include <vulkify/core/time.hpp>

namespace pew {
struct DeltaTime {
	vf::Time real{};
	vf::Time scaled{};
};
} // namespace pew
