#include <cstddef>
#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <stdexcept>
#include <limits>
#include <iosfwd>
#include "vector.hpp"

namespace nstd
{
    template <typename CharT>
    class basic_string
    {
    public:
        basic_string() : _data{}, _length{} {};

        basic_string(const CharT *str)
        {
            if (!str)
            {
                return;
            }

            size_t len{};
            while (str[len])
            {
                ++len;
            }

            auto *temp{new CharT[len]};

            try
            {
                std::copy(str, str + len, temp);
            }
            catch (...)
            {
                delete[] temp;
                throw;
            }

            _data = temp;
            _length = len;
        }

        basic_string(const basic_string &other) : basic_string()
        {
            if (other._length == 0)
            {
                return;
            }

            auto *temp{new CharT[other._length]};

            try
            {
                std::copy(other._data, other._data + other._length, temp);
            }
            catch (...)
            {
                delete[] temp;
                throw;
            }

            _data = temp;
            _length = other._length;
        }

        basic_string(basic_string &&other) : basic_string() noexcept
        {
            _data = other._data;
            _length = other._length;

            other._data = nullptr;
            other._length = 0;
        }

        basic_string &operator=(basic_string other)
        {
            swap(*this, other);
            return *this;
        }

        basic_string &operator=(const CharT *str)
        {
            if (!str || str[0] == charT{})
            {
                delete[] _data;
                _data = nullptr;
                _length = 0;
                return *this;
            }

            size_t len{};
            while (str[len])
            {
                ++len;
            }
            auto *temp{new CharT[len]};
            try
            {
                std::copy(str, str + len, temp);
            }
            catch (...)
            {
                delete[] temp;
                throw;
            }
            delete[] _data;
            _data = temp;
            _length = len;
            return *this;
        }

        ~basic_string()
        {
            delete[] _data;
        }

        friend void swap(basic_string &lhs, basic_string &rhs) noexcept
        {
            std::swap(lhs._data, rhs._data);
            std::swap(lhs._length, rhs._length);
        }

    private:
        CharT *_data{};
        size_t _length{};
    };
} // namespace nstd
