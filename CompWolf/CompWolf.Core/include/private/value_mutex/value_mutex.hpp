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
		value_type _value;
		/* The mutex used by the value_mutex. */
		mutable internal_mutex_type _mutex;

	public:
		/* Constructs the value_mutex to contain a default-constructed object. */
		constexpr value_mutex() noexcept : _value(), _mutex() {}

		/* Constructs the value_mutex with the given value. */
		template <typename ValueInputType>
			requires std::is_convertible_v<ValueInputType, value_type>
		constexpr value_mutex(ValueInputType value)
			noexcept(std::is_nothrow_convertible_v<ValueInputType, value_type>)
			: _value(value), _mutex() {}
		/* Sets the value the value_mutex contains.
		 * Is not thread safe if used while not having the value_mutex locked.
		 */
		template <typename ValueInputType>
			requires std::is_convertible_v<ValueInputType, value_type>
		constexpr value_mutex& operator=(ValueInputType value)
			noexcept(std::is_nothrow_convertible_v<ValueInputType, value_type>)
		{
			_value = value;
			return *this;
		}

	public:
		/* Locks the value_mutex and returns a reference to the value it contains.
		 * If it is already locked, waits until it is unlocked.
		 */
		value_type& lock() noexcept(noexcept(std::declval<internal_mutex_type>().lock()))
		{
			_mutex.lock();
			return _value;
		}
		/* Locks the value_mutex and returns a reference to the value it contains.
		 * If it is already locked, waits until it is unlocked.
		 */
		const value_type& lock() const noexcept(noexcept(std::declval<internal_mutex_type>().lock()))
		{
			_mutex.lock();
			return _value;
		}

		/* Tries to lock the value_mutex and returns a pointer to the value it contains.
		 * If it is already locked, fails and instead returns nullptr.
		 */
		value_type* try_lock() noexcept(noexcept(std::declval<internal_mutex_type>().try_lock()))
		{
			if (_mutex.try_lock()) return &_value;
			return nullptr;
		}
		/* Tries to lock the value_mutex and returns a pointer to the value it contains.
		 * If it is already locked, fails and instead returns nullptr.
		 */
		const value_type* try_lock() const noexcept(noexcept(std::declval<internal_mutex_type>().try_lock()))
		{
			if (_mutex.try_lock()) return &_value;
			return nullptr;
		}

		/* Unlocks the value_mutex.
		 * Accessing the value after this (but before locking the value_mutex again) is not thread safe.
		 */
		void unlock() const noexcept(noexcept(std::declval<internal_mutex_type>().unlock()))
		{
			_mutex.unlock();
		}

		/* Returns a reference to the value the value_mutex contains.
		 * Is not thread safe if used while not having the value_mutex locked.
		 */
		constexpr auto value() noexcept -> value_type&
		{
			return _value;
		}
		/* Returns a reference to the value the value_mutex contains.
		 * Is not thread safe if used while not having the value_mutex locked.
		 */
		constexpr auto value() const noexcept -> const value_type&
		{
			return _value;
		}


		/* Locks the value_mutex for read-only access, and returns a const reference to the value it contains.
		 * If it is locked normally, waits until it is unlocked.
		 */
		const value_type& lock_shared() const
			noexcept(noexcept(std::declval<internal_mutex_type>().lock_shared()))
			requires shared_lockable<internal_mutex_type>
		{
			_mutex.lock_shared();
			return _value;
		}
		/* Tries to lock the value_mutex for read-only access, and returns a pointer to the value it contains.
		 * If it is locked normally, fails and instead returns nullptr.
		 */
		value_type* try_lock_shared() const
			noexcept(noexcept(std::declval<internal_mutex_type>().try_lock_shared()))
			requires shared_lockable<internal_mutex_type>
		{
			if (_mutex.try_lock_shared()) return &_value;
			return nullptr;
		}
		/* Unlocks the value_mutex from read-only access.
		 * Accessing the value after this (but before locking the value_mutex again) is not thread safe.
		 * Note that the object may have additional read-only access locks, and it may therefore still be locked.
		 */
		void unlock_shared() const
			noexcept(noexcept(std::declval<internal_mutex_type>().unlock_shared()))
			requires shared_lockable<internal_mutex_type>
		{
			_mutex.unlock_shared();
		}

		/* Returns a const reference to the value the value_mutex contains.
		 * Is not thread safe if used while not having the value_mutex locked or locked for read-only access.
		 */
		constexpr const value_type& value_shared() const noexcept
			requires shared_lockable<internal_mutex_type>
		{
			return _value;
		}

		/* Locks the value_mutex, gets the value, and then unlocks it.
		 * Only locks for read-only access if possible.
		 * @return A copy of the value that was gotten.
		 */
		value_type get_value_copy_quick() const
			noexcept(shared_lockable<internal_mutex_type>
				? (noexcept(std::declval<internal_mutex_type>().lock_shared()) && noexcept(std::declval<internal_mutex_type>().unlock_shared()))
				: (noexcept(std::declval<internal_mutex_type>().lock()) && noexcept(std::declval<internal_mutex_type>().unlock()))
				)
		{
			if constexpr (shared_lockable<internal_mutex_type>)
			{
				lock_shared();
				auto result = _value;
				unlock_shared();
				return result;
			}
			else
			{
				lock();
				auto result = _value;
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
