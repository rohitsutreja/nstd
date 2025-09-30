#ifndef NSTD_STRING_HPP
#define NSTD_STRING_HPP

#include <cstddef>
#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <stdexcept>
#include <limits>
#include <iosfwd>
#include <cassert>

namespace nstd
{
    template <typename CharT>
    class basic_string
    {
    public:
        basic_string();
        template <size_t N>
        basic_string(const CharT (&str)[N]);
        basic_string(const CharT *str);
        basic_string(const basic_string &other);
        basic_string(basic_string &&other) noexcept;
        basic_string &operator=(basic_string other);
        basic_string &operator=(const CharT *str);
        size_t size() const noexcept;
        size_t length() const noexcept;
        const CharT *data() const noexcept;
        const CharT *c_str() const noexcept;
        CharT &operator[](size_t pos);
        const CharT &operator[](size_t pos) const;
        CharT &at(size_t pos);
        const CharT &at(size_t pos) const;
        bool empty() const noexcept;
        void push_back(CharT ch);
        void pop_back();
        basic_string &append(const CharT *str);
        basic_string &operator+=(CharT ch);
        basic_string &operator+=(const CharT *str);
        basic_string &operator+=(const basic_string &other);
        void clear();
        size_t capacity() const noexcept;
        void reserve(size_t new_cap);
        ~basic_string();

        friend void swap(basic_string &lhs, basic_string &rhs) noexcept
        {
            std::swap(lhs._data, rhs._data);
            std::swap(lhs._length, rhs._length);
            std::swap(lhs._capacity, rhs._capacity);
        }

        CharT *begin() noexcept;
        const CharT *begin() const noexcept;
        const CharT *cbegin() const noexcept;
        CharT *end() noexcept;
        const CharT *end() const noexcept;
        const CharT *cend() const noexcept;

        friend std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const basic_string<CharT> &str)
        {
            return os << str.c_str();
        }

        friend bool operator==(const basic_string &first, const basic_string &second)
        {
            if (first._length != second._length)
            {
                return false;
            }
            return std::equal(first.begin(), first.end(), second.begin());
        }

    private:
        size_t _capacity{};
        size_t _length{};
        CharT *_data{};
    };

    // Definitions

    template <typename CharT>
    basic_string<CharT>::basic_string() : _capacity{1}, _length{}, _data{new CharT[_capacity]{CharT{}}} {}

    template <typename CharT>
    template <size_t N>
    basic_string<CharT>::basic_string(const CharT (&str)[N])
    {
        static_assert(N > 0, "Array size must be greater than 0");

        size_t len = N;

        if (N > 0 && str[N - 1] == CharT{})
        {
            len = N - 1;
        }

        auto *temp{new CharT[len + 1]};

        try
        {
            std::copy(str, str + len, temp);
            temp[len] = CharT{};
        }
        catch (...)
        {
            delete[] temp;
            throw;
        }

        _data = temp;
        _length = len;
        _capacity = len + 1;
    }

    template <typename CharT>
    basic_string<CharT>::basic_string(const CharT *str)
    {
        assert(str);

        size_t len{};

        while (str[len])
        {
            ++len;
        }

        auto *temp{new CharT[len + 1]};

        try
        {
            std::copy(str, str + len + 1, temp);
        }
        catch (...)
        {
            delete[] temp;
            throw;
        }

        _capacity = len + 1;
        _length = len;
        _data = temp;
    }

    template <typename CharT>
    basic_string<CharT>::basic_string(const basic_string &other)
    {
        auto *temp{new CharT[other._length + 1]};

        try
        {
            std::copy(other._data, other._data + other._length + 1, temp);
        }
        catch (...)
        {
            delete[] temp;
            throw;
        }

        _data = temp;
        _length = other._length;
        _capacity = other._length + 1;
    }

    template <typename CharT>
    basic_string<CharT>::basic_string(basic_string &&other) noexcept
    {
        _data = other._data;
        _capacity = other._capacity;
        _length = other._length;

        other._data = new CharT[1]{CharT{}};
        other._capacity = 1;
        other._length = 0;
    }

    template <typename CharT>
    basic_string<CharT> &basic_string<CharT>::operator=(basic_string other)
    {
        swap(*this, other);
        return *this;
    }

    template <typename CharT>
    basic_string<CharT> &basic_string<CharT>::operator=(const CharT *str)
    {
        assert(str != nullptr);

        size_t len{};
        while (str[len])
        {
            ++len;
        }

        auto *temp{new CharT[len + 1]};
        try
        {
            std::copy(str, str + len + 1, temp);
        }
        catch (...)
        {
            delete[] temp;
            throw;
        }
        delete[] _data;
        _data = temp;
        _length = len;
        _capacity = len + 1;
        return *this;
    }

    template <typename CharT>
    size_t basic_string<CharT>::size() const noexcept
    {
        return _length;
    }

    template <typename CharT>
    size_t basic_string<CharT>::length() const noexcept
    {
        return _length;
    }

    template <typename CharT>
    const CharT *basic_string<CharT>::data() const noexcept
    {
        return _data;
    }

    template <typename CharT>
    const CharT *basic_string<CharT>::c_str() const noexcept
    {
        return data();
    }

    template <typename CharT>
    CharT &basic_string<CharT>::operator[](size_t pos)
    {
        return _data[pos];
    }

    template <typename CharT>
    const CharT &basic_string<CharT>::operator[](size_t pos) const
    {
        return _data[pos];
    }

    template <typename CharT>
    CharT &basic_string<CharT>::at(size_t pos)
    {
        if (pos >= _length)
        {
            throw std::out_of_range("basic_string::at: index out of range");
        }

        return _data[pos];
    }

    template <typename CharT>
    const CharT &basic_string<CharT>::at(size_t pos) const
    {
        if (pos >= _length)
        {
            throw std::out_of_range("basic_string::at: index out of range");
        }

        return _data[pos];
    }

    template <typename CharT>
    bool basic_string<CharT>::empty() const noexcept
    {
        return _length == 0;
    }

    template <typename CharT>
    void basic_string<CharT>::push_back(CharT ch)
    {
        if (_length + 1 >= _capacity)
        {
            reserve(std::max(_capacity * 2, _length + 2));
        }

        _data[_length] = ch;
        _data[_length + 1] = CharT{};
        ++_length;
    }

    template <typename CharT>
    void basic_string<CharT>::pop_back()
    {
        if (_length > 0)
        {
            --_length;
            _data[_length] = CharT{};
        }
    }

    template <typename CharT>
    basic_string<CharT> &basic_string<CharT>::append(const CharT *str)
    {
        assert(str != nullptr);

        size_t len{};

        while (str[len])
        {
            ++len;
        }

        if (_length + len >= _capacity)
        {
            reserve(std::max(_capacity * 2, _length + len + 1));
        }

        std::copy(str, str + len + 1, _data + _length);
        _length += len;

        return *this;
    }

    template <typename CharT>
    basic_string<CharT> &basic_string<CharT>::operator+=(CharT ch)
    {
        push_back(ch);
        return *this;
    }

    template <typename CharT>
    basic_string<CharT> &basic_string<CharT>::operator+=(const CharT *str)
    {
        return append(str);
    }

    template <typename CharT>
    basic_string<CharT> &basic_string<CharT>::operator+=(const basic_string &other)
    {
        return append(other.c_str());
    }

    template <typename CharT>
    void basic_string<CharT>::clear()
    {
        _length = 0;
        _data[0] = CharT{};
    }

    template <typename CharT>
    size_t basic_string<CharT>::capacity() const noexcept
    {
        return _capacity;
    }

    template <typename CharT>
    void basic_string<CharT>::reserve(size_t new_cap)
    {
        if (new_cap <= _capacity)
        {
            return;
        }
        auto *temp{new CharT[new_cap]};

        try
        {
            std::copy(_data, _data + _length + 1, temp);
        }
        catch (...)
        {
            delete[] temp;
            throw;
        }

        delete[] _data;
        _data = temp;
        _capacity = new_cap;
    }

    template <typename CharT>
    basic_string<CharT>::~basic_string()
    {
        delete[] _data;
    }

    template <typename CharT>
    CharT *basic_string<CharT>::begin() noexcept { return _data; }

    template <typename CharT>
    const CharT *basic_string<CharT>::begin() const noexcept { return _data; }

    template <typename CharT>
    const CharT *basic_string<CharT>::cbegin() const noexcept { return _data; }

    template <typename CharT>
    CharT *basic_string<CharT>::end() noexcept { return _data + _length; }

    template <typename CharT>
    const CharT *basic_string<CharT>::end() const noexcept { return _data + _length; }

    template <typename CharT>
    const CharT *basic_string<CharT>::cend() const noexcept { return _data + _length; }

    using string = basic_string<char>;

} // namespace nstd

#endif // NSTD_STRING_HPP
