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
        basic_string() : _capacity{1}, _length{}, _data{new CharT[_capacity]{CharT{}}} {};

        template <size_t N>
        basic_string(const CharT (&str)[N])
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

        basic_string(const CharT *str)
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

        basic_string(const basic_string &other)
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

        basic_string(basic_string &&other) noexcept
        {
            _data = other._data;
            _capacity = other._capacity;
            _length = other._length;

            other._data = new CharT[1]{CharT{}};
            other._capacity = 1;
            other._length = 0;
        }

        basic_string &operator=(basic_string other)
        {
            swap(*this, other);
            return *this;
        }

        basic_string &operator=(const CharT *str)
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

        size_t size() const noexcept
        {
            return _length;
        }

        size_t length() const noexcept
        {
            return _length;
        }

        const CharT *data() const noexcept
        {
            return _data;
        }

        const CharT *c_str() const noexcept
        {
            return data();
        }

        CharT &operator[](size_t pos)
        {
            return _data[pos];
        }

        const CharT &operator[](size_t pos) const
        {
            return _data[pos];
        }

        CharT &at(size_t pos)
        {
            if (pos >= _length)
            {
                throw std::out_of_range("basic_string::at: index out of range");
            }

            return _data[pos];
        }

        const CharT &at(size_t pos) const
        {
            if (pos >= _length)
            {
                throw std::out_of_range("basic_string::at: index out of range");
            }

            return _data[pos];
        }

        bool empty() const noexcept
        {
            return _length == 0;
        }

        void push_back(CharT ch)
        {
            if (_length + 1 >= _capacity)
            {
                reserve(std::max(_capacity * 2, _length + 2));
            }

            _data[_length] = ch;
            _data[_length + 1] = CharT{};
            ++_length;
        }

        void pop_back()
        {
            if (_length > 0)
            {
                --_length;
                _data[_length] = CharT{};
            }
        }

        basic_string &append(const CharT *str)
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

        basic_string &operator+=(CharT ch)
        {
            push_back(ch);
            return *this;
        }

        basic_string &operator+=(const CharT *str)
        {
            return append(str);
        }

        basic_string &operator+=(const basic_string &other)
        {
            return append(other.c_str());
        }

        void clear()
        {
            _length = 0;
            _data[0] = CharT{};
        }

        size_t capacity() const noexcept
        {
            return _capacity;
        }

        void reserve(size_t new_cap)
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

        ~basic_string()
        {
            delete[] _data;
        }

        friend void swap(basic_string &lhs, basic_string &rhs) noexcept
        {
            std::swap(lhs._data, rhs._data);
            std::swap(lhs._length, rhs._length);
            std::swap(lhs._capacity, rhs._capacity);
        }

        CharT *begin() noexcept { return _data; }
        const CharT *begin() const noexcept { return _data; }
        const CharT *cbegin() const noexcept { return _data; }

        CharT *end() noexcept { return _data + _length; }
        const CharT *end() const noexcept { return _data + _length; }
        const CharT *cend() const noexcept { return _data + _length; }

        friend std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const basic_string<CharT> &str)
        {
            return os << str.c_str();
        }

    private:
        size_t _capacity{};
        size_t _length{};
        CharT *_data{};
    };

    using string = basic_string<char>;

} // namespace nstd
