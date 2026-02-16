#ifndef NSTD_UNIQUE_PTR_HPP
#define NSTD_UNIQUE_PTR_HPP

#include <utility>
#include <type_traits>
#include <cstddef>

namespace nstd {

	template<typename T>
	struct default_delete {

		constexpr default_delete() noexcept = default;

		template <typename U>
		constexpr default_delete(const default_delete<U>&) noexcept
			requires(std::is_convertible_v<U*, T*>) {}


		constexpr void operator()(T* ptr) const noexcept {
			delete ptr;
		}
	};

	template<typename T, typename Deleter = default_delete<T>>
	class unique_ptr {
	public:
		constexpr unique_ptr() noexcept = default;
		constexpr unique_ptr(std::nullptr_t) noexcept {}

		constexpr explicit unique_ptr(T* ptr) noexcept : _ptr{ ptr } {}

		constexpr unique_ptr(T* ptr, const Deleter& d) noexcept : _ptr{ ptr }, _deleter{ d } {}
		constexpr unique_ptr(T* ptr, Deleter&& d) noexcept : _ptr{ ptr }, _deleter{ std::move(d) } {}

		template<typename U>
		constexpr unique_ptr(unique_ptr<U>&& other) noexcept
			requires(std::is_convertible_v<U*, T*>) : _ptr{ other.release() }, _deleter{ std::move(other.get_deleter()) } {}

		template<typename U>
		unique_ptr& operator=(unique_ptr<U>&& other) noexcept
			requires(std::is_convertible_v<U*, T*>)
		{
			reset(other.release());
			_deleter = std::move(other.get_deleter());
			return *this;
		}

		unique_ptr(const unique_ptr&) = delete;
		unique_ptr& operator=(const unique_ptr&) = delete;

		constexpr unique_ptr(unique_ptr&& other) noexcept : _ptr{ std::exchange(other._ptr, nullptr) }, _deleter{ std::move(other._deleter) } {}

		constexpr unique_ptr& operator=(unique_ptr&& other) noexcept {
			if (this != &other) {
				reset(other.release());
				_deleter = std::move(other._deleter);
			}
			return *this;
		}
		constexpr T* operator->() const noexcept {
			return _ptr;
		}

		constexpr T& operator*() const noexcept {
			return *_ptr;
		}

		constexpr operator bool() const noexcept {
			return _ptr;
		}

		constexpr T* get() const noexcept {
			return _ptr;
		}


		constexpr Deleter& get_deleter() noexcept { return _deleter; }
		constexpr const Deleter& get_deleter() const noexcept { return _deleter; }


		constexpr void reset(T* new_ptr = nullptr) noexcept {
			auto* const old_ptr{ std::exchange(_ptr, new_ptr) };
			if (old_ptr != nullptr) {
				_deleter(old_ptr);
			}
		}

		constexpr T* release() noexcept {
			T* temp = _ptr;
			_ptr = nullptr;
			return temp;
		}

		constexpr ~unique_ptr() {
			if (_ptr != nullptr) {
				_deleter(_ptr);
			}
		}

	private:
		T* _ptr{};
		[[no_unique_address]] Deleter _deleter{};
	};

	template<typename T, typename ...Args>
	constexpr unique_ptr<T> make_unique(Args&& ...args)
	{
		return unique_ptr<T>{new T(std::forward<Args>(args)...)};
	}

}

#endif