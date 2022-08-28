#pragma once

namespace pew {
inline constexpr bool debug_v =
#if defined PEW_DEBUG
	true;
#else
	false;
#endif
} // namespace pew
