#ifndef NSTD_EXPECTED_HPP
#define NSTD_EXPECTED_HPP

#include <utility>
#include <cassert>
#include <compare>
#include <type_traits>

namespace nstd
{
    // ==========================================
    //            Forward Declarations
    // ==========================================

    template <typename E>
    struct unexpected;

    template <typename T, typename E>
    class expected;

    // ==========================================
    //           Class Definitions (API)
    // ==========================================

    template <typename E>
    struct unexpected
    {
    public:
        constexpr explicit unexpected(const E &val);
        constexpr explicit unexpected(E &&val) noexcept(std::is_nothrow_move_constructible_v<E>);

        // Accessors
        constexpr const E &value() const & noexcept;
        constexpr E &value() & noexcept;
        constexpr E &&value() && noexcept;
        constexpr const E &&value() const && noexcept;

    private:
        E _val;
    };

    template <typename T, typename E>
    class expected
    {
    public:
        // Constructors
        constexpr expected()
            requires std::is_default_constructible_v<T>;

        constexpr expected(const T &val);
        constexpr expected(T &&val) noexcept(std::is_nothrow_move_constructible_v<T>);

        constexpr expected(const unexpected<E> &err);
        constexpr expected(unexpected<E> &&err) noexcept(std::is_nothrow_move_constructible_v<E>);

        constexpr expected(const expected &other);
        constexpr expected(expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>);

        template <typename U = T>
            requires(!std::is_same_v<expected, std::remove_cvref_t<U>>) &&
                        (!std::is_same_v<unexpected<E>, std::remove_cvref_t<U>>) &&
                        (std::is_constructible_v<T, U>)

        constexpr explicit(!std::is_convertible_v<U, T>) expected(U &&val) : _value{std::forward<U>(val)}, _hasValue{true}
        {
        }

        // Destructor
        constexpr ~expected();

        // Assignment Operators
        constexpr expected &operator=(const expected &other);
        constexpr expected &operator=(expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>);
        constexpr expected &operator=(const T &rhs);
        constexpr expected &operator=(T &&rhs);
        constexpr expected &operator=(const unexpected<E> &rhs);
        constexpr expected &operator=(unexpected<E> &&rhs);

        // Observers
        constexpr bool has_value() const noexcept;
        constexpr explicit operator bool() const noexcept;

        constexpr T &operator*() & noexcept;
        constexpr const T &operator*() const & noexcept;
        constexpr T &&operator*() && noexcept;
        constexpr const T &&operator*() const && noexcept;

        constexpr T *operator->() noexcept;
        constexpr const T *operator->() const noexcept;

        constexpr T &value() &;
        constexpr const T &value() const &;
        constexpr T &&value() &&;
        constexpr const T &&value() const &&;

        constexpr E &error() &;
        constexpr const E &error() const &;
        constexpr E &&error() &&;
        constexpr const E &&error() const &&;

        // Swap
        constexpr void swap(expected &other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>);

    private:
        union
        {
            T _value;
            E _error;
        };

        bool _hasValue;
    };

    // ==========================================
    //      VOID SPECIALIZATION DECLARATION
    // ==========================================

    template <typename E>
    class expected<void, E>
    {
    public:
        // Constructors
        constexpr expected() noexcept;
        constexpr expected(const unexpected<E> &err);
        constexpr expected(unexpected<E> &&err) noexcept(std::is_nothrow_move_constructible_v<E>);
        constexpr expected(const expected &other);
        constexpr expected(expected &&other) noexcept(std::is_nothrow_move_constructible_v<E>);

        // Destructor
        constexpr ~expected();

        // Assignment Operators
        constexpr expected &operator=(const expected &other);
        constexpr expected &operator=(expected &&other) noexcept(std::is_nothrow_move_constructible_v<E>);
        constexpr expected &operator=(const unexpected<E> &rhs);
        constexpr expected &operator=(unexpected<E> &&rhs);

        // Observers
        constexpr bool has_value() const noexcept;
        constexpr explicit operator bool() const noexcept;

        constexpr void operator*() const noexcept;
        constexpr void value() const;

        constexpr E &error() &;
        constexpr const E &error() const &;
        constexpr E &&error() &&;
        constexpr const E &&error() const &&;

        // Swap
        constexpr void swap(expected &other) noexcept(std::is_nothrow_move_constructible_v<E>);

    private:
        union
        {
            char _dummy; // Placeholder for Success
            E _error;
        };
        bool _hasValue;
    };

    // ==========================================
    //              IMPLEMENTATION
    // ==========================================

    // --- unexpected implementation ---

    template <typename E>
    constexpr unexpected<E>::unexpected(const E &val) : _val{val} {}

    template <typename E>
    constexpr unexpected<E>::unexpected(E &&val) noexcept(std::is_nothrow_move_constructible_v<E>)
        : _val{std::move(val)} {}

    template <typename E>
    constexpr const E &unexpected<E>::value() const & noexcept { return _val; }

    template <typename E>
    constexpr E &unexpected<E>::value() & noexcept { return _val; }

    template <typename E>
    constexpr E &&unexpected<E>::value() && noexcept { return std::move(_val); }

    template <typename E>
    constexpr const E &&unexpected<E>::value() const && noexcept { return std::move(_val); }

    // --- expected<T, E> implementation ---

    template <typename T, typename E>
    constexpr expected<T, E>::expected()
        requires std::is_default_constructible_v<T>
        : _value{}, _hasValue{true}
    {
    }

    template <typename T, typename E>
    constexpr expected<T, E>::expected(const T &val) : _value{val}, _hasValue{true} {}

    template <typename T, typename E>
    constexpr expected<T, E>::expected(T &&val) noexcept(std::is_nothrow_move_constructible_v<T>)
        : _value{std::move(val)}, _hasValue{true} {}

    template <typename T, typename E>
    constexpr expected<T, E>::expected(const unexpected<E> &err)
        : _error{err.value()}, _hasValue{false} {}

    template <typename T, typename E>
    constexpr expected<T, E>::expected(unexpected<E> &&err) noexcept(std::is_nothrow_move_constructible_v<E>)
        : _error{std::move(err).value()}, _hasValue{false} {}

    template <typename T, typename E>
    constexpr expected<T, E>::expected(const expected &other) : _hasValue{other._hasValue}
    {
        if (_hasValue)
        {
            std::construct_at(&_value, other._value);
        }
        else
        {
            std::construct_at(&_error, other._error);
        }
    }

    template <typename T, typename E>
    constexpr expected<T, E>::expected(expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>)
        : _hasValue{other._hasValue}
    {
        if (_hasValue)
        {
            std::construct_at(&_value, std::move(other._value));
        }
        else
        {
            std::construct_at(&_error, std::move(other._error));
        }
    }

    template <typename T, typename E>
    constexpr expected<T, E>::~expected()
    {
        if (_hasValue)
        {
            std::destroy_at(&_value);
        }
        else
        {
            std::destroy_at(&_error);
        }
    }

    // Assignment Operators (expected<T>)
    template <typename T, typename E>
    constexpr expected<T, E> &expected<T, E>::operator=(const expected &other)
    {
        if (this == &other)
            return *this;

        if (other._hasValue && _hasValue)
        {
            _value = other._value;
        }
        else if (!other._hasValue && !_hasValue)
        {
            _error = other._error;
        }
        else if (_hasValue && !other._hasValue)
        {
            std::destroy_at(&_value);
            _hasValue = false;
            std::construct_at(&_error, other._error);
        }
        else
        {
            std::destroy_at(&_error);
            _hasValue = true;
            std::construct_at(&_value, other._value);
        }
        return *this;
    }

    template <typename T, typename E>
    constexpr expected<T, E> &expected<T, E>::operator=(expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>)
    {
        if (this == &other)
            return *this;

        if (_hasValue && other._hasValue)
        {
            _value = std::move(other._value);
        }
        else if (!_hasValue && !other._hasValue)
        {
            _error = std::move(other._error);
        }
        else if (_hasValue && !other._hasValue)
        {
            std::destroy_at(&_value);
            _hasValue = false;
            std::construct_at(&_error, std::move(other._error));
        }
        else
        {
            std::destroy_at(&_error);
            _hasValue = true;
            std::construct_at(&_value, std::move(other._value));
        }
        return *this;
    }

    template <typename T, typename E>
    constexpr expected<T, E> &expected<T, E>::operator=(const T &rhs)
    {
        if (_hasValue)
        {
            _value = rhs;
        }
        else
        {
            std::destroy_at(&_error);
            _hasValue = true;
            std::construct_at(&_value, rhs);
        }
        return *this;
    }

    template <typename T, typename E>
    constexpr expected<T, E> &expected<T, E>::operator=(T &&rhs)
    {
        if (_hasValue)
        {
            _value = std::move(rhs);
        }
        else
        {
            std::destroy_at(&_error);
            _hasValue = true;
            std::construct_at(&_value, std::move(rhs));
        }
        return *this;
    }

    template <typename T, typename E>
    constexpr expected<T, E> &expected<T, E>::operator=(const unexpected<E> &rhs)
    {
        if (!_hasValue)
        {
            _error = rhs.value();
        }
        else
        {
            std::destroy_at(&_value);
            _hasValue = false;
            std::construct_at(&_error, rhs.value());
        }
        return *this;
    }

    template <typename T, typename E>
    constexpr expected<T, E> &expected<T, E>::operator=(unexpected<E> &&rhs)
    {
        if (!_hasValue)
        {
            _error = std::move(rhs).value();
        }
        else
        {
            std::destroy_at(&_value);
            _hasValue = false;
            std::construct_at(&_error, std::move(rhs).value());
        }
        return *this;
    }

    // Observers (expected<T>)
    template <typename T, typename E>
    constexpr bool expected<T, E>::has_value() const noexcept { return _hasValue; }

    template <typename T, typename E>
    constexpr expected<T, E>::operator bool() const noexcept { return _hasValue; }

    template <typename T, typename E>
    constexpr T &expected<T, E>::operator*() & noexcept
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }
    template <typename T, typename E>
    constexpr const T &expected<T, E>::operator*() const & noexcept
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }
    template <typename T, typename E>
    constexpr T &&expected<T, E>::operator*() && noexcept
    {
        assert(_hasValue && "Bad expected access");
        return std::move(_value);
    }
    template <typename T, typename E>
    constexpr const T &&expected<T, E>::operator*() const && noexcept
    {
        assert(_hasValue && "Bad expected access");
        return std::move(_value);
    }

    template <typename T, typename E>
    constexpr T *expected<T, E>::operator->() noexcept
    {
        assert(_hasValue && "Bad expected access");
        return &_value;
    }
    template <typename T, typename E>
    constexpr const T *expected<T, E>::operator->() const noexcept
    {
        assert(_hasValue && "Bad expected access");
        return &_value;
    }

    template <typename T, typename E>
    constexpr T &expected<T, E>::value() &
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }
    template <typename T, typename E>
    constexpr const T &expected<T, E>::value() const &
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }
    template <typename T, typename E>
    constexpr T &&expected<T, E>::value() &&
    {
        assert(_hasValue && "Bad expected access");
        return std::move(_value);
    }
    template <typename T, typename E>
    constexpr const T &&expected<T, E>::value() const &&
    {
        assert(_hasValue && "Bad expected access");
        return std::move(_value);
    }

    template <typename T, typename E>
    constexpr E &expected<T, E>::error() &
    {
        assert(!_hasValue && "Bad error access");
        return _error;
    }
    template <typename T, typename E>
    constexpr const E &expected<T, E>::error() const &
    {
        assert(!_hasValue && "Bad error access");
        return _error;
    }
    template <typename T, typename E>
    constexpr E &&expected<T, E>::error() &&
    {
        assert(!_hasValue && "Bad error access");
        return std::move(_error);
    }
    template <typename T, typename E>
    constexpr const E &&expected<T, E>::error() const &&
    {
        assert(!_hasValue && "Bad error access");
        return std::move(_error);
    }

    template <typename T, typename E>
    constexpr void expected<T, E>::swap(expected &other) noexcept(
        std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>)
    {
        if (this == &other)
            return;

        if (_hasValue && other._hasValue)
        {
            using std::swap;
            swap(_value, other._value);
        }
        else if (!_hasValue && !other._hasValue)
        {
            using std::swap;
            swap(_error, other._error);
        }
        else if (_hasValue && !other._hasValue)
        {
            auto temp{std::move(_value)};
            std::destroy_at(&_value);
            std::construct_at(&_error, std::move(other._error));
            _hasValue = false;
            std::destroy_at(&other._error);
            std::construct_at(&other._value, std::move(temp));
            other._hasValue = true;
        }
        else
        {
            other.swap(*this);
        }
    }

    // --- expected<void, E> Implementation ---

    template <typename E>
    constexpr expected<void, E>::expected() noexcept : _hasValue{true} {}

    template <typename E>
    constexpr expected<void, E>::expected(const unexpected<E> &err)
        : _error{err.value()}, _hasValue{false} {}

    template <typename E>
    constexpr expected<void, E>::expected(unexpected<E> &&err) noexcept(std::is_nothrow_move_constructible_v<E>)
        : _error{std::move(err).value()}, _hasValue{false} {}

    template <typename E>
    constexpr expected<void, E>::expected(const expected &other) : _hasValue{other._hasValue}
    {
        if (!_hasValue)
        {
            std::construct_at(&_error, other._error);
        }
    }

    template <typename E>
    constexpr expected<void, E>::expected(expected &&other) noexcept(std::is_nothrow_move_constructible_v<E>)
        : _hasValue{other._hasValue}
    {
        if (!_hasValue)
        {
            std::construct_at(&_error, std::move(other._error));
        }
    }

    template <typename E>
    constexpr expected<void, E>::~expected()
    {
        if (!_hasValue)
        {
            std::destroy_at(&_error);
        }
    }

    template <typename E>
    constexpr expected<void, E> &expected<void, E>::operator=(const expected &other)
    {
        if (this == &other)
            return *this;

        if (_hasValue && other._hasValue)
        {
            // Both success
        }
        else if (!_hasValue && !other._hasValue)
        {
            _error = other._error;
        }
        else if (_hasValue && !other._hasValue)
        {
            _hasValue = false;
            std::construct_at(&_error, other._error);
        }
        else
        {
            std::destroy_at(&_error);
            _hasValue = true;
        }
        return *this;
    }

    template <typename E>
    constexpr expected<void, E> &expected<void, E>::operator=(expected &&other) noexcept(std::is_nothrow_move_constructible_v<E>)
    {
        if (this == &other)
            return *this;

        if (_hasValue && other._hasValue)
        {
            // Both success
        }
        else if (!_hasValue && !other._hasValue)
        {
            _error = std::move(other._error);
        }
        else if (_hasValue && !other._hasValue)
        {
            _hasValue = false;
            std::construct_at(&_error, std::move(other._error));
        }
        else
        {
            std::destroy_at(&_error);
            _hasValue = true;
        }
        return *this;
    }

    template <typename E>
    constexpr expected<void, E> &expected<void, E>::operator=(const unexpected<E> &rhs)
    {
        if (!_hasValue)
        {
            _error = rhs.value();
        }
        else
        {
            _hasValue = false;
            std::construct_at(&_error, rhs.value());
        }
        return *this;
    }

    template <typename E>
    constexpr expected<void, E> &expected<void, E>::operator=(unexpected<E> &&rhs)
    {
        if (!_hasValue)
        {
            _error = std::move(rhs).value();
        }
        else
        {
            _hasValue = false;
            std::construct_at(&_error, std::move(rhs).value());
        }
        return *this;
    }

    template <typename E>
    constexpr bool expected<void, E>::has_value() const noexcept { return _hasValue; }

    template <typename E>
    constexpr expected<void, E>::operator bool() const noexcept { return _hasValue; }

    template <typename E>
    constexpr void expected<void, E>::operator*() const noexcept
    {
        assert(_hasValue && "Bad expected access");
    }

    template <typename E>
    constexpr void expected<void, E>::value() const
    {
        assert(_hasValue && "Bad expected access");
    }

    template <typename E>
    constexpr E &expected<void, E>::error() &
    {
        assert(!_hasValue);
        return _error;
    }

    template <typename E>
    constexpr const E &expected<void, E>::error() const &
    {
        assert(!_hasValue);
        return _error;
    }

    template <typename E>
    constexpr E &&expected<void, E>::error() &&
    {
        assert(!_hasValue);
        return std::move(_error);
    }

    template <typename E>
    constexpr const E &&expected<void, E>::error() const &&
    {
        assert(!_hasValue);
        return std::move(_error);
    }

    template <typename E>
    constexpr void expected<void, E>::swap(expected &other) noexcept(std::is_nothrow_move_constructible_v<E>)
    {
        if (this == &other)
            return;

        if (_hasValue && other._hasValue)
        {
            // Both success
        }
        else if (!_hasValue && !other._hasValue)
        {
            using std::swap;
            swap(_error, other._error);
        }
        else if (_hasValue && !other._hasValue)
        {
            std::construct_at(&_error, std::move(other._error));
            _hasValue = false;
            std::destroy_at(&other._error);
            other._hasValue = true;
        }
        else
        {
            other.swap(*this);
        }
    }

    // ==========================================
    //            GLOBAL OPERATORS
    // ==========================================

    // --- expected<T, E> Comparisons ---
    template <typename T, typename E>
    constexpr bool operator==(const expected<T, E> &lhs, const expected<T, E> &rhs)
    {
        if (lhs.has_value() != rhs.has_value())
            return false;
        if (lhs.has_value())
            return lhs.value() == rhs.value();
        return lhs.error() == rhs.error();
    }

    template <typename T, typename E>
    constexpr bool operator==(const expected<T, E> &lhs, const T &rhs)
    {
        return lhs.has_value() && lhs.value() == rhs;
    }

    template <typename T, typename E>
    constexpr bool operator==(const expected<T, E> &lhs, const unexpected<E> &rhs)
    {
        return !lhs.has_value() && lhs.error() == rhs.value();
    }

    // --- expected<void, E> Comparisons ---
    template <typename E>
    constexpr bool operator==(const expected<void, E> &lhs, const expected<void, E> &rhs)
    {
        if (lhs.has_value() != rhs.has_value())
            return false;
        if (lhs.has_value())
            return true;
        return lhs.error() == rhs.error();
    }

    template <typename E>
    constexpr bool operator==(const expected<void, E> &lhs, const unexpected<E> &rhs)
    {
        return !lhs.has_value() && lhs.error() == rhs.value();
    }
}

#endif // NSTD_EXPECTED_HPP