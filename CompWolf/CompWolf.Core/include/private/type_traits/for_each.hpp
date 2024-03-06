#ifndef COMPWOLF_FOR_EACH_HEADER
#define COMPWOLF_FOR_EACH_HEADER

#include <type_traits>
#include <tuple>

namespace CompWolf
{
	/* Allows repeating some logic for each of the given types. */
	template <typename... Ts>
	struct for_each
	{
		/* For each type T in Ts, calls Function<T>::invoke(args...). */
		template <template <typename> typename Function, typename... TArgs>
		static inline void invoke(TArgs... args)
		{

		}
	};
	template <typename T, typename... TRest>
	struct for_each<T, TRest...>
	{
		template <template <typename> typename Function, typename... TArgs>
		static inline void invoke(TArgs... args)
		{
			Function<T>::invoke(args...);
			for_each<TRest... >::template invoke<Function, TArgs...>(args...);
		}
	};

	/* Allows repeating some logic. */
	template <size_t InclusiveStart, size_t ExclusiveEnd>
	struct for_each_index
	{
		/* For each value I in [InclusiveStart; ExclusiveEnd[, calls Function<I>::invoke(args...). */
		template <template <size_t> typename Function, typename... TArgs>
		static inline void invoke(TArgs... args)
		{
			if constexpr (InclusiveStart < ExclusiveEnd)
			{
				Function<InclusiveStart>::invoke(args...);
				for_each_index<InclusiveStart + 1, ExclusiveEnd>::template invoke<Function, TArgs...>(args...);
			}
		}
	};

	/* Allows repeating some logic for each item in a tuple-like object. */
	template <typename Tuple>
	struct for_each_in
	{
	private:
		template <template <typename> typename Function, typename... TArgs>
		struct invoke_internal
		{
			template <size_t Index>
			struct index_function
			{
				static inline void invoke(Tuple& tuple, TArgs... args)
				{
					Function<std::tuple_element_t<Index, Tuple>>::template invoke(std::get<Index>(tuple), args...);
				}
			};
		};
	public:
		/* For each object o in the tuple, calls Function::invoke(o, args...). */
		template <template <typename> typename Function, typename... TArgs>
		static inline void invoke(Tuple& tuple, TArgs... args)
		{
			for_each_index<
				0, std::tuple_size_v<Tuple>
			>::template invoke<
				invoke_internal<Function, TArgs...>::template index_function, Tuple, TArgs...
			>(tuple, args...);
		}
	};
}

#endif // ! COMPWOLF_FOR_EACH_HEADER
