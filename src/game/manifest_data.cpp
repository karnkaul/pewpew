#include <game/manifest_data.hpp>
#include <ktl/byte_array.hpp>
#include <util/io.hpp>
#include <util/property.hpp>
#include <sstream>

namespace pew {
std::size_t ManifestData::load(ManifestData& out, char const* uri) {
	auto ret = std::size_t{};
	auto buffer = ktl::byte_array{};
	if (!io::load(buffer, uri)) { return false; }

	auto const set = [&ret](std::string& out_uri, Signature& out_sign, std::string uri) {
		if (uri.empty()) { return; }
		out_uri = std::move(uri);
		out_sign = Signature{out_uri};
		++ret;
	};

	auto str = std::stringstream{std::string{reinterpret_cast<char const*>(buffer.data()), buffer.size()}};
	auto parser = util::Property::Parser{str};
	parser.parse_all([&](util::Property property) {
		if (property.key == "fonts.main") {
			set(out.uris.fonts.main, out.signs.fonts.main, std::move(property.value));
		} else if (property.key == "textures.ship") {
			set(out.uris.textures.ship, out.signs.textures.ship, std::move(property.value));
		} else if (property.key == "textures.background") {
			set(out.uris.textures.background, out.signs.textures.background, std::move(property.value));
		} else if (property.key == "sprite_sheets.ship") {
			set(out.uris.sprite_sheets.ship, out.signs.sprite_sheets.ship, std::move(property.value));
		} else if (property.key == "sfx.fire") {
			set(out.uris.sfx.fire, out.signs.sfx.fire, std::move(property.value));
		}
	});

	return ret;
}
} // namespace pew
