#pragma once
#include <util/index_timeline.hpp>
#include <util/ptr.hpp>
#include <util/signature.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>
#include <vulkify/graphics/texture.hpp>

namespace capo {
class Sound;
}

namespace vf {
class Ttf;
} // namespace vf

namespace pew {
struct Context;

struct SheetAnimation {
	using Sequence = IndexTimeline::Sequence;

	vf::Texture texture{};
	vf::Sprite::Sheet sheet{};
	Sequence sequence{};
};

template <typename T>
struct LoadInfo {
	std::string uri{};
};

template <>
struct LoadInfo<vf::Texture> {
	std::string uri{};
	vf::TextureCreateInfo info{};
};

class Loader {
  public:
	Context const& context;

	template <typename T>
	T load(LoadInfo<T> const& info) const;

  private:
	template <typename T>
	T do_load(LoadInfo<T> const& info) const = delete;
};

template <>
vf::Texture Loader::do_load<vf::Texture>(LoadInfo<vf::Texture> const& info) const;

template <>
vf::Ttf Loader::do_load<vf::Ttf>(LoadInfo<vf::Ttf> const& info) const;

template <>
capo::Sound Loader::do_load<capo::Sound>(LoadInfo<capo::Sound> const& info) const;

template <>
SheetAnimation Loader::do_load<SheetAnimation>(LoadInfo<SheetAnimation> const& info) const;

template <typename T>
T Loader::load(LoadInfo<T> const& info) const {
	if (info.uri.empty()) { return {}; }
	return do_load<T>(info);
}
} // namespace pew
