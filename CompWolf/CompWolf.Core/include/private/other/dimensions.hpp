#ifndef COMPWOLF_DIMENSIONS_HEADER
#define COMPWOLF_DIMENSIONS_HEADER

#include "compwolf_utility"
#include <array>
#include "compwolf_type_traits"
#include <concepts>

namespace CompWolf
{
	template <typename Type, std::size_t... Dimensions>
		requires ((Dimensions > 0) && ...)
	struct dimensions : public std::array<Type, (Dimensions * ...)>
	{
	private:
		using super = std::array<Type, (Dimensions * ...)>;
	public:

		static constexpr std::array<std::size_t, size_of<Dimensions...>> dimension_sizes{ Dimensions... };

		template <typename... IndexTypes>
			requires (std::constructible_from<std::size_t, IndexTypes&&> && ...)
		constexpr auto at(IndexTypes&&... indices) -> super::reference
		{
			std::size_t index_array[]{ static_cast<std::size_t>(std::forward<IndexTypes>(indices))... };
			constexpr std::size_t index_array_size = sizeof(index_array) / sizeof(std::size_t);
			if constexpr (index_array_size == 0) return super::at(0);
			else
			{
				std::size_t index = index_array[0];
				for (std::size_t i = 1; i < index_array_size; ++i) index += index_array[i] * dimension_sizes[i - 1];
				return super::at(index);
			}
		}
		
		template <typename... IndexTypes>
			requires (std::constructible_from<std::size_t, IndexTypes&&> && ...)
		constexpr inline auto at(IndexTypes&&... indices) const -> super::const_reference
		{
			return const_cast<dimensions*>(this)->at(indices...);
		}

	private:
		template <typename... IndexTypes>
			requires (std::constructible_from<std::size_t, IndexTypes&&> && ...)
		static bool constexpr has_index(IndexTypes&&... indices)
		{
			std::size_t index_array[]{ static_cast<std::size_t>(std::forward<IndexTypes>(indices))... };
			constexpr std::size_t index_array_size = sizeof(index_array) / sizeof(std::size_t);
			if (index_array_size > dimension_sizes.size()) return false;
			for (std::size_t i = 0; i < index_array_size; ++i) if (index_array[i] >= dimension_sizes[i]) return false;
			return true;
		}
	public:
#define COMPWOLF_DIMENSIONS_DEFINE_GETTER(getter, index)				\
		constexpr inline auto getter() noexcept -> Type&				\
			requires (has_index index)									\
		{																\
			return at index;											\
		}																\
																		\
		constexpr inline auto getter() const noexcept -> const Type&	\
			requires (has_index index)									\
		{																\
			return at index;											\
		}																\

		COMPWOLF_DIMENSIONS_DEFINE_GETTER(x, (0));
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(y, (1));
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(z, (2));
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(w, (3));

		COMPWOLF_DIMENSIONS_DEFINE_GETTER(xx, (0, 0)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(xy, (0, 1)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(xz, (0, 2)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(xw, (0, 3));
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(yx, (1, 0)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(yy, (1, 1)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(yz, (1, 2)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(yw, (1, 3));
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(zx, (2, 0)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(zy, (2, 1)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(zz, (2, 2)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(zw, (2, 3));
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(wx, (3, 0)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(wy, (3, 1)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(wz, (3, 2)); COMPWOLF_DIMENSIONS_DEFINE_GETTER(ww, (3, 3));
	};

#define COMPWOLF_DIMENSION_TYPE(type)			\
	using type##2 = dimensions<type, 2>;		\
	using type##3 = dimensions<type, 3>;		\
	using type##4 = dimensions<type, 4>;		\
	using type##2x2 = dimensions<type, 2, 2>;	\
	using type##2x3 = dimensions<type, 2, 3>;	\
	using type##2x4 = dimensions<type, 2, 4>;	\
	using type##3x2 = dimensions<type, 3, 2>;	\
	using type##3x3 = dimensions<type, 3, 3>;	\
	using type##3x4 = dimensions<type, 3, 4>;	\
	using type##4x2 = dimensions<type, 4, 2>;	\
	using type##4x3 = dimensions<type, 4, 3>;	\
	using type##4x4 = dimensions<type, 4, 4>	\

	COMPWOLF_DIMENSION_TYPE(float);
	COMPWOLF_DIMENSION_TYPE(double);
	COMPWOLF_DIMENSION_TYPE(int);
	COMPWOLF_DIMENSION_TYPE(bool);
}

#endif // ! COMPWOLF_DIMENSIONS_HEADER
