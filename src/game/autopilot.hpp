#pragma once
#include <core/delta_time.hpp>
#include <ktl/enum_flags/enum_flags.hpp>

namespace pew {
class World;

class Autopilot {
  public:
	enum class Flag { eMove, eFire };
	using Flags = ktl::enum_flags<Flag, std::uint8_t>;

	void tick(World& world, vf::Time dt);

	Flags flags{Flag::eMove, Flag::eFire};

  private:
};
} // namespace pew
