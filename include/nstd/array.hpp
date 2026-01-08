#ifndef NSTD_ARRAY_HPP
#define NSTD_ARRAY_HPP

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
        T &operator[](size_t index);
        const T &operator[](size_t index) const;
        T &at(size_t index);
        const T &at(size_t index) const;
        T &front();
        T &back();
        const T &front() const;
        const T &back() const;
        T *data();
        size_t size() const;
        size_t max_size() const;
        bool empty() const;
        T *begin();
        T *end();
        const T *begin() const;
        const T *end() const;
        const T *cbegin() const;
        const T *cend() const;
        T *rbegin();
        T *rend();
        const T *rbegin() const;
        const T *rend() const;
        const T *crbegin() const;
        const T *crend() const;
        void fill(const T &value);
        void swap(array &other);
        auto operator<=>(const array &other) const;
        bool operator==(const array &other) const;


        T _data[N];
    };

    // Definitions

    template <typename T, size_t N>
    T &array<T, N>::operator[](size_t index)
    {
        return _data[index];
    }

    template <typename T, size_t N>
    const T &array<T, N>::operator[](size_t index) const
    {
        return _data[index];
    }

    template <typename T, size_t N>
    T &array<T, N>::at(size_t index)
    {
        if (index >= N)
        {
            throw std::out_of_range("array index out of range");
        }
        return _data[index];
    }

    template <typename T, size_t N>
    const T &array<T, N>::at(size_t index) const
    {
        if (index >= N)
        {
            throw std::out_of_range("array index out of range");
        }
        return _data[index];
    }

    template <typename T, size_t N>
    T &array<T, N>::front()
    {
        return _data[0];
    }

    template <typename T, size_t N>
    T &array<T, N>::back()
    {
        return _data[N - 1];
    }

    template <typename T, size_t N>
    const T &array<T, N>::front() const
    {
        return _data[0];
    }

    template <typename T, size_t N>
    const T &array<T, N>::back() const
    {
        return _data[N - 1];
    }

    template <typename T, size_t N>
    T *array<T, N>::data()
    {
        return _data;
    }

    template <typename T, size_t N>
    size_t array<T, N>::size() const
    {
        return N;
    }

    template <typename T, size_t N>
    size_t array<T, N>::max_size() const
    {
        return N;
    }

    template <typename T, size_t N>
    bool array<T, N>::empty() const
    {
        return N == 0;
    }

    template <typename T, size_t N>
    T *array<T, N>::begin()
    {
        return &_data[0];
    }

    template <typename T, size_t N>
    T *array<T, N>::end()
    {
        return &_data[N];
    }

    template <typename T, size_t N>
    const T *array<T, N>::begin() const
    {
        return &_data[0];
    }

    template <typename T, size_t N>
    const T *array<T, N>::end() const
    {
        return &_data[N];
    }

    template <typename T, size_t N>
    const T *array<T, N>::cbegin() const
    {
        return &_data[0];
    }

    template <typename T, size_t N>
    const T *array<T, N>::cend() const
    {
        return &_data[N];
    }

    template <typename T, size_t N>
    T *array<T, N>::rbegin()
    {
        return end() - 1;
    }

    template <typename T, size_t N>
    T *array<T, N>::rend()
    {
        return begin() - 1;
    }

    template <typename T, size_t N>
    const T *array<T, N>::rbegin() const
    {
        return end() - 1;
    }

    template <typename T, size_t N>
    const T *array<T, N>::rend() const
    {
        return begin() - 1;
    }

    template <typename T, size_t N>
    const T *array<T, N>::crbegin() const
    {
        return end() - 1;
    }

    template <typename T, size_t N>
    const T *array<T, N>::crend() const
    {
        return begin() - 1;
    }

    template <typename T, size_t N>
    void array<T, N>::fill(const T &value)
    {
        for (size_t i{}; i < N; ++i)
        {
            _data[i] = value;
        }
    }

    template <typename T, size_t N>
    void array<T, N>::swap(array &other)
    {
        for (size_t i{}; i < N; ++i)
        {
            std::swap(_data[i], other._data[i]);
        }
    }

    template <typename T, size_t N>
    auto array<T, N>::operator<=>(const array &other) const
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
    }

    template <typename T, size_t N>
    bool array<T, N>::operator==(const array &other) const
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

} // namespace nstd

#endif // NSTD_ARRAY_HPP