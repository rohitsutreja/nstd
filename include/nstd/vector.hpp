#include <cassert>

namespace nstd
{
    template <typename T>
    class vector
    {
    public:
        // Default constructor
        vector() : data{nullptr},
                   capacity{0},
                   length{0} {}

        vector(const vector &other) : data{new T[other.capacity]},
                                      capacity{other.capacity},
                                      length{other.length}
        {
            for (size_t i{}; i < length; ++i)
            {
                data[i] = other.data[i];
            }
        }

        vector(vector &&other) : data{other.data},
                                 capacity{other.capacity},
                                 length{other.length}
        {
            other.data = nullptr;
            other.capacity = 0;
            other.length = 0;
        }

        ~vector()
        {
            clear();
            delete[] data;
        }

        vector &operator=(vector other)
        {
            swap(*this, other);
            return *this;
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

        bool is_empty() const
        {
            return length == 0;
        }

        size_t get_capacity() const
        {
            return capacity;
        }

        void reserve(size_t new_capacity)
        {
            if (new_capacity <= capacity)
            {
                return;
            }

            auto *new_memory{new T[new_capacity]};
            for (size_t i{}; i < length; ++i)
            {
                new_memory[i] = std::move(data[i]);
            }

            delete[] data;

            data = new_memory;
            capacity = new_capacity;
        }

        void shrink_to_fit()
        {
            if (length < capacity)
            {
                auto *new_memory{new T[length]};
                for (size_t i{}; i < length; ++i)
                {
                    new_memory[i] = std::move(data[i]);
                }

                delete[] data;
                data = new_memory;
                capacity = length;
            }
        }

        void push_back(const T &element)
        {
            if (length >= capacity)
            {
                _reallocate();
            }

            data[length++] = element;
        }

        void push_back(T &&element)
        {
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

        // Iterators
        T *begin()
        {
            return data;
        }

        T *end()
        {
            return data + length;
        }

        const T *begin() const
        {
            return data;
        }

        const T *end() const
        {
            return data + length;
        }

        // Friend functions
        friend void swap(vector &a, vector &b) noexcept
        {
            std::swap(a.data, b.data);
            std::swap(a.capacity, b.capacity);
            std::swap(a.length, b.length);
        }

    private:
        void _reallocate()
        {
            auto new_capacity{capacity ? capacity * 2 : 1};
            reserve(new_capacity);
        }

        T *data{};
        size_t capacity{};
        size_t length{};
    };
}