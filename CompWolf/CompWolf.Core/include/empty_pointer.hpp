#ifndef COMPWOLF_EMPTY_POINTER_HEADER
#define COMPWOLF_EMPTY_POINTER_HEADER

#include <memory>
#include <utility>

namespace CompWolf
{
	struct empty_pointer;
	struct empty_pointer_element;

	/* The type that empty_pointer points to.
	 * @see empty_pointer
	 */
	struct empty_pointer_element
	{
	public:
		using pointer = empty_pointer;
		using element_type = empty_pointer_element;
		using difference_type = std::ptrdiff_t;

	public:
		/* The object received when dereferencing an empty_pointer. */
		static empty_pointer_element instance;
	};

	/* A pointer that can point to nothing while not being equal to nullptr.
	 * All non-nullptr empty_pointers are equal.
	 * @see empty_pointer_element
	 */
	struct empty_pointer
	{
	public:
		using pointer = empty_pointer;
		using element_type = empty_pointer_element;
		using difference_type = std::ptrdiff_t;

	private:
		/* Whether the object is not a nullptr. */
		bool has_value = false;

	public:
		/* Constructs a non-nullptr empty_pointer. */
		constexpr inline empty_pointer() noexcept = default;
		/* @param is_not_nullptr Whether the constructed empty_pointer is not a nullptr. */
		constexpr inline empty_pointer(bool is_not_nullptr) noexcept : has_value(is_not_nullptr) {}
		/* Constructs a nullptr empty_pointer. */
		constexpr inline empty_pointer(std::nullptr_t) noexcept : empty_pointer(false) {}

		/* Whether the empty_pointer is not a nullptr. */
		constexpr inline operator bool() const noexcept { return has_value; }
		/* Whether the empty_pointer is a nullptr. */
		constexpr inline bool is_nullptr() const noexcept { return !has_value; }
		/* Sets whether the empty_pointer is a nullptr. */
		constexpr inline void set_nullptr(bool is_nullptr = true) noexcept { has_value = !is_nullptr; }

		constexpr inline void swap(empty_pointer& other) noexcept { std::swap(has_value, other.has_value); }

		/* Returns empty_pointer_element::instance.
		 * @see empty_pointer_element::instance
		 */
		constexpr inline empty_pointer_element& operator*() noexcept { return empty_pointer_element::instance; }
		/* Returns a pointer to empty_pointer_element::instance.
		 * @see empty_pointer_element::instance
		 */
		constexpr inline empty_pointer_element* operator->() noexcept { return &empty_pointer_element::instance; }
	};

	/* Whether the empty_pointer is a nullptr. */
	inline bool operator==(const empty_pointer& p, const std::nullptr_t&) noexcept { return p.is_nullptr(); }
	/* Whether the empty_pointer is a nullptr. */
	inline bool operator==(const std::nullptr_t&, const empty_pointer& p) noexcept { return p.is_nullptr(); }

	/* Whether both or neither empty_pointers are a nullptr. */
	inline bool operator==(const empty_pointer& lhs, const empty_pointer& rhs) noexcept { return lhs.is_nullptr() == rhs.is_nullptr(); }
}

#endif // ! COMPWOLF_EMPTY_POINTER_HEADER
