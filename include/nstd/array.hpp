#include <cstddef>
#include <algorithm>
#include <tuple>
#include <type_traits>
namespace nstd
{

    template <typename T, size_t N>
    class array
    {
    public:
        T &operator[](size_t index)
        {
            return _data[index];
        }

        const T &operator[](size_t index) const
        {
            return _data[index];
        }

        T &at(size_t index)
        {
            if (index >= N)
            {
                throw std::out_of_range("array index out of range");
            }
            return _data[index];
        }

        const T &at(size_t index) const
        {
            if (index >= N)
            {
                throw std::out_of_range("array index out of range");
            }
            return _data[index];
        }

        T &front()
        {
            return _data[0];
        }

        T &back()
        {
            return _data[N - 1];
        }

        const T &front() const
        {
            return _data[0];
        }

        const T &back() const
        {
            return _data[N - 1];
        }

        T *data()
        {
            return _data;
        }

        size_t size() const
        {
            return N;
        }

        size_t max_size() const
        {
            return N;
        }

        bool empty() const
        {
            return N == 0;
        }

        T *begin()
        {
            return &_data[0];
        }

        T *end()
        {
            return &_data[N];
        }

        const T *begin() const
        {
            return &_data[0];
        }

        const T *end() const
        {
            return &_data[N];
        }

        const T *cbegin() const
        {
            return &_data[0];
        }

        const T *cend() const
        {
            return &_data[N];
        }

        T *rbegin()
        {
            return end() - 1;
        }

        T *rend()
        {
            return begin() - 1;
        }

        const T *rbegin() const
        {
            return end() - 1;
        }

        const T *rend() const
        {
            return begin() - 1;
        }

        const T *crbegin() const
        {
            return end() - 1;
        }

        const T *crend() const
        {
            return begin() - 1;
        }

        void fill(const T &value)
        {
            for (size_t i{}; i < N; ++i)
            {
                _data[i] = value;
            }
        }

        void swap(array &other)
        {
            for (size_t i{}; i < N; ++i)
            {
                std::swap(_data[i], other._data[i]);
            }
        }

        bool operator<=>(const array &other)
        {
            for (size_t i = 0; i < N; ++i)
            {
                if (_data[i] < other._data[i])
                {
                    return std::strong_ordering::less;
                }
                if (_data[i] > other._data[i])
                {
                    return std::strong_ordering::greater;
                }
            }
            return std::strong_ordering::equal;
        };

        bool operator==(const array &other) const
        {
            for (size_t i{}; i < N; ++i)
            {
                if (_data[i] != other._data[i])
                {
                    return false;
                }
            }
            return true;
        }

    private:
        T _data[N];
    };
}