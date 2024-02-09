#ifndef COMPWOLF_VALUE_EVENT_WRAPPER_HEADER
#define COMPWOLF_VALUE_EVENT_WRAPPER_HEADER

#include "event.h"
#include "compwolf_type_traits.h"

namespace CompWolf
{
	/* Contains a value and events for before and after the event is set.
	 * @typeparam ValueType The type of the value the value_events_wrapper contains.
	 */
	template<typename ValueType>
	class value_event_wrapper
	{
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

		/* Sets the value. */
		template <typename ValueType>
			requires std::is_assignable_v<value_type, ValueType>
		operator =(ValueType new_value)
			noexcept(std::is_nothrow_assignable_v<value_type, ValueType>
				&& std::is_nothrow_copy_constructible_v<value_type>)
		{
			value_type old_value(value);
			event_parameter_type update_argument{
				.old_value = old_value,
				.new_value = new_value,
			};

			updating(update_argument);
			_value = new_value;
			updated(update_argument);
		}

		/* Adds the given delta to the value. */
		template <typename DeltaType>
			requires is_addition_assignable_v<value_type, DeltaType>
		operator +=(DeltaType delta)
			noexcept(is_nothrow_addition_assignable_v<value_type, DeltaType>
				&& std::is_nothrow_copy_constructible_v<value_type>)
		{
			value_type old_value(value);
			event_parameter_type update_argument{
				.old_value = old_value,
				.new_value = new_value,
			};

			updating(update_argument);
			_value += delta;
			updated(update_argument);
		}
	};
}

#endif // ! COMPWOLF_VALUE_EVENT_WRAPPER_HEADER
