#ifndef NSTD_MEMORY_POOL
#define NSTD_MEMORY_POOL

#include <algorithm>
#include <new>
#include <vector>

namespace nstd {
template<typename T, size_t BlocksPerChunk = 100> class memory_pool {
public:
    memory_pool() {
        _expand();
    }

    template<typename... Args> T* allocate(Args&&... args) {
        if (!_head) {
            _expand();
        }
        void* result{_head};
        _head = *reinterpret_cast<void**>(_head);
        try {
            return new (result) T(std::forward<Args>(args)...);
        } catch (...) {
            *reinterpret_cast<void**>(result) = _head;
            _head = result;
            throw;
        }
    }

    void deallocate(T* ptr) {
        if (!ptr) {
            return;
        }
        ptr->~T();
        *reinterpret_cast<void**>(ptr) = _head;
        _head = ptr;
    }

    ~memory_pool() {
        for (auto* chunk : _chunks) {
            ::operator delete(chunk);
        }
    }

    memory_pool(const memory_pool&) = delete;
    memory_pool& operator=(const memory_pool&) = delete;

private:
    void _expand() {
        auto* const raw_mem{::operator new(block_size* BlocksPerChunk)};
        _chunks.push_back(raw_mem);

        char* ptr = static_cast<char*>(raw_mem);

        for (size_t i{}; i < BlocksPerChunk - 1; ++i) {
            *reinterpret_cast<void**>(ptr + (i * block_size)) = ptr + ((i + 1) * block_size);
        }

        *reinterpret_cast<void**>(ptr + ((BlocksPerChunk - 1) * block_size)) = _head;
        _head = raw_mem;
    }

    void* _head{};
    nstd::vector<void*> _chunks{};

    static constexpr size_t block_size{(std::max(sizeof(T), sizeof(void*)))};
};
} // namespace nstd

#endif