#ifndef COMPWOLF_FREE_ON_DEPENDENT_FREED_HEADER
#define COMPWOLF_FREE_ON_DEPENDENT_FREED_HEADER

#include "freeable"
#include "event"
#include <concepts>
#include <optional>

namespace CompWolf
{
	template <Freeable DependentType>
		requires std::derived_from<decltype(DependentType::freeing), event<>>
	struct free_on_dependent_freed : public virtual freeable
	{
	private: // fields
		DependentType* _dependent;

		using referenced_freeing_parameter_type = void;
		using referenced_freeing_type = event<referenced_freeing_parameter_type>;
		using referenced_freeing_key_type = referenced_freeing_type::key_type;
		std::optional<referenced_freeing_key_type> _referenced_freeing_key;

	protected: // getters
		inline auto is_subscribed_to_dependent() const noexcept -> bool
		{
			return _referenced_freeing_key.has_value();
		}

		void get_dependent(DependentType*& dependent) noexcept
		{
			dependent = _dependent;
		}
		void get_dependent(const DependentType*& dependent) const noexcept
		{
			dependent = _dependent;
		}

	protected: // setters
		void set_dependent(DependentType* dependent) noexcept
		{
			if (is_subscribed_to_dependent()) _dependent->freeing.unsubscribe(_referenced_freeing_key.value());
			_dependent = dependent;
			if (!_dependent)
				_referenced_freeing_key = std::nullopt;
			else
				_referenced_freeing_key = _dependent->freeing.subscribe([this](const referenced_freeing_type&)
					{
						free();
						set_dependent(nullptr);
					}
			);
		}
	public: // constructors
		free_on_dependent_freed() = default;
		free_on_dependent_freed(free_on_dependent_freed& other) noexcept
		{
			set_dependent(other._dependent);
		}
		auto operator=(free_on_dependent_freed& other) noexcept -> free_on_dependent_freed&
		{
			set_dependent(other._dependent);
		}
		free_on_dependent_freed(free_on_dependent_freed&& other) noexcept
		{
			set_dependent(other._dependent);
			other.set_dependent(nullptr);
		}
		auto operator=(free_on_dependent_freed&& other) noexcept -> free_on_dependent_freed&
		{
			set_dependent(other._dependent);
			other.set_dependent(nullptr);
		}
		~free_on_dependent_freed() noexcept
		{
			set_dependent(nullptr);
		}

		free_on_dependent_freed(DependentType& dependent) noexcept
		{
			set_dependent(&dependent);
		}
	};
}

#endif // ! COMPWOLF_FREE_ON_DEPENDENT_FREED_HEADER
