#pragma once
#include <core/delta_time.hpp>
#include <core/ptr.hpp>
#include <vulkify/context/frame.hpp>

namespace pew {
class World;

class GameObject {
  public:
	virtual ~GameObject() = default;

	void destroy() { m_destroyed = true; }
	bool destroyed() const { return m_destroyed; }

	int layer{};

  protected:
	virtual void setup() = 0;
	virtual void tick(DeltaTime dt) = 0;
	virtual void draw(vf::Frame const& frame) const = 0;

	Ptr<World> m_world{};

  private:
	bool m_destroyed{};

	friend class World;
};

class UiView : public GameObject {
  protected:
	virtual void onKey(vf::KeyEvent const& key) = 0;

	friend class World;
};
} // namespace pew
