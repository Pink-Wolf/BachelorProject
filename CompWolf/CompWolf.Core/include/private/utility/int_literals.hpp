#ifndef COMPWOLF_INT_LITERALS_HEADER
#define COMPWOLF_INT_LITERALS_HEADER

#include <cstdint>
#include <stdexcept>

namespace CompWolf
{
#define COMPWOLF_INT_LITERAL_OPERATOR(suffix, type, max)														\
	constexpr inline auto operator ""##suffix(unsigned long long int a) -> type									\
	{																											\
		if (a > max) throw std::domain_error("Tried defining a "#type" value that is bigger than possible");	\
		return static_cast<type>(a);																			\
	}																											\

#ifdef INT8_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_int8, int8_t, INT8_MAX);
#endif // INT8_MIN
#ifdef INT16_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_int16, int16_t, INT16_MAX);
#endif // INT16_MIN
#ifdef INT32_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_int32, int32_t, INT32_MAX);
#endif // INT32_MIN
#ifdef INT64_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_int64, int64_t, INT64_MAX);
#endif // INT64_MIN

#ifdef UINT8_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_uint8, uint8_t, UINT8_MAX);
#endif // UINT8_MIN
#ifdef UINT16_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_uint16, uint16_t, UINT16_MAX);
#endif // UINT16_MIN
#ifdef UINT32_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_uint32, uint32_t, UINT32_MAX);
#endif // UINT32_MIN
#ifdef UINT64_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_uint64, uint64_t, UINT64_MAX);
#endif // UINT64_MIN

	COMPWOLF_INT_LITERAL_OPERATOR(_int_fast8, int_fast8_t, INT_FAST8_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_int_fast16, int_fast16_t, INT_FAST16_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_int_fast32, int_fast32_t, INT_FAST32_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_int_fast64, int_fast64_t, INT_FAST64_MAX);

	COMPWOLF_INT_LITERAL_OPERATOR(_uint_fast8, uint_fast8_t, UINT_FAST8_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_uint_fast16, uint_fast16_t, UINT_FAST16_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_uint_fast32, uint_fast32_t, UINT_FAST32_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_uint_fast64, uint_fast64_t, UINT_FAST64_MAX);

	COMPWOLF_INT_LITERAL_OPERATOR(_int_least8, int_least8_t, INT_LEAST8_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_int_least16, int_least16_t, INT_LEAST16_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_int_least32, int_least32_t, INT_LEAST32_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_int_least64, int_least64_t, INT_LEAST64_MAX);

	COMPWOLF_INT_LITERAL_OPERATOR(_uint_least8, uint_least8_t, UINT_LEAST8_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_uint_least16, uint_least16_t, UINT_LEAST16_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_uint_least32, uint_least32_t, UINT_LEAST32_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_uint_least64, uint_least64_t, UINT_LEAST64_MAX);

	COMPWOLF_INT_LITERAL_OPERATOR(_intmax, intmax_t, INTMAX_MAX);
	COMPWOLF_INT_LITERAL_OPERATOR(_uintmax, uintmax_t, UINTMAX_MAX);
#ifdef INTPTR_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_intptr, intptr_t, INTPTR_MAX);
#endif // INTPTR_MAX
#ifdef UINTPTR_MAX
	COMPWOLF_INT_LITERAL_OPERATOR(_uintptr, uintptr_t, UINTPTR_MAX);
#endif // UINTPTR_MAX
}

#endif // ! COMPWOLF_INT_LITERALS_HEADER
