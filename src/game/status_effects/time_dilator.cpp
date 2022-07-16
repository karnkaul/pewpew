#include <game/status_effects/time_dilator.hpp>
#include <game/world.hpp>

namespace pew {
void TimeDilator::apply(vf::Time) { m_world->timeScale = timeScale; }
void TimeDilator::unapply(vf::Time) { m_world->timeScale = 1.0f; }
} // namespace pew
