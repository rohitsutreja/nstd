#ifndef NSTD_SHARED_PTR_HPP
#define NSTD_SHARED_PTR_HPP

#include <cstddef> 

namespace nstd {

	template<typename T>
	class shared_ptr {
	public:
		constexpr shared_ptr() noexcept : _ptr{ nullptr }, _ref_count{ nullptr } {}

		constexpr shared_ptr(std::nullptr_t) noexcept : _ptr{ nullptr }, _ref_count{ nullptr } {}

		constexpr explicit shared_ptr(T* ptr)
			: _ptr{ ptr }, _ref_count{ new size_t{1} } {
		}

		constexpr shared_ptr(const shared_ptr& other) noexcept
			: _ptr{ other._ptr }, _ref_count{ other._ref_count } {
			if (_ref_count) {
				++(*_ref_count);
			}
		}

		constexpr shared_ptr(shared_ptr&& other) noexcept
			: _ptr{ other._ptr }, _ref_count{ other._ref_count } {
			other._ptr = nullptr;
			other._ref_count = nullptr;
		}

		constexpr shared_ptr& operator=(const shared_ptr& other) {
			if (this != &other) {
				{
					_release_resource();
				}

				_ptr = other._ptr;
				_ref_count = other._ref_count;

				if (_ref_count) {
					++(*_ref_count);
				}
			}
			return *this;
		}

		constexpr shared_ptr& operator=(shared_ptr&& other) noexcept {
			if (this != &other)
			{
				_release_resource();
			}

			_ptr = other._ptr;
			_ref_count = other._ref_count;

			other._ptr = nullptr;
			other._ref_count = nullptr;

			return *this;
		}

		constexpr ~shared_ptr() {
			_release_resource();
		}

		constexpr T* operator->() const noexcept { return _ptr; }
		constexpr T& operator*() const noexcept { return *_ptr; }
		constexpr size_t use_count() const noexcept { return _ref_count ? *_ref_count : 0; }

	private:
		T* _ptr{};
		size_t* _ref_count{};

		constexpr void _release_resource() {
			if (!_ref_count)
			{
				return;
			}

			--(*_ref_count);
			if (*_ref_count == 0) {
				delete _ptr;
				delete _ref_count;
			}

			_ptr = nullptr;
			_ref_count = nullptr;
		}
	};

	template<typename T, typename ...Args>
	constexpr shared_ptr<T> make_shared(Args&& ...args) {
		return shared_ptr<T>{new T(std::forward<Args>(args)...)};
	}
};

#endif