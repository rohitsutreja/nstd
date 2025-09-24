#include <cassert>

namespace nstd
{
    template <typename T>
    class vector
    {
    public:
        vector() : data{nullptr}, capacity{0}, length{0} {}

        void reserve(size_t new_capacity)
        {
            if (new_capacity <= capacity)
                return;

            auto *new_memory{new T[new_capacity]};
            for (size_t i{}; i < length; ++i)
            {
                new_memory[i] = std::move(data[i]);
            }

            delete[] data;

            data = new_memory;
            capacity = new_capacity;
        }

        void push_back(const T &element)
        {
            // if array is full, make some space
            if (length >= capacity)
            {
                _reallocate();
            }

            data[length++] = element;
        }

        void push_back(T &&element)
        {
            // if array is full, make some space
            if (length >= capacity)
            {
                _reallocate();
            }

            data[length++] = std::move(element);
        }

        T pop_back()
        {
            assert(length >= 1);

            auto element{std::move(data[--length])};
            data[length].~T();
            return element;
        }

        void clear()
        {
            for (size_t i{}; i < length; ++i)
            {
                data[i].~T();
            }
            length = 0;
        }

        T &operator[](const size_t index)
        {
            assert(index < length);
            return data[index];
        }

        const T &operator[](const size_t index) const
        {
            assert(index < length);
            return data[index];
        }

        size_t size() const
        {
            return length;
        }

        size_t get_capacity() const
        {
            return capacity;
        }

        ~vector()
        {
            delete[] data;
        }

    private:
        void _reallocate()
        {
            auto new_capacity{capacity ? capacity * 2 : 1};

            reserve(new_capacity);
        };

        T *data{};
        size_t capacity{};
        size_t length{};
    };
}