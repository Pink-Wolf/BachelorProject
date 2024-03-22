#ifndef COMPWOLF_TYPE_TRAIT_HEADER
#define COMPWOLF_TYPE_TRAIT_HEADER

#include <type_traits>
#include <utility>

namespace CompWolf
{
	namespace Private
	{
		template <std::size_t Index, typename T, typename... TRest>
		struct get_type_at_index_internal
		{
			using type = get_type_at_index_internal<Index - 1, TRest...>::type;
		};
		template <typename T, typename... TRest>
		struct get_type_at_index_internal<0, T, TRest...>
		{
			using type = T;
		};
		template <std::size_t Index, typename... Ts>
		struct get_type_at_index
		{
			static_assert(Index >= 0, "out of range! Tried getting a type from a type list at a negative index");
			static_assert(Index < sizeof...(Ts), "out of range! Tried getting a type from a type list at an index past the last item");

			using type = get_type_at_index_internal<Index, Ts...>::type;
		};
	}

	/* A compile-time list of types. */
	template <typename... Ts>
	struct type_list
	{
		/* The amount of types the type_list contains. */
		static constexpr int size = sizeof...(Ts);
		/* Whether the type_list contains nothing. */
		static constexpr bool empty = false;

		/* Gets the type at the given index in the type_list. */
		template <std::size_t Index>
		using at = Private::template get_type_at_index<Index, Ts...>::type;

		/* The first type in the type_list. */
		using front = at<0>;
		/* The last type in the type_list. */
		using back = at<size - 1>;
		

		/* Whether the type_list contains the given type. */
		template <typename T>
		static constexpr bool has = (std::is_same_v<Ts, T> || ...);

		/* A copy of the type_list which, in addition to the original types contained, also contains the given types. */
		template <typename... TOthers>
		using and_types = type_list<Ts..., TOthers...>;
		template <typename TOther>
		using and_type_list_before_this = TOther::template and_types<Ts...>;
		/* A copy of the type_list which, in addition to the original types contained, also contains the types in the given other type_list. */
		template <typename TOther>
		using and_type_list = TOther::template and_type_list_before_this<type_list>;
	};

	template <>
	struct type_list<>
	{
		/* The amount of types the type_list contains. */
		static constexpr int size = 0;
		/* Whether the type_list contains nothing. */
		static constexpr bool empty = true;

		/* Whether the type_list contains the given type. */
		template <typename T>
		static constexpr bool has = false;

		/* A copy of the type_list which, in addition to the original types contained, also contains the given types. */
		template <typename... TOthers>
		using and_types = type_list<TOthers...>;
		template <typename TOther>
		using and_type_list_before_this = TOther;
		/* A copy of the type_list which, in addition to the original types contained, also contains the types in the given other type_list. */
		template <typename TOther>
		using and_type_list = TOther;
	};

	template <typename... TypeLists>
	struct combine_type_lists
	{
		using type = type_list<>;
	};
	template <typename TypeList, typename... RestTypeLists>
	struct combine_type_lists<TypeList, RestTypeLists...>
	{
		using type = TypeList::template and_type_list<typename combine_type_lists<RestTypeLists...>::type>;
	};
	template <typename... TypeLists>
	using combine_type_lists_t = combine_type_lists<TypeLists...>::type;
}

#endif // ! COMPWOLF_TYPE_TRAIT_HEADER
