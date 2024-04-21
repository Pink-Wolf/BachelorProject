#ifndef COMPWOLF_DEPENDENT_BOOL_HEADER
#define COMPWOLF_DEPENDENT_BOOL_HEADER

#include <type_traits>

namespace CompWolf
{
	template <typename T>
	constexpr bool dependent_true = std::is_same_v<T, T>;
	template <typename T>
	constexpr bool dependent_false = !dependent_true<T>;
}

#endif // ! COMPWOLF_DEPENDENT_BOOL_HEADER
