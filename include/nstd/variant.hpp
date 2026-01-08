#ifndef NSTD_VARIANT
#define NSTD_VARIANT


#include <new>
#include <utility>
#include <cstddef>
#include "nstd/array.hpp"

namespace nstd {

	namespace detail {
		template<typename Target, typename... List>
		struct type_index {
			static_assert(!std::is_same_v<Target, Target>,
				"ERROR: Type not found in nstd::variant type list!");
		};

		template<typename Target, typename ...List>
		struct type_index<Target, Target, List...> {
			static constexpr size_t value = 0;
		};

		template<typename Target, typename First, typename ...List>
		struct type_index<Target, First, List...> {
			static constexpr size_t value = 1 + type_index<Target, List...>::value;
		};


		// FOR LEARNING PURPOSE, We can not use it in our destructor because _index is not constexpr.
		template<size_t CurrentIndex, size_t TargetIndex, typename ...List>
		struct index_type;

		template<size_t TargetIndex, typename Head, typename... Rest>
		struct index_type<TargetIndex, TargetIndex, Head, Rest...> {
			using type = Head;
		};

		template<size_t CurrentIndex, size_t TargetIndex, typename Head, typename... Rest>
		struct index_type<CurrentIndex, TargetIndex, Head, Rest...> {
			using type = typename index_type<CurrentIndex + 1, TargetIndex, Rest...>::type;
		};

		template<size_t Index, typename... Types>
		using type_at = typename index_type<0, Index, Types...>::type;

	}

	template<typename ...Types>
	class variant {
	public:
		variant() {
			using TypeToConstruct = detail::type_at<0, Types...>;

			new (_buffer) TypeToConstruct{};
		}

		template<typename T>
		variant(T&& value) requires (!std::is_same_v<std::decay_t<T>, variant>) {
			using DecayedT = std::decay_t<T>;

			_index = detail::type_index<DecayedT, Types...>::value;

			new (_buffer) DecayedT{ std::forward<T>(value) };
		}

		variant(const variant& other) : _index(other._index) {
			if (_index == -1) {
				return;
			}

			_copiers[_index](other._buffer, _buffer);
		}

		variant(variant&& other) : _index(other._index) {
			if (_index == -1) {
				return;
			}

			_movers[_index](other._buffer, _buffer);
		}

		variant& operator=(const variant& other) {
			if (this == &other) {
				return *this;
			}

			if (_index != -1) {
				_destroyers[_index](_buffer);
				_index = -1;
			}

			if (other._index == -1) {
				return *this;
			}

			_copiers[other._index](other._buffer, _buffer);
			_index = other._index;

			return *this;
		}

		variant& operator=(variant&& other) {
			if (this == &other) {
				return *this;
			}

			if (_index != -1) {
				_destroyers[_index](_buffer);
				_index = -1;
			}

			if (other._index == -1) {
				return *this;
			}

			_movers[other._index](other._buffer, _buffer);
			_index = other._index;

			return *this;
		}


		template<typename T>
		T& get() {
			using DecayedT = std::decay_t<T>;
			constexpr auto target_index{ detail::type_index<DecayedT, Types...>::value };

			if (_index != target_index) {
				throw std::runtime_error("bad_variant_access");
			}

			return *reinterpret_cast<DecayedT*>(_buffer);
		}

		template<typename Visitor>
		void visit(Visitor&& visitor) {
			if (_index == -1) {
				throw std::runtime_error("Bad variant access");
			}

			static constexpr nstd::array<void(*)(void*, Visitor&&), sizeof...(Types)> table{
				(
					+[](void* buffer, Visitor&& vis) {
						vis(*reinterpret_cast<Types*>(buffer));
					}
				)...
			};


			 table[_index](_buffer, std::forward<Visitor>(visitor));
		}

		template<typename T>
		const T& get() const {
			using DecayedT = std::decay_t<T>;
			constexpr size_t target_index{ detail::type_index<DecayedT, Types...>::value };

			if (_index != target_index) {
				throw std::runtime_error("bad_variant_access");
			}

			return *reinterpret_cast<const DecayedT*>(_buffer);
		}

		size_t index() {
			return _index;
		}

		~variant() {
			if (_index != -1) {
				_destroyers[_index](_buffer);
			}
		}

	private:
		template<typename T>
		static void _destroyer(void* ptr) {
			reinterpret_cast<T*>(ptr)->~T();
		}

		static constexpr nstd::array<void(*)(void*), sizeof...(Types)> _destroyers{ _destroyer<Types>... };

		static constexpr nstd::array<void(*)(const void*, void*), sizeof...(Types)> _copiers{
			(
				+[](const void* src, void* dst) {
					new (dst) Types(*reinterpret_cast<const Types*>(src));
				}
			)...
		};

		static constexpr nstd::array<void(*)(void*, void*), sizeof...(Types)> _movers{
			(
				+[](void* src, void* dst) {
					new (dst) Types(std::move(*reinterpret_cast<Types*>(src)));
				}
			)...
		};


		static constexpr size_t SIZE{ std::max({sizeof(Types)...}) };
		static constexpr size_t DATA_ALIGN{ std::max({alignof(Types)...}) };

		alignas(DATA_ALIGN) std::byte _buffer[SIZE];
		size_t _index = -1;
	};
}

#endif