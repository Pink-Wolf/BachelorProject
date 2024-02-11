#ifndef COMPWOLF_FUNCTIONAL_HEADER
#define COMPWOLF_FUNCTIONAL_HEADER

#include <utility>
#include <type_traits>

namespace CompWolf
{
	/* Whether an object of the given type T is a functional object taking the given ParameterTypes and returns the given ReturnType.
	 * @typename T The type to check.
	 * @typename ReturnType The type that invoking T should return; can be null when it should return nothing.
	 * @typename ParameterTypes The types of parameters that invoking T should takes.
	 */
	template <typename T, typename ReturnType, typename... ParameterTypes>
	concept callable = requires(T t)
	{
		{ t(std::declval<ParameterTypes>()...) } -> std::same_as<ReturnType>;
	};

	/* Whether an object of the given type T is a functional object taking the given ParameterTypes and returns the given ReturnType.
	 * @typename T The type to check.
	 * @typename ReturnType The type that invoking T should return; can be null when it should return nothing.
	 * @typename ParameterTypes The types of parameters that invoking T should takes.
	 */
	template <typename T, typename ReturnType, typename... ParameterTypes>
	struct is_callable : std::conditional_t<callable<T, ReturnType, ParameterTypes...>, std::true_type, std::false_type> {};
	/* Whether an object of the given type T is a functional object taking the given ParameterTypes and returns the given ReturnType.
	 * @typename T The type to check.
	 * @typename ReturnType The type that invoking T should return; can be null when it should return nothing.
	 * @typename ParameterTypes The types of parameters that invoking T should takes.
	 */
	template <typename T, typename ReturnType, typename... ParameterTypes>
	constexpr auto is_callable_v = is_callable<T, ReturnType, ParameterTypes...>::value;
}

#endif // ! COMPWOLF_FUNCTIONAL_HEADER
