#ifndef COMPWOLF_VALUE_MUTEX_HEADER
#define COMPWOLF_VALUE_MUTEX_HEADER

#include <mutex>
#include <shared_mutex>
#include <concepts>

namespace CompWolf
{
	/* Specifies that a type can be locked and unlocked when not already locked. */
	template <typename T>
	concept lockable = requires(T t)
	{
		/* Locks the object. If it is already locked, waits until it is unlocked. */
		t.lock();
		/* Locks the object if possible. Returns whether it was successfully locked. */
		{ t.try_lock() } -> std::convertible_to<bool>;
		/* If called by the thread that locked the object, unlocks it. */
		t.unlock();
	};
	/* Specifies that a type can be locked and unlocked in a ("shared") way where others may lock it the same way simultaneously, but not normally. */
	template <typename T>
	concept shared_lockable = lockable<T> and requires(T t)
	{
		/* Locks the object for shared ownership. If it is locked for non-shared ownership, waits until it is unlocked. */
		t.lock_shared();
		/* Tries to lock the object for shared ownership and returning true. If it is locked for non-shared ownership, fails and returns true. */
		{ t.try_lock_shared() } -> std::convertible_to<bool>;
		/* If called by a thread that locked the object for shared ownership, unlocks it.
		 * Note that the object may have additional locks, and it may therefore still be locked.
		 */
		t.unlock_shared();
	};

	/* Contains an object and an associated mutex.
	 * The object can only be retreived by locking the mutex.
	 * @typeparam ValueType The type of object contained by this.
	 * @typeparam MutexType The type of mutex used. If it fulfills shared_lockable, allows read-access through the shared lock.
	 */
	template <typename ValueType, lockable MutexType = std::mutex>
	class value_mutex
	{
	public:
		/* The type of value contained by the value_mutex. */
		using value_type = ValueType;
		/* The type of mutex used by the value_mutex. */
		using internal_mutex_type = MutexType;
	private:
		/* The value contained by the value_mutex. */
		value_type value_object;
		/* The mutex used by the value_mutex. */
		internal_mutex_type mutex;

	public:
		/* Constructs the value_mutex to contain a default-constructed object. */
		constexpr value_mutex() noexcept = default;

		/* Constructs the value_mutex with the given value. */
		template <typename ValueInputType>
			requires std::is_convertible_v<ValueInputType, value_type>
		constexpr value_mutex(ValueInputType value)
			noexcept(std::is_nothrow_convertible_v<ValueInputType, value_type>)
			: value_object(value), mutex() {}
		/* Sets the value the value_mutex contains.
		 * Is not thread safe if used while not having the value_mutex locked.
		 */
		template <typename ValueInputType>
			requires std::is_convertible_v<ValueInputType, value_type>
		constexpr value_mutex& operator=(ValueInputType value)
			noexcept(std::is_nothrow_convertible_v<ValueInputType, value_type>)
		{
			value_object = value;
			return *this;
		}

	public:
		/* Locks the value_mutex and returns a reference to the value it contains.
		 * If it is already locked, waits until it is unlocked.
		 */
		value_type& lock() noexcept(noexcept(std::declval<internal_mutex_type>().lock()))
		{
			mutex.lock();
			return value_object;
		}
		/* Tries to lock the value_mutex and returns a pointer to the value it contains.
		 * If it is already locked, fails and instead returns nullptr.
		 */
		value_type* try_lock() noexcept(noexcept(std::declval<internal_mutex_type>().try_lock()))
		{
			if (mutex.try_lock()) return &value_object;
			return nullptr;
		}
		/* Unlocks the value_mutex.
		 * Accessing the value after this (but before locking the value_mutex again) is not thread safe.
		 */
		void unlock() noexcept(noexcept(std::declval<internal_mutex_type>().unlock()))
		{
			mutex.unlock();
		}

		/* Returns a reference to the value the value_mutex contains.
		 * Is not thread safe if used while not having the value_mutex locked.
		 */
		constexpr value_type& value() noexcept
		{
			return value_object;
		}


		/* Locks the value_mutex for read-only access, and returns a const reference to the value it contains.
		 * If it is locked normally, waits until it is unlocked.
		 */
		const value_type& lock_shared() noexcept(noexcept(std::declval<internal_mutex_type>().lock_shared()))
			requires shared_lockable<internal_mutex_type>
		{
			mutex.lock_shared();
			return value_object;
		}
		/* Tries to lock the value_mutex for read-only access, and returns a pointer to the value it contains.
		 * If it is locked normally, fails and instead returns nullptr.
		 */
		value_type* try_lock_shared() noexcept(noexcept(std::declval<internal_mutex_type>().try_lock_shared()))
			requires shared_lockable<internal_mutex_type>
		{
			if (mutex.try_lock_shared()) return &value_object;
			return nullptr;
		}
		/* Unlocks the value_mutex from read-only access.
		 * Accessing the value after this (but before locking the value_mutex again) is not thread safe.
		 * Note that the object may have additional read-only access locks, and it may therefore still be locked.
		 */
		void unlock_shared() noexcept(noexcept(std::declval<internal_mutex_type>().unlock_shared()))
			requires shared_lockable<internal_mutex_type>
		{
			mutex.unlock_shared();
		}

		/* Returns a const reference to the value the value_mutex contains.
		 * Is not thread safe if used while not having the value_mutex locked or locked for read-only access.
		 */
		constexpr const value_type& value_shared() noexcept
			requires shared_lockable<internal_mutex_type>
		{
			return value_object;
		}

		/* Locks the value_mutex, gets the value, and then unlocks it.
		 * Only locks for read-only access if possible.
		 * @return A copy of the value that was gotten.
		 */
		value_type get_value_copy_quick()
			noexcept(shared_lockable<internal_mutex_type>
				? (noexcept(std::declval<internal_mutex_type>().lock_shared()) && noexcept(std::declval<internal_mutex_type>().unlock_shared()))
				: (noexcept(std::declval<internal_mutex_type>().lock()) && noexcept(std::declval<internal_mutex_type>().unlock()))
				)
		{
			if constexpr (shared_lockable<internal_mutex_type>)
			{
				lock_shared();
				auto result = value_object;
				unlock_shared();
				return result;
			}
			else
			{
				lock();
				auto result = value_object;
				unlock();
				return result;
			}
		}
	};

	/* Contains an object and an associated shared_mutex.
	 * The object can only be retreived by locking the mutex.
	 * @typeparam ValueType The type of object contained by this.
	 */
	template <typename ValueType>
	using shared_value_mutex = value_mutex<ValueType, std::shared_mutex>;
}

#endif // ! COMPWOLF_VALUE_MUTEX_HEADER
