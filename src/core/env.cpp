#include <core/env.hpp>
#include <filesystem>

namespace pew {
namespace stdfs = std::filesystem;

namespace {
stdfs::path findData(stdfs::path start) {
	while (!start.empty() && start.parent_path() != start) {
		auto ret = start / "data";
		if (stdfs::is_directory(ret)) { return ret; }
		start = start.parent_path();
	}
	return {};
}
} // namespace

Env Env::make(int argc, char const* const* argv) {
	auto ret = Env{};
	ret.argv = {argv, static_cast<std::size_t>(argc)};
	ret.paths.pwd = stdfs::absolute(stdfs::current_path()).generic_string();
	if (!ret.argv.empty()) { ret.paths.exe = stdfs::absolute(ret.argv[0]).generic_string(); }
	ret.paths.data = findData(ret.paths.exe.empty() ? ret.paths.pwd : ret.paths.exe).generic_string();
	if (ret.paths.data.empty()) { ret.paths.data = ret.paths.pwd; }
	return ret;
}
} // namespace pew

std::string pew::dataPath(Env const& env, std::string_view uri) { return (stdfs::path(env.paths.data) / uri).generic_string(); }