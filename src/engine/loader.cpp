#include <engine/animated_sprite.hpp>
#include <engine/context.hpp>
#include <engine/loader.hpp>
#include <ktl/byte_array.hpp>
#include <util/io.hpp>
#include <util/logger.hpp>
#include <util/property.hpp>
#include <fstream>

namespace pew {
namespace {
struct SheetInfo {
	std::string image_uri{};
	glm::ivec2 tile_count{};
	vf::Filtering filtering{vf::Filtering::eNearest};
};

constexpr vf::Filtering to_filtering(std::string_view str) {
	if (str == "linear") { return vf::Filtering::eLinear; }
	return vf::Filtering::eNearest;
}

SheetInfo get_sheet_info(std::istream& in, Ptr<SheetAnimation::Sequence> sequence = {}) {
	auto parser = util::Property::Parser{in};
	auto ret = SheetInfo{};
	parser.parse_all([&](util::Property property) {
		if (property.key == "image") {
			ret.image_uri = std::move(property.value);
		} else if (property.key == "x") {
			ret.tile_count.x = std::atoi(property.value.c_str());
		} else if (property.key == "y") {
			ret.tile_count.y = std::atoi(property.value.c_str());
		} else if (property.key == "filtering") {
			ret.filtering = to_filtering(property.value);
		} else if (property.key == "duration" && sequence) {
			sequence->duration = vf::Time(std::atof(property.value.c_str()));
		} else if (sequence && property.key == "begin") {
			auto begin = std::atoi(property.value.c_str());
			if (begin >= 0) { sequence->begin = static_cast<std::size_t>(begin); }
		} else if (sequence && property.key == "end") {
			auto end = std::atoi(property.value.c_str());
			if (end >= 0) { sequence->end = static_cast<std::size_t>(end); }
		}
	});
	return ret;
}

bool validate(SheetInfo const& info, std::string_view const uri) {
	if (info.image_uri.empty()) {
		logger::warn("[Resources] Required field missing/empty in Sprite::Sheet [{}]: [image]", uri);
		return false;
	}
	if (info.tile_count.x <= 0 || info.tile_count.y <= 0) {
		logger::warn("[Resources] Required field missing/empty in Sprite::Sheet [{}]: [x/y]", uri);
		return false;
	}
	return true;
}

bool validate(IndexTimeline::Sequence const& sequence, std::string_view const uri) {
	if (sequence.duration < 0s) {
		logger::warn("[Resources] Invalid duration in Sprite::Sheet [{}]: [{}]", uri, sequence.duration.count());
		return false;
	}
	if (sequence.begin > sequence.end) {
		logger::warn("[Resources] Invalid sequence in Sprite::Sheet [{}]: [{}] - [{}]", sequence.begin, sequence.end);
		return false;
	}
	return true;
}

bool load_image(ktl::byte_array& out_buffer, vf::Image& out, char const* uri) {
	if (!io::load(out_buffer, uri)) {
		logger::warn("[Resources] Failed to read image: [{}]", uri);
		return false;
	}
	if (!out.load(vf::Image::Encoded{out_buffer})) {
		logger::warn("[Resources] Failed to open image: [{}]", uri);
		return false;
	}
	return true;
}

struct Stopwatch {
	vf::Clock::time_point start{vf::Clock::now()};

	vf::Time dt() const { return vf::diff(start); }
};
} // namespace

template <>
vf::Texture Loader::do_load<vf::Texture>(LoadInfo<vf::Texture> const& info) const {
	auto sw = Stopwatch{};
	auto image = vf::Image{};
	auto m_buffer = ktl::byte_array{};
	if (!load_image(m_buffer, image, info.uri.c_str())) {
		logger::warn("[Resources] Failed to open Image: [{}]", info.uri);
		return {};
	}
	auto ret = vf::Texture{context.vf_context.device(), image, info.info};
	if (!ret) {
		logger::warn("[Resources] Failed to create Texture: [{}]", info.uri);
		return {};
	}
	logger::debug("[Resources] Texture loaded: [{}] in {:.1f}ms", info.uri, sw.dt().count() * 1000.0f);
	return ret;
}

template <>
vf::Ttf Loader::do_load<vf::Ttf>(LoadInfo<vf::Ttf> const& info) const {
	auto sw = Stopwatch{};
	auto m_buffer = ktl::byte_array{};
	if (!io::load(m_buffer, info.uri.c_str())) {
		logger::warn("[Resources] Failed to open font: [{}]", info.uri);
		return {};
	}
	auto ret = vf::Ttf{context.vf_context.device()};
	if (!ret.load(m_buffer)) {
		logger::warn("[Resources] Failed to create Ttf: [{}]", info.uri);
		return {};
	}
	logger::debug("[Resources] Ttf loaded: [{}] in {:.1f}ms", info.uri, sw.dt().count() * 1000.0f);
	return ret;
}

template <>
capo::Sound Loader::do_load<capo::Sound>(LoadInfo<capo::Sound> const& info) const {
	auto sw = Stopwatch{};
	auto m_buffer = ktl::byte_array{};
	if (!io::load(m_buffer, info.uri.c_str())) {
		logger::warn("[Resources] Failed to read PCM: [{}]", info.uri);
		return {};
	}
	auto pcm = capo::PCM::from_memory(m_buffer, capo::FileFormat::eUnknown);
	if (!pcm) {
		logger::warn("[Resources] Failed to load PCM: [{}]", info.uri);
		return {};
	}
	auto ret = context.capo_instance->make_sound(*pcm);
	logger::debug("[Resources] Sound loaded: [{}] in {:.1f}ms", info.uri, sw.dt().count() * 1000.0f);
	return ret;
}

template <>
SheetAnimation Loader::do_load<SheetAnimation>(LoadInfo<SheetAnimation> const& info) const {
	auto sw = Stopwatch{};
	auto m_buffer = ktl::byte_array{};
	if (!io::load(m_buffer, info.uri.c_str())) {
		logger::warn("[Resources] Failed to read sheet animation: [{}]", info.uri);
		return {};
	}
	auto str = std::stringstream{std::string{reinterpret_cast<char const*>(m_buffer.data()), m_buffer.size()}};
	auto ret = SheetAnimation{};
	auto const sheet_info = get_sheet_info(str, &ret.sequence);
	if (!validate(sheet_info, info.uri)) { return {}; }
	if (!validate(ret.sequence, info.uri)) { return {}; }

	auto image = vf::Image{};
	if (!load_image(m_buffer, image, sheet_info.image_uri.c_str())) { return {}; }
	auto const tci = vf::TextureCreateInfo{.filtering = sheet_info.filtering};
	ret.texture = vf::Texture{context.vf_context.device(), image, tci};
	auto const tile_count = glm::uvec2{sheet_info.tile_count};
	ret.sheet.set_uvs(&ret.texture, tile_count.y, tile_count.x);
	if (ret.sequence.end <= ret.sequence.begin) { ret.sequence.end = ret.sheet.uv_count(); }

	logger::debug("[Resources] SheetAnimation loaded: [{}] in {:.1f}ms", info.uri, sw.dt().count() * 1000.0f);
	return ret;
}
} // namespace pew
