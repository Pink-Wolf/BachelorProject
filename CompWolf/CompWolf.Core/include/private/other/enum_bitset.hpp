#ifndef COMPWOLF_ENUM_BITSET_HEADER
#define COMPWOLF_ENUM_BITSET_HEADER

#include <bitset>
#include "compwolf_type_traits"
#include "compwolf_functional"
#include <initializer_list>

namespace CompWolf
{
	/* A std::bitset whose bits represent different values from an EnumType.
	 * As in an associative container with EnumType-values as keys and bools/bits as values.
	 * @typeparam EnumType The type whose different values are represented by the bitset.
	 * * The values must be explicitly convertible to a size_t denoting their position in the bitset.
	 * * Not all values need to be represented in the bitset. Values not represented must be converted to a position outside of the bitfield.
	 * * Unless Size is specified, EnumType::size must also be convertible to size_t.
	 * * An enum inheriting an integral type, whose elements are not set to a specific value, and whose last element is "size", fulfills these requirements.
	 * @typeparam Size The amount of bits in the bitset. By default EnumType::size.
	 */
	template <typename EnumType, size_t Size = static_cast<size_t>(EnumType::size)>
		requires explicitly_convertible_to<EnumType, size_t>
	struct enum_bitset : public std::bitset<Size>
	{
	public: using super = std::template bitset<Size>;
	private:
		/* Gets the position of the bit representing the given enum-value. */
		std::size_t to_pos(EnumType pos)
		{
			return static_cast<std::size_t>(pos);
		}
	public:
		/* Accessees the bit representing the given enum-value. */
		bool operator[](EnumType pos) const
		{
			return super::operator[](to_pos(pos));
		}
		/* Accessees the bit representing the given enum-value. */
		super::reference operator[](EnumType pos)
		{
			return super::operator[](to_pos(pos));
		}
		/* Accessees the bit representing the given enum-value.
		 * @throws std::out_of_range if the given value is not represented in the bitset.
		 */
		bool test(EnumType pos) const
		{
			return test(to_pos(pos));
		}

		/* Sets the bit representing the given enum-value to the given value.
		 * @throws std::out_of_range if the given value is not represented in the bitset.
		 */
		enum_bitset& set(EnumType pos, bool value = true)
		{
			set(to_pos(pos), value);
			return *this;
		}
		/* Sets the bit representing the given enum-value to 0.
		 * @throws std::out_of_range if the given value is not represented in the bitset.
		 */
		enum_bitset& reset(EnumType pos)
		{
			reset(to_pos(pos));
			return *this;
		}
		/* Flips the bit representing the given enum-value.
		 * @throws std::out_of_range if the given value is not represented in the bitset.
		 */
		enum_bitset& flip(EnumType pos)
		{
			flip(to_pos(pos));
			return *this;
		}

	public:
		constexpr enum_bitset() noexcept = default;

		using super::super;

		/* Constructs a bitset with only the given enum-value's bit being 1. */
		enum_bitset(EnumType value) : enum_bitset()
		{
			set(value);
		}
		/* Constructs a bitset with only the given enum-value's bit being 1. */
		enum_bitset(std::initializer_list<EnumType> values) : enum_bitset()
		{
			for (auto value : values)
			{
				set(value);
			}
		}
		/* Sets the bits representing the given values to 1, and the rest to 0. */
		enum_bitset& operator =(std::initializer_list<EnumType> values)
		{
			reset();
			for (auto value : values)
			{
				set(value);
			}
		}
	};
}

#endif // ! COMPWOLF_ENUM_BITSET_HEADER
