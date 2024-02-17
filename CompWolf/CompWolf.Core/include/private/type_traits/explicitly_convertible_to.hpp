#ifndef COMPWOLF_EXPLICITLY_CONVERTIBLE_TO_HEADER
#define COMPWOLF_EXPLICITLY_CONVERTIBLE_TO_HEADER

#include <type_traits>

namespace CompWolf
{
	/* Whether an object of type From can be explicitly converted to the type To. */
	template <typename From, typename To>
	concept explicitly_convertible_to = requires(From from)
	{
		static_cast<To>(from);
	};
	/* Whether an object of type From can be explicitly converted to the type To. */
	template <typename From, typename To>
	struct is_explicitly_convertible : std::false_type {};
	template <typename From, typename To>
		requires explicitly_convertible_to<From, To>
	struct is_explicitly_convertible<From, To> : std::true_type {};
	/* Whether an object of type From can be explicitly converted to the type To. */
	template <typename From, typename To>
	using is_explicitly_convertible_v = is_explicitly_convertible<From, To>::value;


	/* Whether explicitly converting an object of type From to the type To is noexcept. */
	template <typename From, typename To>
	concept nothrow_explicitly_convertible_to = requires(From from)
	{
		noexcept(noexcept(static_cast<To>(from)));
	};
	/* Whether explicitly converting an object of type From to the type To is noexcept. */
	template <typename From, typename To>
	struct is_nothrow_explicitly_convertible : std::false_type {};
	template <typename From, typename To>
		requires nothrow_explicitly_convertible_to<From, To>
	struct is_nothrow_explicitly_convertible<From, To> : std::true_type {};
	/* Whether explicitly converting an object of type From to the type To is noexcept. */
	template <typename From, typename To>
	using is_nothrow_explicitly_convertible_v = is_nothrow_explicitly_convertible<From, To>::value;
}

#endif // ! COMPWOLF_EXPLICITLY_CONVERTIBLE_TO_HEADER
