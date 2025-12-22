#ifndef NSTD_VECTOR_HPP
#define NSTD_VECTOR_HPP

#include <cassert>
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace nstd
{
    template <typename T>
    class vector
    {
    public:
        // --- Constructors & Destructor ---
        vector();
        vector(const std::initializer_list<T> list);
        vector(const vector& other);
        vector(vector&& other) noexcept;
        ~vector();

        // --- Assignment ---
        vector& operator=(vector other);

        // --- Iterators ---
        T* begin() noexcept;
        const T* begin() const noexcept;
        const T* cbegin() const noexcept;

        T* end() noexcept;
        const T* end() const noexcept;
        const T* cend() const noexcept;

        // --- Capacity ---
        size_t size() const;
        size_t get_capacity() const;
        bool is_empty() const;
        void reserve(size_t new_capacity);
        void shrink_to_fit();

        // --- Element Access ---
        T& operator[](const size_t index);
        const T& operator[](const size_t index) const;

        T& at(size_t index);
        const T& at(size_t index) const;

        T& front();
        const T& front() const;

        T& back();
        const T& back() const;

        T* data() noexcept;
        const T* data() const noexcept;

        // --- Modifiers ---
        void push_back(const T& element);
        void push_back(T&& element);

        template <typename ...Args>
        T& emplace_back(Args&& ...args);

        void pop_back();
        void clear();

        // --- Friends ---
        friend void swap(vector& a, vector& b) noexcept
        {
            std::swap(a._data, b._data);
            std::swap(a._capacity, b._capacity);
            std::swap(a._length, b._length);
        }

    private:
        // --- Helpers ---
        void _reallocate(size_t size);

        // --- Member Data ---
        size_t _capacity{};
        size_t _length{};
        T* _data{};
    };

    // ==========================================
    // Implementation
    // ==========================================

    // --- Constructors & Destructor ---

    template <typename T>
    vector<T>::vector()
        : _capacity{ 0 }, _length{ 0 }, _data{ nullptr }
    {
    }

    template <typename T>
    vector<T>::vector(const std::initializer_list<T> list)
        : vector()
    {
        if (list.size() == 0)
        {
            return;
        }

        _capacity = list.size();
        _data = static_cast<T*>(::operator new(sizeof(T) * _capacity));

        try
        {
            for (const auto& item : list)
            {
                std::construct_at(_data + _length, item);
                ++_length;
            }
        }
        catch (...)
        {
            std::destroy(_data, _data + _length);
            ::operator delete(_data);
            throw;
        }
    }

    template <typename T>
    vector<T>::vector(const vector& other)
        : vector()
    {
        if (other._length <= 0)
        {
            return;
        }

        _capacity = other._capacity;
        _data = static_cast<T*>(operator new(sizeof(T) * _capacity));

        try
        {
            for (size_t i{}; i < other._length; ++i)
            {
                std::construct_at(_data + i, other._data[i]);
                ++_length;
            }
        }
        catch (...)
        {
            std::destroy(_data, _data + _length);
            ::operator delete(_data);
            throw;
        }
    }

    template <typename T>
    vector<T>::vector(vector&& other) noexcept
        : _data{ other._data },
        _capacity{ other._capacity },
        _length{ other._length }
    {
        other._data = nullptr;
        other._capacity = 0;
        other._length = 0;
    }

    template <typename T>
    vector<T>::~vector()
    {
        std::destroy(_data, _data + _length);
        ::operator delete(_data);
    }

    // --- Assignment ---

    template <typename T>
    vector<T>& vector<T>::operator=(vector other)
    {
        swap(*this, other);
        return *this;
    }

    // --- Iterators ---

    template <typename T>
    T* vector<T>::begin() noexcept { return _data; }

    template <typename T>
    const T* vector<T>::begin() const noexcept { return _data; }

    template <typename T>
    const T* vector<T>::cbegin() const noexcept { return _data; }

    template <typename T>
    T* vector<T>::end() noexcept { return _data + _length; }

    template <typename T>
    const T* vector<T>::end() const noexcept { return _data + _length; }

    template <typename T>
    const T* vector<T>::cend() const noexcept { return _data + _length; }

    // --- Capacity ---

    template <typename T>
    size_t vector<T>::size() const
    {
        return _length;
    }

    template <typename T>
    size_t vector<T>::get_capacity() const
    {
        return _capacity;
    }

    template <typename T>
    bool vector<T>::is_empty() const
    {
        return _length == 0;
    }

    template <typename T>
    void vector<T>::reserve(size_t new_cap)
    {
        if (new_cap <= _capacity)
        {
            return;
        }

        _reallocate(new_cap);
    }

    template <typename T>
    void vector<T>::shrink_to_fit()
    {
        if (_length == 0)
        {
            std::destroy(_data, _data + _length);
            ::operator delete(_data);

            _data = nullptr;
            _capacity = 0;
            _length = 0;
            return;
        }

        if (_length < _capacity)
        {
            _reallocate(_length);
        }
    }

    // --- Element Access ---

    template <typename T>
    T& vector<T>::operator[](const size_t index)
    {
        assert(index < _length);
        return _data[index];
    }

    template <typename T>
    const T& vector<T>::operator[](const size_t index) const
    {
        assert(index < _length);
        return _data[index];
    }

    template <typename T>
    T& vector<T>::at(size_t index)
    {
        if (index >= _length)
            throw std::out_of_range("vector index out of range");
        return _data[index];
    }

    template <typename T>
    const T& vector<T>::at(size_t index) const
    {
        if (index >= _length)
            throw std::out_of_range("vector index out of range");
        return _data[index];
    }

    template <typename T>
    T& vector<T>::front()
    {
        assert(_length >= 1);
        return _data[0];
    }

    template <typename T>
    const T& vector<T>::front() const
    {
        assert(_length >= 1);
        return _data[0];
    }

    template <typename T>
    T& vector<T>::back()
    {
        assert(_length >= 1);
        return _data[_length - 1];
    }

    template <typename T>
    const T& vector<T>::back() const
    {
        assert(_length >= 1);
        return _data[_length - 1];
    }

    template <typename T>
    T* vector<T>::data() noexcept { return _data; }

    template <typename T>
    const T* vector<T>::data() const noexcept { return _data; }

    // --- Modifiers ---

    template <typename T>
    void vector<T>::push_back(const T& element)
    {
        if (_length == _capacity)
        {
            reserve(_capacity ? _capacity * 2 : 1);
        }

        std::construct_at(_data + _length, element);
        ++_length;
    }

    template <typename T>
    void vector<T>::push_back(T&& element)
    {
        if (_length == _capacity)
        {
            if (&element >= _data && &element < _data + _length)
            {
                auto index{ &element - _data };
                reserve(_capacity ? _capacity * 2 : 1);
                std::construct_at(_data + _length, _data[index]);
            }
            else
            {
                reserve(_capacity ? _capacity * 2 : 1);
                std::construct_at(_data + _length, std::move(element));
            }
        }
        else
        {
            std::construct_at(_data + _length, std::move(element));
        }

        ++_length;
    }

    template<typename T>
    template<typename ...Args>
    T& vector<T>::emplace_back(Args&& ...args)
    {
        if (_length == _capacity)
        {
            reserve(_capacity ? _capacity * 2 : 1);
        }
        std::construct_at(_data + _length, std::forward<Args>(args)...);
        return _data[_length++];
    }

    template <typename T>
    void vector<T>::pop_back()
    {
        assert(_length >= 1);
        std::destroy_at(_data + _length - 1);
        --_length;
    }

    template <typename T>
    void vector<T>::clear()
    {
        std::destroy(_data, _data + _length);
        _length = 0;
    }

    // --- Private Helpers ---

    template<typename T>
    void vector<T>::_reallocate(size_t size)
    {
        auto* new_mem{ static_cast<T*>(::operator new(size * sizeof(T))) };
        size_t i{};

        try
        {
            for (; i < _length; ++i)
            {
                std::construct_at(new_mem + i, std::move_if_noexcept(_data[i]));
            }
        }
        catch (...)
        {
            std::destroy(new_mem, new_mem + i);
            ::operator delete(new_mem);
            throw;
        }

        std::destroy(_data, _data + _length);
        ::operator delete(_data);

        _data = new_mem;
        _capacity = size;
    }

} // namespace nstd

#endif // NSTD_VECTOR_HPP