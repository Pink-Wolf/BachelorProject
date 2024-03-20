#ifndef COMPWOLF_DIMENSIONS_HEADER
#define COMPWOLF_DIMENSIONS_HEADER

#include <stdio.h>
#include <array>

namespace CompWolf
{
	template <size_t Dimensions, typename Type>
	struct dimensions : public std::array<Type, Dimensions>
	{


#define COMPWOLF_DIMENSIONS_GETTER(getter, index)						\
		constexpr inline auto getter() noexcept -> Type&				\
			requires (Dimensions >= index + 1)							\
		{																\
			return this->operator[](index);								\
		}																\
																		\
		constexpr inline auto getter() const noexcept -> const Type&	\
			requires (Dimensions >= index + 1)							\
		{																\
			return this->operator[](index);								\
		}																\

		COMPWOLF_DIMENSIONS_GETTER(x, 0);
		COMPWOLF_DIMENSIONS_GETTER(y, 1);
		COMPWOLF_DIMENSIONS_GETTER(z, 2);
		COMPWOLF_DIMENSIONS_GETTER(w, 3);
	};

#define COMPWOLF_DIMENSION_TYPE(type)		\
	using type##2d = dimensions<2, type>;	\
	using type##3d = dimensions<3, type>;	\
	using type##4d = dimensions<4, type>	\

	COMPWOLF_DIMENSION_TYPE(float);
	COMPWOLF_DIMENSION_TYPE(double);
	COMPWOLF_DIMENSION_TYPE(int);
	COMPWOLF_DIMENSION_TYPE(bool);
}

#endif // ! COMPWOLF_DIMENSIONS_HEADER
