target_sources(${PROJECT_NAME} PRIVATE
  core/basis.hpp
  core/collection.hpp
  core/delta_time.hpp
  core/env.cpp
  core/env.hpp
  core/framerate.hpp
  core/log.cpp
  core/log.hpp

  engine/context.hpp
  engine/instanced.hpp
  engine/keyboard.hpp

  game/controller.cpp
  game/controller.hpp
  game/game_object.hpp
  game/layers.hpp
  game/layout.hpp
  game/pawn.cpp
  game/pawn.hpp
  game/resources.hpp
  game/status_effect.hpp
  game/world.cpp
  game/world.hpp

  game/objects/background.cpp
  game/objects/background.hpp
  game/objects/consumable.cpp
  game/objects/consumable.hpp
  game/objects/damager.cpp
  game/objects/damager.hpp
  game/objects/drifter.cpp
  game/objects/drifter.hpp
  game/objects/hud.cpp
  game/objects/hud.hpp
  game/objects/player.cpp
  game/objects/player.hpp
  game/objects/progress_bar.cpp
  game/objects/progress_bar.hpp
  game/objects/weapon.cpp
  game/objects/weapon.hpp

  game/status_effects/time_dilator.cpp
  game/status_effects/time_dilator.hpp

  main.cpp
)
