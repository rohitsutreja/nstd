#include <utility>
#include <type_traits>
#include <cstddef>

namespace nstd {

	template<typename T>
	class unique_ptr {
	public:
		constexpr unique_ptr() noexcept = default;
		constexpr unique_ptr(std::nullptr_t) noexcept {}
		constexpr explicit unique_ptr(T* ptr) noexcept : _ptr{ ptr } {}

		template<typename U>
		constexpr unique_ptr(unique_ptr<U>&& other) noexcept
			requires(std::is_convertible_v<U*, T*>)
		{
			_ptr = other.release();
		}


		template<typename U>
		unique_ptr& operator=(unique_ptr<U>&& other) noexcept
			requires(std::is_convertible_v<U*, T*>)
		{
			reset(other.release());
			return *this;
		}

		unique_ptr(const unique_ptr&) = delete;
		void operator=(const unique_ptr&) = delete;

		constexpr unique_ptr(unique_ptr&& other) noexcept : _ptr{ other._ptr } {
			other._ptr = nullptr;
		}

		constexpr unique_ptr& operator=(unique_ptr&& other) noexcept {
			if (this != &other) {
				delete _ptr;
				_ptr = other._ptr;
				other._ptr = nullptr;
			}
			return *this;
		}

		constexpr T* operator->() const noexcept {
			return _ptr;
		}

		constexpr T& operator*() const noexcept {
			return *_ptr;
		}

		constexpr void reset(T* new_ptr = nullptr) {
			delete _ptr;
			_ptr = new_ptr;
		}

		T* release() {
			T* temp = _ptr;
			_ptr = nullptr;
			return temp;
		}

		constexpr ~unique_ptr() {
			delete _ptr;
		}

	private:
		T* _ptr{};
	};

	template<typename T, typename ...Args>
	constexpr unique_ptr<T> make_unique(Args&& ...args)
	{
		return unique_ptr<T>{new T(std::forward<Args>(args)...)};
	}

}