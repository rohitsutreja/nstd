#include <memory>
#include "nstd/unique_ptr.h"

namespace nstd {

	namespace detail {
		template<typename R, typename... Args>
		class Callable {
		public:
			virtual R invoke(Args...) = 0;
			virtual nstd::unique_ptr<Callable> clone() const = 0;
			virtual ~Callable() = default;
		};

		template<typename T, typename R, typename... Args>
		class CallableImpl : public Callable<R, Args...> {
		public:
			T _callable;

			template<typename U = T>
			CallableImpl(U&& callable) : _callable{ std::forward<U>(callable) } {}

			R invoke(Args... args) override {
				return _callable(std::forward<Args>(args)...);
			}

			nstd::unique_ptr<Callable<R, Args...>> clone() const override {
				return nstd::make_unique<CallableImpl>(_callable);
			}
		};
	}



	template<typename Signature>
	class function;

	template<typename R, typename ...Args>
	class function<R(Args...)> {
	public:
		function() = default;

		function(std::nullptr_t) : function() {}

		template<typename T>
		function(T&& callable) requires(!std::is_same_v<function, std::decay_t<T>>) {
			_callable = nstd::make_unique<detail::CallableImpl<std::decay_t<T>, R, Args...>>(std::forward<T>(callable));
		}

		function(const function& other) {
			if (other) {
				_callable = other._callable->clone();
			}
			else {
				_callable = nullptr;
			}
		};

		function(function&& other) noexcept : _callable{ std::move(other._callable) } {};

		function& operator=(const function& other) {
			if (this != &other) {
				_callable = other ? other._callable->clone() : nullptr;
			}
			return *this;
		};

		function& operator=(function&& other) noexcept {
			if (this != &other) {
				_callable = std::move(other._callable);
			}
			return *this;
		}

		explicit operator bool() const noexcept {
			return _callable;
		};

		R operator()(Args... args) const {
			if (!_callable) {
				throw std::bad_function_call();
			}
			return _callable->invoke(std::forward<Args>(args)...);
		}


		friend void swap(function& first, function& second) noexcept {
			std::swap(first._callable, second._callable);
		}

	private:
		nstd::unique_ptr<detail::Callable<R, Args...>> _callable{};
	};
}