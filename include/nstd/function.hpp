#include <memory>
#include "nstd/unique_ptr.h"

namespace nstd {

	namespace detail {
		template<typename R, typename... Args>
		class Callable {
		public:
			virtual R invoke(Args...) const = 0;
			virtual nstd::unique_ptr<Callable> clone() const = 0;
			virtual ~Callable() = default;
		};

		template<typename T, typename R, typename... Args>
		class CallableImpl : public Callable<R, Args...> {
		public:
			T _callable;

			CallableImpl(T&& callable) : _callable{ std::move(callable) } {}

			R invoke(Args... args) const override  {
				return _callable(args...);
			}

			nstd::unique_ptr<Callable<R, Args...>> clone() const override {
				return nstd::make_unique<CallableImpl>(T{ _callable });
			}
		};
	}



	template<typename Signature>
	class function;

	template<typename R, typename ...Args>
	class function<R(Args...)> {
	public:
		template<typename T>
		function(T&& callable) requires(!std::is_same_v<function, std::decay_t<T>>) {
			_callable = nstd::make_unique<detail::CallableImpl<std::decay_t<T>, R, Args...>>(std::forward<T>(callable));
		}

		function(const function& other) {
			if (other._callable) {
				_callable = other._callable->clone();
			}
			else {
				_callable = nullptr;
			}
		};

		function(function&& other) noexcept : _callable{ std::move(other._callable) } {};

		explicit operator bool() const noexcept {
			return _callable;
		};

		R operator()(Args... args) {
			return _callable->invoke(args...);
		}

	private:
		nstd::unique_ptr<detail::Callable<R, Args...>> _callable{};
	};
}