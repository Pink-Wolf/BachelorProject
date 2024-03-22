#ifndef COMPWOLF_OWNED_HEADER
#define COMPWOLF_OWNED_HEADER

#include <compwolf_type_traits>
#include "utility"

namespace CompWolf
{
	namespace Private
	{
		template <typename T>
			requires std::is_default_constructible_v<T>
		struct default_value
		{
			static auto function()
				noexcept(std::is_nothrow_default_constructible_v<T>)
				-> T
			{ return T(); }
			static constexpr T value = T();
		};
	}

	/* Contains an object and has ownership-like behavior; specifically:
	 * When default-constructed, sets the value to some default value.
	 * When move-constructed or -assigned between owned objects, sets the source-value to some default value.
	 * @typeparam NewDefault Function returning a default value.
	 */
	template <typename T, typename NewDefault = Private::default_value<T>>
		requires std::is_constructible_v<T, T&&> && std::is_assignable_v<T&, T&&>
		struct owned
	{
	public: // fields
		using new_default = NewDefault;
		static constexpr bool is_nothrow_new_default = noexcept(new_default::function());
		T value;
	public: // getters
		operator T& () noexcept
		{
			return value;
		}
		operator const T& () const noexcept
		{
			return value;
		}
		template <typename TOut = bool>
			requires is_explicitly_convertible_v<const T, TOut>
		explicit operator TOut() const
			noexcept(is_nothrow_explicitly_convertible_v<const T, TOut>)
		{
			return static_cast<TOut>(value);
		}

	public: // constructors
		owned()
			noexcept(is_nothrow_new_default && std::is_nothrow_constructible_v<T, T&&>)
			: value(T()) {}

		owned(const owned&) = default;
		auto operator=(const owned&)->owned & = default;

		owned(owned&& other)
			noexcept(std::is_nothrow_constructible_v<T&, T&&>
				&& (!std::is_nothrow_assignable_v<T&, T&&> && !is_nothrow_new_default))
		{
			value = std::move(other.value);
			other.value = owned<T>::new_default::function();
		}
		auto operator=(owned&& other)
			noexcept(std::is_nothrow_assignable_v<T&, T&&>
				&& (!std::is_nothrow_assignable_v<T&, T&&> && !is_nothrow_new_default))
			-> owned&
		{
			value = std::move(other.value);
			other.value = owned<T>::new_default::function();
			return *this;
		}

		template <typename Arg>
			requires std::is_constructible_v<T&, Arg&&>
		owned(owned<Arg>&& other)
			noexcept(std::is_nothrow_constructible_v<T&, Arg&&>
				&& (std::is_rvalue_reference_v<owned<Arg>> && (!std::is_nothrow_assignable_v<Arg&, Arg&&> && !owned<Arg>::is_nothrow_new_default)))
		{
			value = std::forward<Arg>(other.value);
			if constexpr (std::is_rvalue_reference_v<owned<Arg>>)
			{
				other.value = std::move(owned<Arg>::new_default::function());
			}
		}

		template <typename Arg>
			requires std::is_assignable_v<T&, Arg&&>
		auto operator=(owned<Arg>&& other)
			noexcept(std::is_nothrow_assignable_v<T&, Arg&&>
				&& (std::is_rvalue_reference_v<owned<Arg>> && (!std::is_nothrow_assignable_v<Arg&, Arg&&> && !owned<Arg>::is_nothrow_new_default)))
			-> owned&
		{
			value = std::forward<Arg>(other.value);
			if constexpr (std::is_rvalue_reference_v<owned<Arg>>)
			{
				other.value = std::move(owned<Arg>::new_default::function());
			}
			return *this;
		}

		template <typename... Args>
			requires std::is_constructible_v<T, Args...>
		owned(Args&&... inputs)
			noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
			: value(std::forward<Args>(inputs)...)
		{

		}

		template <typename Arg>
			requires std::is_assignable_v<T&, Arg&&>
		auto operator=(Arg&& input)
			noexcept(std::is_nothrow_assignable_v<T&, Arg&&>)
			-> owned&
		{
			value = std::forward<Arg>(input);
			return *this;
		}
	};

	template <typename TPointer>
	struct owned_ptr_default_constructor { static auto function() noexcept -> TPointer { return nullptr; } };
	template <typename TPointer>
	struct owned_ptr : public owned<TPointer, owned_ptr_default_constructor<TPointer>>
	{
	private: // fields
		using super = owned<TPointer, owned_ptr_default_constructor<TPointer>>;

	public: // getters
		template <typename T>
		auto operator *()
			noexcept(nothrow_dereferenceable_to<TPointer, T>)
			-> T&
		{
			return *super::value;
		}
		template <typename T>
		auto operator *() const
			noexcept(nothrow_dereferenceable_to<TPointer, T>)
			-> const T&
		{
			return *super::value;
		}

		auto operator ->() noexcept -> TPointer&
		{
			return super::value;
		}
		auto operator ->() const noexcept -> const TPointer&
		{
			return super::value;
		}

		template <typename T>
		auto operator [](std::size_t i)
			noexcept(noexcept(std::declval<TPointer>()[0]))
			-> T&
		{
			return super::value[i];
		}
		template <typename T>
		auto operator [](std::size_t i) const
			noexcept(noexcept(std::declval<TPointer>()[0]))
			-> const T&
		{
			return super::value[i];
		}

	public: // constructors
		using super::owned;
	};

	struct owned_bool_default_constructor { static auto function() noexcept -> bool { return false; } };
	using owned_bool = owned<bool, owned_bool_default_constructor>;
}

#endif // ! COMPWOLF_OWNED_HEADER
