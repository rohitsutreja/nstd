#ifndef NSTD_VECTOR_HPP
#define NSTD_VECTOR_HPP

#include <cassert>
#include <utility>
#include <stdexcept>
#include <type_traits>

namespace nstd
{
    template <typename T>
    class vector
    {
    public:
        vector();
        vector(const std::initializer_list<T> &list);
        vector(const vector &other);
        vector(vector &&other) noexcept;
        ~vector();
        vector &operator=(vector other);
        T &operator[](const size_t index);
        const T &operator[](const size_t index) const;
        T &at(size_t index);
        const T &at(size_t index) const;
        size_t size() const;
        bool is_empty() const;
        size_t get_capacity() const;
        void reserve(size_t new_capacity);
        void shrink_to_fit();
        void push_back(const T &element);
        void push_back(T &&element);
        void pop_back();
        T &front();
        T &back();
        const T &front() const;
        const T &back() const;
        void clear();
        T *begin() noexcept;
        T *end() noexcept;
        const T *begin() const noexcept;
        const T *end() const noexcept;
        const T *cbegin() const noexcept;
        const T *cend() const noexcept;
        T *data() noexcept;
        const T *data() const noexcept;

        friend void swap(vector &a, vector &b) noexcept
        {
            std::swap(a._data, b._data);
            std::swap(a._capacity, b._capacity);
            std::swap(a._length, b._length);
        }

    private:
        size_t _capacity{};
        size_t _length{};
        T *_data{};
    };

    // Definitions

    template <typename T>
    vector<T>::vector() : _data{nullptr}, _capacity{0}, _length{0} {}

    template <typename T>
    vector<T>::vector(const std::initializer_list<T> &list) : vector()
    {
        if (list.size() <= 0)
        {
            return;
        }
        _data = new T[list.size()];
        _capacity = list.size();

        try
        {
            size_t i{};
            for (auto begin{list.begin()}; begin != list.end(); begin++)
            {
                _data[i] = *begin;
                ++_length;
                ++i;
            }
        }
        catch (...)
        {
            delete[] _data;
            throw;
        }
    }

    template <typename T>
    vector<T>::vector(const vector &other) : vector()
    {
        if (other._length <= 0)
        {
            return;
        }

        _data = new T[other._length];
        _capacity = other._length;

        try
        {
            for (size_t i{}; i < other._length; ++i)
            {
                _data[i] = other._data[i];
                ++_length;
            }
        }
        catch (...)
        {
            delete[] _data;
            throw;
        }
    }

    template <typename T>
    vector<T>::vector(vector &&other) noexcept : _data{other._data},
                                                 _capacity{other._capacity},
                                                 _length{other._length}
    {
        other._data = nullptr;
        other._capacity = 0;
        other._length = 0;
    }

    template <typename T>
    vector<T>::~vector()
    {
        delete[] _data;
    }

    template <typename T>
    vector<T> &vector<T>::operator=(vector other)
    {
        swap(*this, other);
        return *this;
    }

    template <typename T>
    T &vector<T>::operator[](const size_t index)
    {
        assert(index < _length);
        return _data[index];
    }

    template <typename T>
    const T &vector<T>::operator[](const size_t index) const
    {
        assert(index < _length);
        return _data[index];
    }

    template <typename T>
    T &vector<T>::at(size_t index)
    {
        if (index >= _length)
            throw std::out_of_range("vector index out of range");
        return _data[index];
    }

    template <typename T>
    const T &vector<T>::at(size_t index) const
    {
        if (index >= _length)
            throw std::out_of_range("vector index out of range");
        return _data[index];
    }

    template <typename T>
    size_t vector<T>::size() const
    {
        return _length;
    }

    template <typename T>
    bool vector<T>::is_empty() const
    {
        return _length == 0;
    }

    template <typename T>
    size_t vector<T>::get_capacity() const
    {
        return _capacity;
    }

    template <typename T>
    void vector<T>::reserve(size_t new_capacity)
    {
        if (new_capacity <= _capacity)
        {
            return;
        }

        auto *new_memory{new T[new_capacity]};

        try
        {
            for (size_t i{}; i < _length; ++i)
            {
                // We only use move if is noexcept, because if move throws our vector will loose values already moved.
                if constexpr (std::is_nothrow_move_constructible_v<T>)
                {
                    new_memory[i] = std::move(_data[i]);
                }
                else
                {
                    new_memory[i] = _data[i];
                }
            }
        }
        catch (...)
        {
            delete[] new_memory;
            throw;
        }

        delete[] _data;

        _data = new_memory;
        _capacity = new_capacity;
    }

    template <typename T>
    void vector<T>::shrink_to_fit()
    {
        if (_length == 0)
        {
            delete[] _data;
            _data = nullptr;
            _capacity = 0;
            _length = 0;
            return;
        }

        if (_length < _capacity)
        {
            auto *new_memory{new T[_length]};

            try
            {
                for (size_t i{}; i < _length; ++i)
                {
                    // Use move only if noexcept to avoid partial moves on exceptions
                    if constexpr (std::is_nothrow_move_constructible_v<T>)
                    {
                        new_memory[i] = std::move(_data[i]);
                    }
                    else
                    {
                        new_memory[i] = _data[i];
                    }
                }
            }
            catch (...)
            {
                delete[] new_memory;
                throw;
            }

            delete[] _data;
            _data = new_memory;
            _capacity = _length;
        }
    }

    template <typename T>
    void vector<T>::push_back(const T &element)
    {
        if (_length == _capacity)
        {
            reserve(_capacity ? _capacity * 2 : 1);
        }

        _data[_length++] = element;
    }

    template <typename T>
    void vector<T>::push_back(T &&element)
    {
        if (_length == _capacity)
        {
            reserve(_capacity ? _capacity * 2 : 1);
        }

        _data[_length++] = std::move(element);
    }

    template <typename T>
    void vector<T>::pop_back()
    {
        assert(_length >= 1);
        --_length;
    }

    template <typename T>
    T &vector<T>::front()
    {
        assert(_length >= 1);
        return _data[0];
    }

    template <typename T>
    T &vector<T>::back()
    {
        assert(_length >= 1);
        return _data[_length - 1];
    }

    template <typename T>
    const T &vector<T>::front() const
    {
        assert(_length >= 1);
        return _data[0];
    }

    template <typename T>
    const T &vector<T>::back() const
    {
        assert(_length >= 1);
        return _data[_length - 1];
    }

    template <typename T>
    void vector<T>::clear()
    {
        _length = 0;
    }

    template <typename T>
    T *vector<T>::begin() noexcept { return _data; }

    template <typename T>
    T *vector<T>::end() noexcept { return _data + _length; }

    template <typename T>
    const T *vector<T>::begin() const noexcept { return _data; }

    template <typename T>
    const T *vector<T>::end() const noexcept { return _data + _length; }

    template <typename T>
    const T *vector<T>::cbegin() const noexcept { return _data; }

    template <typename T>
    const T *vector<T>::cend() const noexcept { return _data + _length; }

    template <typename T>
    T *vector<T>::data() noexcept { return _data; }

    template <typename T>
    const T *vector<T>::data() const noexcept { return _data; }

} // namespace nstd

#endif // NSTD_VECTOR_HPP