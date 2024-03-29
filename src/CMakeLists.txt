target_sources(${PROJECT_NAME} PRIVATE
  engine/animated_sprite.cpp
  engine/animated_sprite.hpp
  engine/async_loader.cpp
  engine/async_loader.hpp
  engine/context.hpp
  engine/framerate.hpp
  engine/frame.hpp
  engine/instanced.hpp
  engine/loader.cpp
  engine/loader.hpp
  engine/resources.cpp
  engine/resources.hpp

  game/controller.cpp
  game/controller.hpp
  game/layers.hpp
  game/layout.hpp
  game/manifest_data.cpp
  game/manifest_data.hpp
  game/manifest.hpp
  game/world.cpp
  game/world.hpp

  game/attachments/background.cpp
  game/attachments/background.hpp
  game/attachments/hud.cpp
  game/attachments/hud.hpp
  game/attachments/object.hpp
  game/attachments/player.cpp
  game/attachments/player.hpp
  game/attachments/prop.hpp
  game/attachments/progress_bar.cpp
  game/attachments/progress_bar.hpp
  game/attachments/renderer.hpp
  game/attachments/weapon.cpp
  game/attachments/weapon.hpp

  util/async_queue.hpp
  util/collection.hpp
  util/defines.hpp
  util/index_timeline.cpp
  util/index_timeline.hpp
  util/logger.cpp
  util/logger.hpp
  util/property.cpp
  util/property.hpp
  util/ptr.hpp
  util/random.hpp
  util/signature.cpp
  util/signature.hpp
  util/util.cpp
  util/util.hpp

  util/io.cpp
  util/io.hpp

  main.cpp
)
