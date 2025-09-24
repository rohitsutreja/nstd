#include <cassert>

namespace nstd
{
    template <typename T>
    class vector
    {
    public:
        vector() : data{nullptr}, capacity{0}, length{0} {}

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

        T &operator[](const size_t index)
        {
            assert(index >= 0 || index < length);
            return data[index];
        }

        ~vector()
        {
            delete[] data;
        }

    private:
        void _reallocate()
        {
            auto old_capacity{capacity};
            auto new_capacity{old_capacity ? old_capacity * 2 : 1};

            auto new_memory{new T[new_capacity]};

            for (int i{}; i < length; ++i)
            {
                new_memory[i] = data[i];
            }

            delete[] data;

            data = new_memory;
            capacity = new_capacity;
        };

        T *data{};
        size_t capacity{};
        size_t length{};
    };
}