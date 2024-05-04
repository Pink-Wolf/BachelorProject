#ifndef COMPWOLF_VALUE_LISTENABLE_HEADER
#define COMPWOLF_VALUE_LISTENABLE_HEADER

#include "event.hpp"
#include "compwolf_type_traits"
#include <utility>

namespace CompWolf
{
	/* Contains a value and an event that is invoked when the value changes.
	 * @typeparam ValueType The type of value that the listenable contains.
	 * The value must be copyable.
	 */
	template<typename ValueType>
		requires std::is_move_constructible_v<ValueType> && std::is_move_assignable_v<ValueType>
	class listenable
	{
	public: // type definitions
		/* The type of value that the listenable contains. */
		using value_type = ValueType;
		/* The type used to pass the value by reference. */
		using reference_type = std::add_lvalue_reference_t<value_type>;
		/* The type used to pass the value by const reference. */
		using const_reference_type = std::add_const_t<reference_type>;

		/* Aggregate type used by listenable::value_updated. */
		struct update_parameters
		{
			/* The value before it is/was updated.
			 * For listenable::value_updating, the actual value; for listenable::value_updated, a copy of the old value.
			 */
			listenable::const_reference_type old_value;
			/* The value after it is/was updated.
			 * For listenable::value_updating, a copy of the new value; for listenable::value_updated, the actual value.
			 */
			listenable::const_reference_type new_value;

			update_parameters() = default;
			update_parameters(listenable::const_reference_type new_value, listenable::const_reference_type old_value)
				: old_value(old_value), new_value(new_value)
			{}
		};

	private: // fields
		value_type _value;
		event<update_parameters&> _value_updating;
		event<update_parameters&> _value_updated;

	public: // accessors
		/* Returns the value. */
		inline auto& value() const noexcept { return _value; }

		/* Returns the event invoked before the value is changed. */
		inline auto& value_updating() noexcept { return _value_updating; }
		/* Returns the event invoked before the value is changed. */
		inline auto& value_updating() const noexcept { return _value_updating; }

		/* Returns the event invoked after the value is changed. */
		inline auto& value_updated() noexcept { return _value_updated; }
		/* Returns the event invoked after the value is changed. */
		inline auto& value_updated() const noexcept { return _value_updated; }

	public: // modifiers
		/* Sets the value to the given input. */
		template <typename InputType>
			requires std::is_assignable_v<value_type, InputType&&>
				&& std::is_constructible_v<value_type, InputType&&>
			auto& set_value(InputType&& input)
			noexcept(std::is_nothrow_assignable_v<value_type, InputType&&>
				&& std::is_nothrow_constructible_v<value_type, InputType&&>
				&& std::is_nothrow_move_assignable_v<value_type>)
		{
			value_type new_value(std::forward<InputType>(input));

			update_parameters updating_args(_value, new_value);
			_value_updating(updating_args);

			value_type old_value = std::move(_value);
			_value = std::move(new_value);

			update_parameters updated_args(old_value, _value);
			_value_updated(updated_args);

			return value();
		}

	public: // constructors
		listenable(const listenable& other)
			noexcept(std::is_nothrow_copy_constructible_v<value_type>)
			: _value(other._value)
			, _value_updating(other._value_updating)
			, _value_updated(other._value_updated)
		{}
		auto operator=(const listenable& other)
			noexcept(std::is_nothrow_copy_assignable_v<value_type>)
		{
			_value = other._value;
			_value_updating = other._value_updating;
			_value_updated = other._value_updated;
		}

		listenable(listenable&& other)
			noexcept(std::is_nothrow_move_constructible_v<value_type>)
			: _value(std::move(other._value))
			, _value_updating(std::move(other._value_updating))
			, _value_updated(std::move(other._value_updated))
		{}
		auto operator=(listenable&& other)
			noexcept(std::is_nothrow_move_assignable_v<value_type>)
		{
			_value = std::move(other._value);
			_value_updating = std::move(other._value_updating);
			_value_updated = std::move(other._value_updated);
		}

		template <typename... ValueParameterTypes>
			requires std::is_constructible_v<value_type, ValueParameterTypes&&...>
		listenable(ValueParameterTypes&&... value_arguments)
			noexcept(std::is_nothrow_constructible_v<value_type, ValueParameterTypes&&...>)
			: _value(std::forward<ValueParameterTypes>(value_arguments)...)
		{}

		template <typename InputType>
			requires std::is_assignable_v<value_type, InputType&&>
			&& std::is_constructible_v<value_type, InputType&&>
		auto& operator=(InputType&& input)
			noexcept(std::is_nothrow_assignable_v<value_type, InputType&&>
				&& std::is_nothrow_constructible_v<value_type, InputType&&>
				&& std::is_nothrow_move_assignable_v<value_type>)
		{
				return set_value(input);
		}
	};
}

#endif // ! COMPWOLF_VALUE_LISTENABLE_HEADER
