#ifndef COMPWOLF_EVENT_HEADER
#define COMPWOLF_EVENT_HEADER

#include <vector>
#include <functional>
#include <type_traits>

namespace CompWolf
{
	/* An implementation of the observer pattern whose observers are callable objects.
	 * That is to say, callable objects like functions and lambdas can be "subscribed" to an event,
	 * and the event can then be invoked to invoke all subscribed objects.
	 * @typeparam ParameterType The type of object to pass to the observers.
	 */
	template<typename ParameterType>
	class event
	{
	public:
		/* The type of object passed to observers. */
		using parameter_type = std::remove_cvref_t<ParameterType>;

		/* The type of callable object that can listen to this event. */
		using value_type = std::function<void(parameter_type&)>;
		/* The type of callable object that can listen to this event, as a reference type. */
		using reference = value_type&;
		/* The type of callable object that can listen to this event, as a const reference type. */
		using const_reference = const value_type&;
	private:
		using internal_container = std::vector<value_type>;
	public:
		/* The type used to identify a specific event. */
		using key_type = internal_container::size_type;

	private:
		/* A vector of observers, and possibly some empty functions. */
		internal_container _observers;

	public:
		/* Subscribes the given object to the event. */
		template <typename ObserverType>
			requires std::is_convertible_v<ObserverType, value_type>
		key_type subscribe(ObserverType observer) noexcept(std::is_nothrow_convertible_v<ObserverType, value_type>)
		{
			auto key = _observers.size();
			_observers.push_back(observer);
			return key;
		}
		/* Unsubscribes the given object from the event. */
		key_type unsubscribe(key_type observer_key) noexcept
		{
			_observers[observer_key] = value_type();
		}

		/* Invokes all subscribed objects, passing the given parameters to each. */
		void invoke(parameter_type& parameter)
		{
			for (auto& observer : _observers)
			{
				if (observer == nullptr) continue;
				observer(parameter);
			}
		}
		/* Invokes all subscribed objects, passing the given parameters to each. */
		void operator()(parameter_type& parameter)
		{
			for (auto& observer : _observers)
			{
				if (observer == nullptr) continue;
				observer(parameter);
			}
		}
	};
}

#endif // ! COMPWOLF_EVENT_HEADER
