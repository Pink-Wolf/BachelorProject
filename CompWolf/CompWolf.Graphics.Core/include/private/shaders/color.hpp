#ifndef COMPWOLF_GRAPHICS_COLOR_HEADER
#define COMPWOLF_GRAPHICS_COLOR_HEADER

#include <dimensions>
#include <utility>
#include <cstdint>
#include <initializer_list>
#include <algorithm>

namespace CompWolf::Graphics
{
	struct single_color
	{
	private: // fields
		uint8_t _data;

	public: // modifiers
		/* Sets the color to the given value, where 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 */
		constexpr auto& set(long double f) { _data = static_cast<uint8_t>(std::clamp(f * 255.L, 0.L, 255.L)); return *this; }
		/* Sets the color to the given value, where 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 */
		constexpr auto& set(double f) { _data = static_cast<uint8_t>(std::clamp(f * 255., 0., 255.)); return *this; }
		/* Sets the color to the given value, where 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 */
		constexpr auto& set(float f) { _data = static_cast<uint8_t>(std::clamp(f * 255.f, 0.f, 255.f)); return *this; }
		/* Sets the color to the given value, where 0 is no color and 255 is the max amount of color. */
		constexpr auto& set(uint8_t i) { _data = i; return *this; }

	public: // operators
		/* single_color can be explicit converted to a long double, where no color is 0 and the max amount of color is 1. */
		inline constexpr explicit operator long double() const { return _data / 255.L; }
		/* single_color can be explicit converted to a double, where no color is 0 and the max amount of color is 1. */
		inline constexpr explicit operator double() const { return _data / 255.; }
		/* single_color can be explicit converted to a float, where no color is 0 and the max amount of color is 1. */
		inline constexpr explicit operator float() const { return _data / 255.f; }
		/* single_color can be explicit converted to a uint8_t, where no color is 0 and the max amount of color is 255. */
		inline constexpr explicit operator uint8_t() const { return _data; }

	public: // constructors
		/* Constructs a single_color with no color. */
		single_color() = default;
		single_color(const single_color&) = default;
		auto operator=(const single_color&) -> single_color& = default;
		single_color(single_color&&) = default;
		auto operator=(single_color&&) -> single_color& = default;

		/* Constructs a single_color with the given value, where 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 */
		explicit inline constexpr single_color(long double f) { set(f); }
		/* Constructs a single_color with the given value, where 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 */
		explicit inline constexpr single_color(double f) { set(f); }
		/* Constructs a single_color with the given value, where 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 */
		explicit inline constexpr single_color(float f) { set(f); }
		/* Constructs a single_color with the given value, where 0 is no color and 255 is the max amount of color. */
		explicit inline constexpr single_color(uint8_t i) : _data(i) {}
	};

	inline constexpr auto operator""_scolor(long double f) { return single_color(f); }
	inline constexpr auto operator+(single_color lhs, single_color rhs) { return single_color(static_cast<uint8_t>(static_cast<uint8_t>(lhs) + static_cast<uint8_t>(rhs))); }
	inline constexpr auto operator-(single_color lhs, single_color rhs) { return single_color(static_cast<uint8_t>(static_cast<uint8_t>(lhs) - static_cast<uint8_t>(rhs))); }
	inline constexpr auto operator*(single_color lhs, single_color rhs) { return single_color(static_cast<uint8_t>(static_cast<uint8_t>(lhs) * static_cast<uint8_t>(rhs))); }
	inline constexpr auto operator/(single_color lhs, single_color rhs) { return single_color(static_cast<uint8_t>(static_cast<uint8_t>(lhs) / static_cast<uint8_t>(rhs))); }
	inline constexpr auto operator<=>(single_color lhs, single_color rhs) { return static_cast<uint8_t>(lhs) <=> static_cast<uint8_t>(rhs); }
	inline constexpr auto operator==(single_color lhs, single_color rhs) { return static_cast<uint8_t>(lhs) == static_cast<uint8_t>(rhs); }
	inline constexpr auto operator!=(single_color lhs, single_color rhs) { return static_cast<uint8_t>(lhs) != static_cast<uint8_t>(rhs); }

	struct transparent_color;
	/* A color made out of a mix of red, green, and blue. */
	struct opaque_color : dimensions<single_color, 3>
	{
		/* Returns the amount of red used to make the color. */
		inline constexpr auto& r() { return x(); }
		/* Returns the amount of red used to make the color. */
		inline constexpr auto& r() const { return x(); }
		/* Returns the amount of green used to make the color. */
		inline constexpr auto& g() { return y(); }
		/* Returns the amount of green used to make the color. */
		inline constexpr auto& g() const { return y(); }
		/* Returns the amount of blue used to make the color. */
		inline constexpr auto& b() { return z(); }
		/* Returns the amount of blue used to make the color. */
		inline constexpr auto& b() const { return z(); }

		/* A black color. */
		opaque_color() = default;
		opaque_color(const opaque_color&) = default;
		auto operator=(const opaque_color&) -> opaque_color& = default;
		opaque_color(opaque_color&&) = default;
		auto operator=(opaque_color&&) -> opaque_color& = default;

		/* Constructs a color from mixing the given amount of red, green, and blue. */
		inline constexpr opaque_color(single_color r, single_color g, single_color b) noexcept
			: dimensions<single_color, 3>({ r, g, b }) {}
		/* Constructs a color from mixing the given amount of red, green, and blue.
		 * The values in initializer_list are red, green, and blue respectively.
		 */
		inline constexpr opaque_color(std::initializer_list<single_color> v) noexcept
		{
			auto i = v.begin();
			if (i) { r() = *i; ++i; }
			if (i) { g() = *i; ++i; }
			if (i) { b() = *i; ++i; }
		}
		/* Constructs a color from mixing the given amount of red, green, and blue,
		 * The values in initializer_list are red, green, and blue respectively.
		 * The floats are converted to single_colors.
		 */
		inline constexpr opaque_color(std::initializer_list<float> v) noexcept
		{
			auto i = v.begin();
			if (i) { r() = single_color(*i); ++i; }
			if (i) { g() = single_color(*i); ++i; }
			if (i) { b() = single_color(*i); ++i; }
		}

		constexpr operator transparent_color() const;
	};
	/* A color made out of a mix of red, green, and blue, and an alpha specifying how much one can see through the color. */
	struct transparent_color : dimensions<single_color, 4>
	{
		/* Returns the amount of red used to make the color. */
		inline constexpr auto& r() { return x(); }
		/* Returns the amount of red used to make the color. */
		inline constexpr auto& r() const { return x(); }
		/* Returns the amount of green used to make the color. */
		inline constexpr auto& g() { return y(); }
		/* Returns the amount of green used to make the color. */
		inline constexpr auto& g() const { return y(); }
		/* Returns the amount of blue used to make the color. */
		inline constexpr auto& b() { return z(); }
		/* Returns the amount of blue used to make the color. */
		inline constexpr auto& b() const { return z(); }
		/* Returns the alpha of the color. */
		inline constexpr auto& a() { return w(); }
		/* Returns the alpha of the color. */
		inline constexpr auto& a() const { return w(); }

		/* A completely transparent color. */
		transparent_color() = default;
		transparent_color(const transparent_color&) = default;
		auto operator=(const transparent_color&) -> transparent_color& = default;
		transparent_color(transparent_color&&) = default;
		auto operator=(transparent_color&&) -> transparent_color& = default;

		/* Constructs a color from mixing the given amount of red, green, and blue, and with the given transparency. */
		inline constexpr transparent_color(single_color r, single_color g, single_color b, single_color alpha = 1._scolor) noexcept
			: dimensions<single_color, 4>({ r, g, b, alpha }) {}
		/* Constructs a color from mixing the given amount of red, green, and blue, and with the given transparency.
		 * The values in initializer_list are red, green, blue, and alpha respectively.
		 */
		inline constexpr transparent_color(std::initializer_list<single_color> v) noexcept
		{
			auto i = v.begin();
			if (i) { r() = *i; ++i; }
			if (i) { g() = *i; ++i; }
			if (i) { b() = *i; ++i; }
			if (i) { a() = *i; ++i; }
		}
		/* Constructs a color from mixing the given amount of red, green, and blue, and with the given transparency.
		 * The values in initializer_list are red, green, blue, and alpha respectively.
		 * The floats are converted to single_colors.
		 */
		inline constexpr transparent_color(std::initializer_list<float> v) noexcept
		{
			auto i = v.begin();
			if (i) { r() = single_color(*i); ++i; }
			if (i) { g() = single_color(*i); ++i; }
			if (i) { b() = single_color(*i); ++i; }
			if (i) { a() = single_color(*i); ++i; }
		}

		explicit constexpr operator opaque_color() const;
	};
	inline constexpr opaque_color::operator transparent_color() const { return transparent_color{ r(), g(), b(), 1._scolor }; }
	inline constexpr transparent_color::operator opaque_color() const { return opaque_color{ r(), g(), b() }; }
}

#endif // ! COMPWOLF_GRAPHICS_COLOR_HEADER
