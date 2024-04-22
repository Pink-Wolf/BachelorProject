#ifndef COMPWOLF_DEPENDENT_BOOL_HEADER
#define COMPWOLF_DEPENDENT_BOOL_HEADER

namespace CompWolf
{
	template <typename... T>
	constexpr bool dependent_true = sizeof...(T) == sizeof...(T);
	template <typename... T>
	constexpr bool dependent_false = !dependent_true<T...>;
}

#endif // ! COMPWOLF_DEPENDENT_BOOL_HEADER
