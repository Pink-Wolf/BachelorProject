#ifndef COMPWOLF_REFERENCE_CONTAINER_HEADER
#define COMPWOLF_REFERENCE_CONTAINER_HEADER

#define define_reference_container(contained_type, interface_name, getter_method)									\
struct const_##interface_name																						\
{																													\
	virtual const contained_type& getter_method() const = 0;														\
};																													\
																													\
struct interface_name : const_##interface_name																		\
{																													\
	virtual contained_type& getter_method() = 0;																	\
	const contained_type& getter_method() const override															\
	{																												\
		return const_cast<interface_name*>(this)->getter_method();													\
	}																												\
};																													\
																													\
class basic_const_##interface_name : public const_##interface_name													\
{																													\
private:																											\
	const contained_type* _reference;																				\
public:																												\
	/* Constructs an empty container, making the getters have undefined behaviour. */								\
	basic_const_##interface_name() = default;																		\
	inline basic_const_##interface_name(const contained_type& reference) noexcept									\
	{																												\
		_reference = &reference;																					\
	}																												\
																													\
public:																												\
	inline const contained_type& getter_method() const noexcept final												\
	{																												\
		return *_reference;																							\
	}																												\
																													\
protected:																											\
	/* Sets what gpu contains the object. */																		\
	inline void set_##getter_method(const contained_type& reference) noexcept										\
	{																												\
		_reference = &reference;																					\
	}																												\
};																													\
																													\
class basic_##interface_name : public interface_name																\
{																													\
private:																											\
	contained_type* _reference;																						\
public:																												\
	/* Constructs an empty container, making the getters have undefined behaviour. */								\
	basic_##interface_name() = default;																				\
	inline basic_##interface_name(contained_type& reference) noexcept												\
	{																												\
		_reference = &reference;																					\
	}																												\
																													\
public:																												\
	inline contained_type& getter_method() noexcept final															\
	{																												\
		return *_reference;																							\
	}																												\
	inline const contained_type& getter_method() const noexcept final												\
	{																												\
		return *_reference;																							\
	}																												\
																													\
protected:																											\
	/* Sets what gpu contains the object. */																		\
	inline void set_##getter_method(contained_type& reference) noexcept												\
	{																												\
		_reference = &reference;																					\
	}																												\
}																													\

#endif // ! COMPWOLF_REFERENCE_CONTAINER_HEADER
