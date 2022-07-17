#pragma once
#include <game/game_object.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>
#include <vulkify/graphics/resources/texture.hpp>

namespace pew {
class Background final : public GameObject {
  public:
	bool open(std::string_view uri);
	bool set(vf::Texture const& texture);

	float speed{20.0f};

  private:
	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;

	vf::Texture m_texture{};
	std::vector<vf::Mesh> m_tiles{};
	glm::vec2 m_size{};
	float m_speed{};
};
} // namespace pew
