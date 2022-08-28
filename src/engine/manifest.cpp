#include <engine/manifest.hpp>
#include <ktl/byte_array.hpp>
#include <util/io.hpp>
#include <util/logger.hpp>
#include <util/property.hpp>
#include <sstream>

namespace pew {
std::size_t Manifest::load(Manifest& out, char const* uri) {
	auto ret = std::size_t{};
	auto buffer = ktl::byte_array{};
	if (!io::load(buffer, uri)) { return false; }

	auto const push = [&ret](auto& out_list, std::string uri) {
		out_list.push_back({.uri = std::move(uri)});
		++ret;
	};

	auto str = std::stringstream{std::string{reinterpret_cast<char const*>(buffer.data()), buffer.size()}};
	auto parser = util::Property::Parser{str};
	parser.parse_all([&](util::Property property) {
		if (property.key.starts_with("fonts/")) {
			push(out.fonts, std::move(property.key));
		} else if (property.key.starts_with("textures/")) {
			push(out.textures, std::move(property.key));
		} else if (property.key.starts_with("sheets/")) {
			push(out.sprite_sheets, std::move(property.key));
		} else if (property.key.starts_with("sfx/")) {
			push(out.sfx, std::move(property.key));
		} else {
			logger::warn("[Manifest] Unrecognized key ignored: [{}]", property.key);
		}
	});

	return ret;
}
} // namespace pew
