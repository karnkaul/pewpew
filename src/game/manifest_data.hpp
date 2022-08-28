#pragma once
#include <game/manifest.hpp>
#include <util/signature.hpp>
#include <string>

namespace pew {
struct ManifestData {
	Manifest<std::string> uris{};
	Manifest<Signature> signs{};

	static std::size_t load(ManifestData& out, char const* uri);
};
} // namespace pew
