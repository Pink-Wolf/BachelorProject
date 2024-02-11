#ifndef COMPWOLF_INDEX_FUNCTION_HEADER
#define COMPWOLF_INDEX_FUNCTION_HEADER

#include <functional>
#include "functional.hpp"
#include <concepts>
#include "compwolf_type_traits"

namespace CompWolf
{
	/* A wrapper for a callable object, which passes the amount of times this has been called along to the wrapped object.
	 * @typeparam FunctionSignature The signature of the functions the index_function wraps, for example "void(int)".
	 */
	template <typename FunctionSignature>
	class index_function
	{
		// Using is_same_v to make the assertion a type-dependent expression.
		static_assert(std::is_same_v<FunctionSignature, FunctionSignature>, "Tried defining an index_function with a signature it could not understand. Its type parameter must be of a form like \"void(int)\"");
	};
	template<typename ReturnType, typename... ParameterTypes>
	class index_function<ReturnType(ParameterTypes...)>
	{
	private:
		using function_type = std::function<ReturnType(int, ParameterTypes...)>;
		function_type function;
		int counter = 0;

	public:
		/* Creates an empty index_function. */
		inline index_function() noexcept = default;

		/* Creates a index_function with the given callable object. */
		template<typename... InputTypes>
			requires std::constructible_from<function_type, InputTypes...>
		inline index_function(InputTypes... inputs)
			noexcept(std::is_nothrow_constructible_v<function_type, InputTypes...>)
			: function(inputs...) {}

		/* Sets the callable object the index_function contains. */
		template<typename InputType>
			requires std::assignable_from<function_type, InputType>
		inline auto operator =(InputType new_function)
			noexcept(std::is_nothrow_assignable_v<function_type, InputType>)
			-> index_function
		{
			function = new_function;
		}

		/* Swaps what callable function the index_function has with another, as well as how many times they have been invoked. */
		void swap(index_function& other) noexcept
		{
			function.swap(other.function);
			counter.swap(other.counter);
		}

		/* Whether the index_function is empty, as in containing no callable object. */
		inline explicit operator bool() const noexcept
		{
			return static_cast<bool>(function);
		}

		/* Invokes the contained object with the given arguments. */
		ReturnType operator()(ParameterTypes... arguments)
		{
			auto index = counter++; // index equals the counter before it was incremented

			if (std::is_void_v<ReturnType>)
				function(index, arguments...);
			else
				return function(index, arguments...);
		}

		/* Returns the amount of times the index_function has been invoked. */
		inline int get_counter() noexcept { return counter; }
		/* Tells the index_function that it has been invoked the given amount of times.
		 * @return The amount of times the index_function thought it had been invoked.
		 */
		inline int set_counter(int new_count) noexcept
		{
			auto old = counter;
			counter = new_count;
			return old;
		}
		/* Tells the index_function that it has not been invoked yet.
		 * @return The amount of times the index_function thought it had been invoked.
		 */
		inline int reset_counter() noexcept { return set_counter(0); }
	};
}

#endif // ! COMPWOLF_INDEX_FUNCTION_HEADER
