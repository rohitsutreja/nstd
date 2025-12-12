#ifndef NSTD_EXPECTED_HPP
#define NSTD_EXPECTED_HPP

#include <utility>
#include <new>
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
        explicit unexpected(const E &val);
        explicit unexpected(E &&val) noexcept(std::is_nothrow_move_constructible_v<E>);

        // Accessors
        const E &value() const & noexcept;
        E &value() & noexcept;
        E &&value() && noexcept; // R-value accessor (Steal data)
        const E &&value() const && noexcept;

    private:
        E _val;
    };

    template <typename T, typename E>
    class expected
    {
    public:
        // Constructors
        expected()
            requires std::is_default_constructible_v<T>;

        expected(const T &val);
        expected(T &&val) noexcept(std::is_nothrow_move_constructible_v<T>);

        expected(const unexpected<E> &err);
        expected(unexpected<E> &&err) noexcept(std::is_nothrow_move_constructible_v<E>);

        expected(const expected &other);
        expected(expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>);

        // Destructor
        ~expected();

        // ==========================================
        //           ASSIGNMENT OPERATORS
        // ==========================================

        // 1. Copy Assignment (Manual Implementation)
        expected &operator=(const expected &other);

        // 2. Move Assignment (Manual Implementation)
        expected &operator=(expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>);

        // 3. Mixed Assignments (Manual Implementation)
        expected &operator=(const T &rhs);
        expected &operator=(T &&rhs);
        expected &operator=(const unexpected<E> &rhs);
        expected &operator=(unexpected<E> &&rhs);

        // ==========================================
        //               OBSERVERS
        // ==========================================
        bool has_value() const noexcept;
        explicit constexpr operator bool() const noexcept;

        // --- operator* (Unchecked Access) ---
        T &operator*() & noexcept;
        const T &operator*() const & noexcept;
        T &&operator*() && noexcept;
        const T &&operator*() const && noexcept;

        // --- operator-> (Unchecked Access) ---
        T *operator->() noexcept;
        const T *operator->() const noexcept;

        // --- value() (Checked Access) ---
        T &value() &;
        const T &value() const &;
        T &&value() &&;
        const T &&value() const &&;

        // --- error() (Checked Access) ---
        E &error() &;
        const E &error() const &;
        E &&error() &&;
        const E &&error() const &&;

        // swap
        void swap(expected &other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>);

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
    unexpected<E>::unexpected(E &&val) noexcept(std::is_nothrow_move_constructible_v<E>)
        : _val{std::move(val)} {}

    template <typename E>
    const E &unexpected<E>::value() const & noexcept { return _val; }

    template <typename E>
    E &unexpected<E>::value() & noexcept { return _val; }

    template <typename E>
    E &&unexpected<E>::value() && noexcept { return std::move(_val); }

    template <typename E>
    const E &&unexpected<E>::value() const && noexcept { return std::move(_val); }

    // --- expected implementation ---

    // Constructors
    template <typename T, typename E>
    expected<T, E>::expected()
        requires std::is_default_constructible_v<T>
        : _value{}, _hasValue{true}
    {
    }

    template <typename T, typename E>
    expected<T, E>::expected(const T &val) : _value{val}, _hasValue{true} {}

    template <typename T, typename E>
    expected<T, E>::expected(T &&val) noexcept(std::is_nothrow_move_constructible_v<T>)
        : _value{std::move(val)}, _hasValue{true} {}

    template <typename T, typename E>
    expected<T, E>::expected(const unexpected<E> &err)
        : _error{err.value()}, _hasValue{false} {}

    template <typename T, typename E>
    expected<T, E>::expected(unexpected<E> &&err) noexcept(std::is_nothrow_move_constructible_v<E>)
        : _error{std::move(err).value()}, _hasValue{false} {}

    template <typename T, typename E>
    expected<T, E>::expected(const expected &other) : _hasValue{other._hasValue}
    {
        if (_hasValue)
            new (&_value) T(other._value);
        else
            new (&_error) E(other._error);
    }

    template <typename T, typename E>
    expected<T, E>::expected(expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>)
        : _hasValue{other._hasValue}
    {
        if (_hasValue)
            new (&_value) T{std::move(other._value)};
        else
            new (&_error) E{std::move(other._error)};
    }

    // Destructor
    template <typename T, typename E>
    expected<T, E>::~expected()
    {
        if (_hasValue)
            _value.~T();
        else
            _error.~E();
    }

    // Assignment Operators

    // 1. Copy Assignment
    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(const expected &other)
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
            _value.~T();
            _hasValue = false;
            new (&_error) E(other._error);
        }
        else
        {
            _error.~E();
            _hasValue = true;
            new (&_value) T(other._value);
        }
        return *this;
    }

    // 2. Move Assignment
    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>)
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
            _value.~T();
            _hasValue = false;
            new (&_error) E{std::move(other._error)};
        }
        else
        {
            _error.~E();
            _hasValue = true;
            new (&_value) T{std::move(other._value)};
        }
        return *this;
    }

    // 3. Mixed Assignments
    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(const T &rhs)
    {
        if (_hasValue)
        {
            _value = rhs;
        }
        else
        {
            _error.~E();
            _hasValue = true;
            new (&_value) T(rhs);
        }
        return *this;
    }

    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(T &&rhs)
    {
        if (_hasValue)
        {
            _value = std::move(rhs);
        }
        else
        {
            _error.~E();
            _hasValue = true;
            new (&_value) T(std::move(rhs));
        }
        return *this;
    }

    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(const unexpected<E> &rhs)
    {
        if (!_hasValue)
        {
            _error = rhs.value();
        }
        else
        {
            _value.~T();
            _hasValue = false;
            new (&_error) E(rhs.value());
        }
        return *this;
    }

    template <typename T, typename E>
    expected<T, E> &expected<T, E>::operator=(unexpected<E> &&rhs)
    {
        if (!_hasValue)
        {
            _error = std::move(rhs).value();
        }
        else
        {
            _value.~T();
            _hasValue = false;
            new (&_error) E(std::move(rhs).value());
        }
        return *this;
    }

    // Observers
    template <typename T, typename E>
    bool expected<T, E>::has_value() const noexcept { return _hasValue; }

    template <typename T, typename E>
    constexpr expected<T, E>::operator bool() const noexcept { return _hasValue; }

    // --- operator* Implementation ---
    template <typename T, typename E>
    T &expected<T, E>::operator*() & noexcept
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }
    template <typename T, typename E>
    const T &expected<T, E>::operator*() const & noexcept
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }
    template <typename T, typename E>
    T &&expected<T, E>::operator*() && noexcept
    {
        assert(_hasValue && "Bad expected access");
        return std::move(_value);
    }
    template <typename T, typename E>
    const T &&expected<T, E>::operator*() const && noexcept
    {
        assert(_hasValue && "Bad expected access");
        return std::move(_value);
    }

    // --- operator-> Implementation ---
    template <typename T, typename E>
    T *expected<T, E>::operator->() noexcept
    {
        assert(_hasValue && "Bad expected access");
        return &_value;
    }
    template <typename T, typename E>
    const T *expected<T, E>::operator->() const noexcept
    {
        assert(_hasValue && "Bad expected access");
        return &_value;
    }

    // --- value() Implementation ---
    template <typename T, typename E>
    T &expected<T, E>::value() &
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }
    template <typename T, typename E>
    const T &expected<T, E>::value() const &
    {
        assert(_hasValue && "Bad expected access");
        return _value;
    }
    template <typename T, typename E>
    T &&expected<T, E>::value() &&
    {
        assert(_hasValue && "Bad expected access");
        return std::move(_value);
    }
    template <typename T, typename E>
    const T &&expected<T, E>::value() const &&
    {
        assert(_hasValue && "Bad expected access");
        return std::move(_value);
    }

    // --- error() Implementation ---
    template <typename T, typename E>
    E &expected<T, E>::error() &
    {
        assert(!_hasValue && "Bad error access");
        return _error;
    }
    template <typename T, typename E>
    const E &expected<T, E>::error() const &
    {
        assert(!_hasValue && "Bad error access");
        return _error;
    }
    template <typename T, typename E>
    E &&expected<T, E>::error() &&
    {
        assert(!_hasValue && "Bad error access");
        return std::move(_error);
    }
    template <typename T, typename E>
    const E &&expected<T, E>::error() const &&
    {
        assert(!_hasValue && "Bad error access");
        return std::move(_error);
    }

    // --- Swap ---
    template <typename T, typename E>
    void expected<T, E>::swap(expected &other) noexcept(
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
            _value.~T();
            new (&_error) E{std::move(other._error)};
            _hasValue = false;
            other._error.~E();
            new (&other._value) T{std::move(temp)};
            other._hasValue = true;
        }
        else
        {
            other.swap(*this);
        }
    }

    // Comparision Operations
    template <typename T, typename E>
    bool operator==(const expected<T, E> &lhs, const expected<T, E> &rhs)
    {
        if (lhs.has_value() != rhs.has_value())
            return false;
        if (lhs.has_value())
            return lhs.value() == rhs.value();
        return lhs.error() == rhs.error();
    }

    template <typename T, typename E>
    bool operator==(const expected<T, E> &lhs, const T &rhs)
    {
        return lhs.has_value() && lhs.value() == rhs;
    }

    template <typename T, typename E>
    bool operator==(const expected<T, E> &lhs, const unexpected<E> &rhs)
    {
        return !lhs.has_value() && lhs.error() == rhs.value();
    }
}

#endif // NSTD_EXPECTED_HPP