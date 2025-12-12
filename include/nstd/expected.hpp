#ifndef NSTD_EXPECTED_HPP
#define NSTD_EXPECTED_HPP

#include <utility>
#include <new>
#include <cassert>

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
        explicit unexpected(const E &val);
        explicit unexpected(E &&val);

        const E &value() const;
        E &value();

    private:
        E _val;
    };

    template <typename T, typename E>
    class expected
    {
    public:
        // Constructors
        expected();
        expected(const T &val);
        expected(const unexpected<E> &err);
        expected(const expected &other);
        expected(expected &&other) noexcept;

        // Destructor
        ~expected();

        // Assignment Operators
        expected &operator=(const expected &other);
        expected &operator=(expected &&other) noexcept;
        expected &operator=(const T &rhs);
        expected &operator=(const unexpected<E> &rhs);

        // Observers
        bool has_value() const noexcept;
        T &operator*();
        const T &operator*() const;
        T *operator->();
        const T *operator->() const;
        E &error();
        const E &error() const;

    private:
        union
        {
            T _value;
            E _error;
        };

        bool _hasValue;
    };

    // ==========================================
    //              Implementation
    // ==========================================

    // --- unexpected implementation ---

    template <typename E>
    unexpected<E>::unexpected(const E &val) : _val{val} {}

    template <typename E>
    unexpected<E>::unexpected(E &&val) : _val{std::move(val)} {}

    template <typename E>
    const E &unexpected<E>::value() const { return _val; }

    template <typename E>
    E &unexpected<E>::value() { return _val; }

    // --- expected implementation ---

    // Constructors
    template <typename T, typename E>
    expected<T, E>::expected()
        : _value{}, _hasValue{true}
    {
    }

    template <typename T, typename E>
    expected<T, E>::expected(const T &val)
        : _value{val}, _hasValue{true}
    {
    }

    template <typename T, typename E>
    expected<T, E>::expected(const unexpected<E> &err)
        : _error{err.value()}, _hasValue{false}
    {
    }

    template <typename T, typename E>
    expected<T, E>::expected(const expected &other)
        : _hasValue{other._hasValue}
    {
        if (_hasValue)
        {
            new (&_value) T(other._value);
        }
        else
        {
            new (&_error) E(other._error);
        }
    }

    template <typename T, typename E>
    expected<T, E>::expected(expected &&other) noexcept
        : _hasValue{other._hasValue}
    {
        if (_hasValue)
        {
            new (&_value) T{std::move(other._value)};
        }
        else
        {
            new (&_error) E{std::move(other._error)};
        }
    }

    // Destructor
    template <typename T, typename E>
    expected<T, E>::~expected()
    {
        if (_hasValue)
        {
            _value.~T();
        }
        else
        {
            _error.~E();
        }
    }

    // Assignment Operators
    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(const expected &other)
    {
        if (this == &other)
        {
            return *this;
        }

        if (other._hasValue && _hasValue)
        {
            _value = other._value;
            return *this;
        }

        if (!other._hasValue && !_hasValue)
        {
            _error = other._error;
            return *this;
        }

        if (_hasValue && !other._hasValue)
        {
            _value.~T();
            new (&_error) E(other._error);
            _hasValue = false;
            return *this;
        }

        _error.~E();
        new (&_value) T(other._value);
        _hasValue = true;
        return *this;
    }

    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(expected &&other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        if (_hasValue && other._hasValue)
        {
            _value = std::move(other._value);
            return *this;
        }

        if (!_hasValue && !other._hasValue)
        {
            _error = std::move(other._error);
            return *this;
        }

        if (_hasValue && !other._hasValue)
        {
            _hasValue = false;
            _value.~T();
            new (&_error) E{std::move(other._error)};
            return *this;
        }

        _hasValue = true;
        _error.~E();
        new (&_value) T{std::move(other._value)};
        return *this;
    }

    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(const T &rhs)
    {
        if (_hasValue)
        {
            _value = rhs;
            return *this;
        }

        _hasValue = true;
        _error.~E();
        new (&_value) T(rhs);

        return *this;
    }

    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(const unexpected<E> &rhs)
    {
        if (!_hasValue)
        {
            _error = rhs.value();
            return *this;
        }

        _hasValue = false;
        _value.~T();
        new (&_error) E(rhs.value());

        return *this;
    }

    // Observers
    template <typename T, typename E>
    bool expected<T, E>::has_value() const noexcept
    {
        return _hasValue;
    }

    template <typename T, typename E>
    T &expected<T, E>::operator*()
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }

    template <typename T, typename E>
    const T &expected<T, E>::operator*() const
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }

    template <typename T, typename E>
    T *expected<T, E>::operator->()
    {
        assert(_hasValue && "Bad expected access");
        return &_value;
    }

    template <typename T, typename E>
    const T *expected<T, E>::operator->() const
    {
        assert(_hasValue && "Bad expected access");
        return &_value;
    }

    template <typename T, typename E>
    E &expected<T, E>::error()
    {
        assert(!_hasValue && "Bad error access");
        return _error;
    }

    template <typename T, typename E>
    const E &expected<T, E>::error() const
    {
        assert(!_hasValue && "Bad error access");
        return _error;
    }
}

#endif // NSTD_EXPECTED_HPP