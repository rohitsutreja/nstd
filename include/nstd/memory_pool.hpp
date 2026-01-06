
#include <new>       

namespace nstd {

	template<typename T, size_t BlocksPerChunk = 100>
	class memory_pool {
	private:
		union Block {
			T val;
			Block* next;
		};


	public:
		memory_pool() {
			_expand();
		}

		template<typename ...Args>
		T* allocate(Args&& ...args) {
			if (!_head) {
				_expand();
			}

			void* result{ _head };
			_head = *reinterpret_cast<void**>(_head);
			return new (result) T(std::forward<Args>(args)...);
		}

		void deallocate(T* ptr) {
			if (!ptr) {
				return;
			}

			ptr->~T();

			new (ptr) void* { _head };

			_head = ptr;
		}

		~memory_pool() {
			for (auto* chunk : _chunks) {
				::operator delete(chunk);
			}
		}

	private:

		void _expand() {
			auto* const raw_mem{ ::operator new(block_size * BlocksPerChunk) };
			_chunks.push_back(raw_mem);

			char* ptr = static_cast<char*>(raw_mem);

			for (int i{}; i < BlocksPerChunk - 1; ++i) {
				new (ptr + (i * block_size)) void* { ptr + (i + 1) * block_size };

				// OR

				// std::construct_at(
				// 	reinterpret_cast<void**>(ptr + (i * block_size)),
				// 	ptr + (i + 1) * block_size
				// );
			}


			new (ptr + (BlocksPerChunk - 1) * block_size) void* { _head };

			_head = raw_mem;
		}


		void* _head{};
		nstd::vector<void*> _chunks{};
		static constexpr size_t block_size{ std::max(sizeof(T), sizeof(void*)) };
	};
}