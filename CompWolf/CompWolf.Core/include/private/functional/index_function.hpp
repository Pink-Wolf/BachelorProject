#ifndef COMPWOLF_INDEX_FUNCTION_HEADER
#define COMPWOLF_INDEX_FUNCTION_HEADER

#include <functional>
#include <concepts>
#include <utility>
#include "compwolf_type_traits"
#include "freeable"

namespace CompWolf
{
	/* Function wrapper like std::function, which contains a counter for the amount of times this is invoked, and passes that along as the first argument to the contained function/functor.
	 * @typeparam FunctionSignature The signature of the contained function, like void(int, bool).
	 * This must have int as its first argument.
	 */
	template <typename FunctionSignature>
	class index_function
	{
		static_assert(dependent_false<FunctionSignature>, "Tried defining an index_function with a signature it could not understand. Its type parameter must be of a form like \"void(int, bool)\"");
		
	public: // constructors (included for IDE and name lookup support)
		/* Constructs a freed index_function, as in one that cannot be called. */
		index_function() noexcept = default;
		index_function(const index_function&) = default;
		auto operator=(const index_function&) -> index_function& = default;
		index_function(index_function&&) = default;
		auto operator=(index_function&&) -> index_function& = default;

		/* Constructs a freed index_function, as in one that cannot be called. */
		inline index_function(std::nullptr_t) noexcept : index_function() {}

		/* Constructs an index_function containing the given function. */
		template<typename FunctionType>
			requires std::constructible_from<std::function<FunctionSignature>, FunctionType>
		inline index_function(FunctionType&& function)
			noexcept(std::is_nothrow_constructible_v<std::function<FunctionSignature>, FunctionType>)
		{}
	};
	template<typename ReturnType, typename... ParameterTypes>
	class index_function<ReturnType(int, ParameterTypes...)> : basic_freeable
	{
	public: // type definitions
		/* The return type of the contained function. */
		using result_type = ReturnType;
		/* The parameter types of the contained function, including the counter that index_function provides, in a type_list. */
		using argument_types = type_list<int, ParameterTypes...>;
		/* The parameter types of the index_function, not including the counter that index_function passes to its contained function, in a type_list. */
		using argument_types_excluding_counter = type_list<ParameterTypes...>;

	private: // fields
		std::function<ReturnType(int, ParameterTypes...)> _function;
		int _counter = 0;

	public: // accessors
		/* Gets the amount of times the function has been called. */
		inline auto& counter() noexcept { return _counter; }
		/* Gets the amount of times the function has been called. */
		inline auto counter() const noexcept { return _counter; }

	public: // modifiers
		/* Resets the index_function's counter to 0, making it think its function has been called 0 times so far. */
		inline void reset_counter() noexcept { counter() = 0; }

	public: // operators
		/* Invokes the contained function, passing along the index_function's counter and the given arguments. */
		inline ReturnType invoke(ParameterTypes... arguments)
		{
			auto index = _counter++; // index equals the counter before it was incremented
			return _function(index, arguments...);
		}
		/* Invokes the contained function, passing along the index_function's counter and the given arguments. */
		inline ReturnType operator()(ParameterTypes... arguments) { return invoke(arguments...); }

	public: // constructors
		/* Constructs a freed index_function, as in one that cannot be called. */
		index_function() noexcept = default;
		index_function(const index_function&) = default;
		auto operator=(const index_function&) -> index_function& = default;
		index_function(index_function&&) = default;
		auto operator=(index_function&&) -> index_function& = default;

		/* Constructs a freed index_function, as in one that cannot be called. */
		inline index_function(std::nullptr_t) noexcept : index_function() {}

		/* Constructs an index_function containing the given function. */
		template<typename FunctionType>
			requires std::constructible_from<std::function<ReturnType(int, ParameterTypes...)>, FunctionType>
		inline index_function(FunctionType&& function)
			noexcept(std::is_nothrow_constructible_v<std::function<ReturnType(int, ParameterTypes...)>, FunctionType>)
			: _function(std::forward<FunctionType>(function)) {}
	};
}

#endif // ! COMPWOLF_INDEX_FUNCTION_HEADER
