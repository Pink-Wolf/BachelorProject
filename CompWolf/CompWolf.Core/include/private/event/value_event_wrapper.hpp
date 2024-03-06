#ifndef COMPWOLF_VALUE_EVENT_WRAPPER_HEADER
#define COMPWOLF_VALUE_EVENT_WRAPPER_HEADER

#include "event.hpp"
#include "compwolf_type_traits"

namespace CompWolf
{
	/* Contains a value and events for before and after the value is set.
	 * The value can only be set by a protected member function.
	 * @typeparam ValueType The type of the value the value_events_wrapper contains.
	 */
	template<typename ValueType>
		requires std::is_default_constructible_v<ValueType>
	class const_value_event_wrapper
	{
	public:
		/* The type of value the value_event contains. */
		using value_type = ValueType;
		/* The type of value the value_event contains, as a reference type. */
		using reference = value_type&;
		/* The type of value the value_event contains, as a const reference type. */
		using const_reference = const value_type&;

		struct event_parameter_type
		{
			/* The value before it is changed. */
			const_reference old_value;
			/* The value after it is changed. */
			const_reference new_value;

			event_parameter_type() = default;
			event_parameter_type(const_reference new_value, const_reference old_value) :
				old_value(old_value),
				new_value(new_value)
			{}
		};

	public:
		/* Invoked right before the value is changed. */
		event<event_parameter_type> updating;
		/* Invoked right after the value is changed. */
		event<event_parameter_type> updated;
	private:
		value_type _value;

	public:
		/* Returns the value. */
		operator const_reference() const noexcept
		{
			return _value;
		}
		/* Returns the value. */
		const_reference value() const noexcept
		{
			return _value;
		}

	protected:
		template <typename ValueType>
		using is_settible = std::is_assignable<value_type, ValueType>;
		template <typename ValueType>
		using is_nothrow_settible = std::is_nothrow_assignable<value_type, ValueType>;

		template <typename ValueType>
		static constexpr bool is_settible_v = is_settible<ValueType>::value;
		template <typename ValueType>
		static constexpr bool is_nothrow_settible_v = is_nothrow_settible<ValueType>::value;

		/* Sets the value. */
		template <typename ValueType>
			requires is_settible_v<ValueType>
		auto set(ValueType new_value)
			noexcept(is_nothrow_settible_v<ValueType>)
			-> const_reference
		{
			value_type old_value = _value;
			event_parameter_type update_argument(old_value, new_value);

			updating(update_argument);
			_value = new_value;
			updated(update_argument);

			return value();
		}

	public:
		const_value_event_wrapper() = default;
		template <typename ValueType>
			requires std::is_constructible_v<value_type, ValueType>
		const_value_event_wrapper(ValueType value) : _value(value)
		{}
	};

	/* Contains a value and events for before and after the event is set.
	 * Allows getting non-const references to the events, even when this is const.
	 * @typeparam ValueType The type of the value the value_events_wrapper contains.
	 */
	template<typename ValueType>
	class value_event_wrapper : public const_value_event_wrapper<ValueType>
	{
	public:
		/* The type of value the value_event contains. */
		using value_type = const_value_event_wrapper<ValueType>::value_type;
		/* The type of value the value_event contains, as a reference type. */
		using reference = const_value_event_wrapper<ValueType>::reference;
		/* The type of value the value_event contains, as a const reference type. */
		using const_reference = const_value_event_wrapper<ValueType>::const_reference;
	protected:
		template <typename ValueType>
		static constexpr bool is_settible_v = const_value_event_wrapper<value_type>::is_settible<ValueType>::value;
		template <typename ValueType>
		static constexpr bool is_nothrow_settible_v = const_value_event_wrapper<value_type>::is_nothrow_settible<ValueType>::value;
	public:
		/* Sets the value. */
		template <typename ValueType>
			requires is_settible_v<ValueType>
		inline void set(ValueType&& new_value)
			noexcept(is_nothrow_settible_v<ValueType>)
		{
			const_value_event_wrapper::set(std::forward(new_value));
		}

		/* Sets the value. */
		template <typename ValueType>
			requires is_settible_v<ValueType>
		inline auto operator=(ValueType&& new_value)
			noexcept(is_nothrow_settible_v<ValueType>)
			-> const_reference
		{
			const_value_event_wrapper<value_type>::set<ValueType>(std::forward<ValueType>(new_value));

			return const_value_event_wrapper<ValueType>::value();
		}

	public:
		auto const_wrapper() const noexcept -> const_value_event_wrapper<value_type>&
		{
			return *static_cast<const_value_event_wrapper<value_type>*>(const_cast<value_event_wrapper*>(this));
		}
	};
}

#endif // ! COMPWOLF_VALUE_EVENT_WRAPPER_HEADER
