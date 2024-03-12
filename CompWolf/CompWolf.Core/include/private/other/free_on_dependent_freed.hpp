#ifndef COMPWOLF_FREE_ON_DEPENDENT_FREED_HEADER
#define COMPWOLF_FREE_ON_DEPENDENT_FREED_HEADER

#include "freeable"
#include "event"
#include <optional>

namespace CompWolf
{
	/* A Freeable who has events for being freed. */
	template <typename T>
	concept FreeableWithEvents = Freeable<T> && requires (T t)
	{
		typename T::freeing_parameter_type;

		{ t.freeing() } -> std::derived_from<event<typename T::freeing_parameter_type>>;
	};
	/* A Freeable who has events for being freed. */
	template <typename T>
	struct is_freeable_with_events : std::false_type {};
	template <FreeableWithEvents T>
	struct is_freeable_with_events<T> : std::true_type {};
	/* A Freeable who has events for being freed. */
	template <FreeableWithEvents T>
	static constexpr bool is_freeable_with_events = FreeableWithEvents<T>;

	template <FreeableWithEvents DependentType>
	struct free_on_dependent_freed : public virtual freeable
	{
	private: // fields
		using referenced_freeing_parameter_type = DependentType::freeing_parameter_type;
		using referenced_freeing_type = event<referenced_freeing_parameter_type>;
		using referenced_freeing_key_type = referenced_freeing_type::key_type;
		std::optional<referenced_freeing_key_type> _referenced_freeing_key;
	private: // event handlers
		void on_dependent_freeing(referenced_freeing_type&, referenced_freeing_parameter_type&);
	protected: // other methods
		inline auto is_subscribed_to_dependent() const -> bool
		{
			return _referenced_freeing_key;
		}
		void subscribe_to_dependent(DependentType& a)
		{
			_referenced_freeing_key = a.freeing.subscribe(on_dependent_freeing);
		}
		void unsubscribe_from_dependent(DependentType& a)
		{
			a.freeing.unsubscribe(_referenced_freeing_key);
		}
	public: // constructors
		basic_association_relationship() = default;
	};

	template <FreeableWithEvents DependentType>
	void free_on_dependent_freed<DependentType>::on_dependent_freeing(
		referenced_freeing_type& sender,
		referenced_freeing_parameter_type& a)
	{
		free();
		unsubscribe_from_dependent(sender);
	}
}

#endif // ! COMPWOLF_FREE_ON_DEPENDENT_FREED_HEADER
