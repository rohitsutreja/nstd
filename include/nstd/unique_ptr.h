#include <utility> 
#include <cstddef>

namespace nstd {

	template<typename T>
	class unique_ptr {
	public:
		constexpr unique_ptr() noexcept = default;

		constexpr unique_ptr(std::nullptr_t) noexcept {}

		unique_ptr(const unique_ptr&) = delete;
		void operator=(const unique_ptr&) = delete;

		constexpr explicit unique_ptr(T* ptr) noexcept : _ptr{ ptr } {}

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