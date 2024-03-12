#ifndef COMPWOLF_DEREFERENCEABLE_TO_HEADER
#define COMPWOLF_DEREFERENCEABLE_TO_HEADER

#include <type_traits>
#include <concepts>

namespace CompWolf
{
	template <typename Pointer, typename Result>
	concept dereferenceable_to = requires (Pointer p)
	{
		{ *p } -> std::convertible_to<Result>;
	};

	template <typename Pointer, typename Result>
	struct is_dereferenceable_to : std::false_type {};
	template <typename Pointer, typename Result>
		requires dereferenceable_to<Pointer, Result>
	struct is_dereferenceable_to<Pointer, Result> : std::true_type {};

	template <typename Pointer, typename Result>
	constexpr bool is_dereferenceable_to_v = is_dereferenceable_to<Pointer, Result>::value;


	
	template <typename Pointer, typename Result>
	concept nothrow_dereferenceable_to = requires (Pointer p)
	{
		noexcept(noexcept(static_cast<Result>(*p)));
	};

	template <typename Pointer, typename Result>
	struct is_nothrow_dereferenceable_to : std::false_type {};
	template <typename Pointer, typename Result>
		requires nothrow_dereferenceable_to<Pointer, Result>
	struct is_nothrow_dereferenceable_to<Pointer, Result> : std::true_type {};

	template <typename Pointer, typename Result>
	constexpr bool is_nothrow_dereferenceable_to_v = is_nothrow_dereferenceable_to<Pointer, Result>::value;
}

#endif // ! COMPWOLF_DEREFERENCEABLE_TO_HEADER
