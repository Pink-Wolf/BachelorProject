#ifndef COMPWOLF_TYPE_TRAITS_HEADER
#define COMPWOLF_TYPE_TRAITS_HEADER

// This file includes <type_traits>

#include "event.h"
#include <type_traits>

namespace CompWolf
{
	/* For a given title and expression, defines entities like is_[title]_assignable_v<ValueType, DeltaType>, which is true when  */
#define COMPWOLF_DEFINE_ASSIGNABLE(title, expression)												\
	/* base version */																				\
	template <class ValueType, class DeltaType, typename = void>									\
	struct is_##title##_assignable : std::false_type {};											\
																									\
	template <class ValueType, class DeltaType>														\
	struct is_##title##_assignable<ValueType, DeltaType,											\
		typename std::enable_if_t<std::is_member_function_pointer_v<decltype(						\
			&ValueType::operator expression(std::declval<DeltaType>())								\
			)>>> : std::true_type {};																\
																									\
	template <class ValueType, class DeltaType>														\
	constexpr bool is_##title##_assignable_v = is_##title##_assignable::value;						\
																									\
	/* nothrow version */																			\
	template <class ValueType, class DeltaType, typename = void>									\
	struct is_nothrow_##title##_assignable : std::false_type {};									\
																									\
	template <class ValueType, class DeltaType>														\
	struct is_nothrow_##title##_assignable<ValueType, DeltaType,									\
		typename std::enable_if_t<noexcept(															\
			std::declval<ValueType>() expression std::declval<DeltaType>()							\
			)>> : std::true_type {};																\
																									\
	template <class ValueType, class DeltaType>														\
	constexpr bool is_nothrow_##title##_assignable_v = is_nothrow_##title##_assignable::value;		\


	COMPWOLF_DEFINE_ASSIGNABLE(addition, +=);
	COMPWOLF_DEFINE_ASSIGNABLE(subtraction, -=);
	COMPWOLF_DEFINE_ASSIGNABLE(multiplication, *=);
	COMPWOLF_DEFINE_ASSIGNABLE(division, /=);
	COMPWOLF_DEFINE_ASSIGNABLE(remainder, %=);
	COMPWOLF_DEFINE_ASSIGNABLE(bitwise_and, &=);
	COMPWOLF_DEFINE_ASSIGNABLE(bitwise_or, |=);
	COMPWOLF_DEFINE_ASSIGNABLE(bitwise_xor, ^=);
	COMPWOLF_DEFINE_ASSIGNABLE(bitwise_left_shift, <<=);
	COMPWOLF_DEFINE_ASSIGNABLE(bitwise_right_shift, >>=);
}

#endif // ! COMPWOLF_TYPE_TRAITS_HEADER
