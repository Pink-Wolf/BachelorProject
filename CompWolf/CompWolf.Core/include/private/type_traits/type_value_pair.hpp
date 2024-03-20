#ifndef COMPWOLF_TYPE_VALUE_PAIR_HEADER
#define COMPWOLF_TYPE_VALUE_PAIR_HEADER

namespace CompWolf
{
	template <typename Type, size_t Value>
	struct type_value_pair
	{
		using type = Type;
		static constexpr auto value = Value;
	};

	namespace Private
	{
		template <typename T>
		struct type_value_pair_default_requirement : std::true_type {};
	}
	template <typename T>
	concept TypeValuePair = requires
	{
		typename T::type;
		{ T::value };
	};
}

#endif // ! COMPWOLF_TYPE_VALUE_PAIR_HEADER
