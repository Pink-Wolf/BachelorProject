#ifndef COMPWOLF_TYPE_VALUE_PAIR_LIST_HEADER
#define COMPWOLF_TYPE_VALUE_PAIR_LIST_HEADER

#include "type_list.hpp"
#include "type_value_pair.hpp"
#include <type_traits>

namespace CompWolf
{
	template <typename PairList1, typename PairList2>
	struct merge_type_value_pairs_by_value
	{
		static_assert(PairList1::empty || PairList2::empty, "Tried calling merge_type_value_pairs_by_value with types that are not type_lists of type_value_pairs");
		using types = PairList1::template and_type_list<PairList2>;
	};
	template <typename Type1, auto Value1, typename Type2, auto Value2, TypeValuePair... TRest1, TypeValuePair... TRest2>
	struct merge_type_value_pairs_by_value
		< type_list<type_value_pair<Type1, Value1>, TRest1...>
		, type_list<type_value_pair<Type2, Value2>, TRest2...>
		>
	{
	private:
		using pair1 = type_value_pair<Type1, Value1>;
		using pair2 = type_value_pair<Type2, Value2>;

	public:
		using types = std::conditional_t
			< Value1 <= Value2
			, typename type_list<pair1>::template and_type_list<typename merge_type_value_pairs_by_value<type_list<TRest1...>, type_list<pair2, TRest2...>>::types>
			, typename type_list<pair2>::template and_type_list<typename merge_type_value_pairs_by_value<type_list<pair1, TRest1...>, type_list<TRest2...>>::types>
			>;
	};
	template <typename PairList1, typename PairList2>
	using merge_type_value_pairs_by_value_t = merge_type_value_pairs_by_value<PairList1, PairList2>::types;
}

#endif // ! COMPWOLF_TYPE_VALUE_PAIR_LIST_HEADER
