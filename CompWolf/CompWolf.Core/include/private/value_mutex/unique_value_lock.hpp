#ifndef COMPWOLF_UNIQUE_VALUE_LOCK_HEADER
#define COMPWOLF_UNIQUE_VALUE_LOCK_HEADER

#include "value_mutex.hpp"
#include <stdexcept>

namespace CompWolf
{
	/* A unique_lock for value_mutex, which allows accessing its value while locked.
	 * @see value_mutex
	 * @see shared_value_lock
	 * @typeparam MutexType The type of value_mutex this can lock.
	 */
	template <typename MutexType>
	class unique_value_lock
	{
		// Using is_same_v to make the assertion a type-dependent expression.
		static_assert(std::is_same_v<MutexType, MutexType>, "Tried defining a unique_value_mutex for a type that is not a value_mutex");
	};

	template <typename ValueType, lockable InternalMutexType>
	class unique_value_lock<value_mutex<ValueType, InternalMutexType>>
	{
	public:
		using mutex_type = value_mutex<ValueType, InternalMutexType>;
		/* The type of value contained by the value_mutex. */
		using value_type = mutex_type::value_type;
		/* The type of mutex used by the value_mutex. */
		using internal_mutex_type = mutex_type::internal_mutex_type;

	private:
		/* The associated mutex, or none if the lock has no associated mutex. */
		mutex_type* mutex_object = nullptr;
		/* A pointer to the associated mutex' pointer, or nullptr if the lock is not currently locking the mutex. */
		value_type* value_object = nullptr;
	public:
		/* The mutex the lock is associated with, or nullptr if it has no associated mutex. */
		inline mutex_type* mutex() const noexcept
		{
			return mutex_object;
		}
		/* Whether the lock is currently locking the mutex. */
		inline bool owns_lock() const noexcept
		{
			return value_object != nullptr;
		}
		/* Whether the lock is currently locking the mutex. */
		inline explicit operator bool() const noexcept { return owns_lock(); }

	public:
		/* Locks the associated mutex.
		 * If the lock is already locking the mutex, returns immediately.
		 * @throws std::logic_error if there is no associated mutex.
		 */
		void lock()
		{
			if (mutex_object == nullptr) throw std::logic_error("Called unique_value_lock.lock() with no associated mutex to actually lock.");
			if (owns_lock()) return;

			value_object = &mutex_object->lock();
		}
		/* Locks the associated mutex if possible.
		 * If the lock is already locking the mutex, returns true immediately.
		 * If it could not lock the mutex, returns false.
		 * @throws std::logic_error if there is no associated mutex.
		 */
		bool try_lock()
		{
			if (mutex_object == nullptr) throw std::logic_error("Called unique_value_lock.try_lock() with no associated mutex to actually lock.");
			if (owns_lock()) return true;

			value_object = mutex_object->try_lock();

			return owns_lock();
		}
		/* Unlocks the associated mutex.
		 * @throws std::logic_error if there is no associated mutex, or it is not locked.
		 */
		void unlock()
		{
			if (mutex_object == nullptr) throw std::logic_error("Called unique_value_lock.unlock() with no associated mutex to actually unlock.");
			if (!owns_lock()) throw std::logic_error("Called unique_value_lock.unlock() while it does not actually lock the associated mutex.");

			mutex_object->unlock();
			value_object = nullptr;
		}

		/* Sets the lock to not have an associated mutex.
		 * If the lock was locking the mutex, lets the current thread take over locking the mutex.
		 * @return The mutex the lock was associated with, or nullptr if it already has no associated mutex.
		 */
		mutex_type* release() noexcept
		{
			auto output = mutex_object;
			mutex_object = nullptr;
			value_object = nullptr;
			return output;
		}

	public:
		/* Constructs a lock with no associated lock. */
		unique_value_lock() noexcept = default;
		/* Constructs a lock for the given mutex, but does not actually lock it yet. */
		unique_value_lock(mutex_type& mutex, std::defer_lock_t) : mutex_object(&mutex) {}
		/* Constructs a lock for the given mutex, and locks it if possible.
		 * If it cannot currently lock the mutex, does not lock it.
		 */
		unique_value_lock(mutex_type& mutex, std::try_to_lock_t) : mutex_object(&mutex)
		{
			try_lock();
		}
		/* Constructs a lock for the given mutex, and takes over locking it from the current thread.
		 * If the current thread is not locking it, is undefined behaviour.
		 * @param value Reference to the mutex' value; if not actually a reference to the mutex' value, has undefined behaviour.
		 */
		unique_value_lock(mutex_type& mutex, std::adopt_lock_t) : mutex_object(&mutex), value_object(&mutex.value()) {}
		/* Constructs a lock for the given mutex, and locks it.
		 * If it cannot currently lock the mutex, waits until it can.
		 */
		explicit unique_value_lock(mutex_type& mutex) : mutex_object(&mutex)
		{
			lock();
		}

		/* If the lock is locking an associated mutex, unlocks it. */
		~unique_value_lock()
		{
			if (owns_lock()) unlock();
		}

		unique_value_lock(const unique_value_lock&) = delete;
		auto operator=(const unique_value_lock&)->unique_value_lock & = delete;

	public:
		/* Returns the associated mutex' value.
		 * @throws std::logic_error if there is no associated mutex, or it is not locked.
		 */
		value_type& value() const noexcept
		{
			if (mutex_object == nullptr) throw std::logic_error("Called unique_value_lock.get_value() with no associated mutex to actually get the value of.");
			if (!owns_lock()) throw std::logic_error("Called unique_value_lock.get_value() while it does not actually lock the associated mutex.");

			return *value_object;
		}
	};
}

#endif // ! COMPWOLF_UNIQUE_VALUE_LOCK_HEADER
