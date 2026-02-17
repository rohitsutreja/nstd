#ifndef NSTD_UNIQUE_PTR_HPP
#define NSTD_UNIQUE_PTR_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

namespace nstd {

/*-----------------------------------------------------------------------------*/

template<typename T> struct default_delete {
    constexpr default_delete() noexcept = default;

    template<typename U>
    constexpr default_delete(const default_delete<U>&) noexcept
        requires(std::is_convertible_v<U*, T*>);

    constexpr void operator()(T* ptr) const noexcept;
};

template<typename T>
template<typename U>
constexpr default_delete<T>::default_delete(const default_delete<U>&) noexcept
    requires(std::is_convertible_v<U*, T*>) {}

template<typename T> constexpr void default_delete<T>::operator()(T* ptr) const noexcept {
    delete ptr;
}

/*-----------------------------------------------------------------------------*/

template<typename T> struct default_delete<T[]> {
    constexpr default_delete() noexcept = default;

    constexpr void operator()(T* ptr) const noexcept;
};

template<typename T> constexpr void default_delete<T[]>::operator()(T* ptr) const noexcept {
    delete[] ptr;
}

/*-----------------------------------------------------------------------------*/

template<typename T, typename Deleter = default_delete<T>> class unique_ptr {
public:
    constexpr unique_ptr() noexcept = default;
    constexpr unique_ptr(std::nullptr_t) noexcept;
    constexpr explicit unique_ptr(T* ptr) noexcept;
    constexpr unique_ptr(T* ptr, const Deleter& d) noexcept;
    constexpr unique_ptr(T* ptr, Deleter&& d) noexcept;

    template<typename U>
    constexpr unique_ptr(unique_ptr<U>&& other) noexcept requires(std::is_convertible_v<U*, T*>);

    constexpr unique_ptr(unique_ptr&& other) noexcept;

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    template<typename U>
    unique_ptr& operator=(unique_ptr<U>&& other) noexcept requires(std::is_convertible_v<U*, T*>);

    constexpr unique_ptr& operator=(unique_ptr&& other) noexcept;

    constexpr T* operator->() const noexcept;
    constexpr T& operator*() const noexcept;
    constexpr operator bool() const noexcept;
    constexpr T* get() const noexcept;
    constexpr Deleter& get_deleter() noexcept;
    constexpr const Deleter& get_deleter() const noexcept;

    constexpr void reset(T* new_ptr = nullptr) noexcept;
    constexpr T* release() noexcept;

    constexpr ~unique_ptr();

private:
    T* _ptr{};
    [[no_unique_address]] Deleter _deleter{};
};

template<typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr(std::nullptr_t) noexcept {}

template<typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr(T* ptr) noexcept : _ptr{ptr} {}

template<typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr(T* ptr, const Deleter& d) noexcept
    : _ptr{ptr}, _deleter{d} {}

template<typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr(T* ptr, Deleter&& d) noexcept
    : _ptr{ptr}, _deleter{std::move(d)} {}

template<typename T, typename Deleter>
template<typename U>
constexpr unique_ptr<T, Deleter>::unique_ptr(unique_ptr<U>&& other) noexcept
    requires(std::is_convertible_v<U*, T*>)
    : _ptr{other.release()}, _deleter{std::move(other.get_deleter())} {}

template<typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr(unique_ptr&& other) noexcept
    : _ptr{std::exchange(other._ptr, nullptr)}, _deleter{std::move(other._deleter)} {}

template<typename T, typename Deleter>
template<typename U>
unique_ptr<T, Deleter>& unique_ptr<T, Deleter>::operator=(unique_ptr<U>&& other) noexcept
    requires(std::is_convertible_v<U*, T*>) {
    reset(other.release());
    _deleter = std::move(other.get_deleter());
    return *this;
}

template<typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>& unique_ptr<T, Deleter>::operator=(unique_ptr&& other) noexcept {
    if (this != &other) {
        reset(other.release());
        _deleter = std::move(other._deleter);
    }
    return *this;
}

template<typename T, typename Deleter>
constexpr T* unique_ptr<T, Deleter>::operator->() const noexcept {
    return _ptr;
}

template<typename T, typename Deleter>
constexpr T& unique_ptr<T, Deleter>::operator*() const noexcept {
    return *_ptr;
}

template<typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::operator bool() const noexcept {
    return _ptr;
}

template<typename T, typename Deleter> constexpr T* unique_ptr<T, Deleter>::get() const noexcept {
    return _ptr;
}

template<typename T, typename Deleter>
constexpr Deleter& unique_ptr<T, Deleter>::get_deleter() noexcept {
    return _deleter;
}

template<typename T, typename Deleter>
constexpr const Deleter& unique_ptr<T, Deleter>::get_deleter() const noexcept {
    return _deleter;
}

template<typename T, typename Deleter>
constexpr void unique_ptr<T, Deleter>::reset(T* new_ptr) noexcept {
    auto* const old_ptr{std::exchange(_ptr, new_ptr)};
    if (old_ptr != nullptr) {
        _deleter(old_ptr);
    }
}

template<typename T, typename Deleter> constexpr T* unique_ptr<T, Deleter>::release() noexcept {
    T* temp = _ptr;
    _ptr = nullptr;
    return temp;
}

template<typename T, typename Deleter> constexpr unique_ptr<T, Deleter>::~unique_ptr() {
    if (_ptr != nullptr) {
        _deleter(_ptr);
    }
}

/*-----------------------------------------------------------------------------*/

template<typename T, typename Deleter> class unique_ptr<T[], Deleter> {
public:
    constexpr unique_ptr() noexcept = default;
    constexpr unique_ptr(std::nullptr_t) noexcept;

    template<typename U>
    constexpr explicit unique_ptr(U* ptr) noexcept requires(std::is_same_v<U, T>);

    template<typename U>
    constexpr unique_ptr(U* ptr, const Deleter& d) noexcept requires(std::is_same_v<U, T>);

    template<typename U>
    constexpr unique_ptr(U* ptr, Deleter&& d) noexcept requires(std::is_same_v<U, T>);
    ;

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    constexpr unique_ptr(unique_ptr&& other) noexcept;
    constexpr unique_ptr& operator=(unique_ptr&& other) noexcept;

    constexpr operator bool() const noexcept;
    constexpr T* get() const noexcept;
    constexpr T& operator[](std::size_t index) const noexcept;
    constexpr Deleter& get_deleter() noexcept;
    constexpr const Deleter& get_deleter() const noexcept;

    constexpr void reset(T* new_ptr = nullptr) noexcept;
    constexpr T* release() noexcept;

    constexpr ~unique_ptr();

private:
    T* _ptr{};
    [[no_unique_address]] Deleter _deleter{};
};

template<typename T, typename Deleter>
constexpr unique_ptr<T[], Deleter>::unique_ptr(std::nullptr_t) noexcept {}

template<typename T, typename Deleter>
template<typename U>
constexpr unique_ptr<T[], Deleter>::unique_ptr(U* ptr) noexcept requires(std::is_same_v<U, T>)
    : _ptr{ptr} {}

template<typename T, typename Deleter>
template<typename U>
constexpr unique_ptr<T[], Deleter>::unique_ptr(U* ptr, const Deleter& d) noexcept
    requires(std::is_same_v<U, T>)
    : _ptr{ptr}, _deleter{d} {}

template<typename T, typename Deleter>
template<typename U>
constexpr unique_ptr<T[], Deleter>::unique_ptr(U* ptr, Deleter&& d) noexcept
    requires(std::is_same_v<U, T>)
    : _ptr{ptr}, _deleter{std::move(d)} {}

template<typename T, typename Deleter>
constexpr unique_ptr<T[], Deleter>::unique_ptr(unique_ptr&& other) noexcept
    : _ptr{std::exchange(other._ptr, nullptr)}, _deleter{std::move(other._deleter)} {}

template<typename T, typename Deleter>
constexpr unique_ptr<T[], Deleter>&
unique_ptr<T[], Deleter>::operator=(unique_ptr&& other) noexcept {
    if (this != &other) {
        reset(other.release());
        _deleter = std::move(other._deleter);
    }
    return *this;
}

template<typename T, typename Deleter>
constexpr unique_ptr<T[], Deleter>::operator bool() const noexcept {
    return _ptr;
}

template<typename T, typename Deleter> constexpr T* unique_ptr<T[], Deleter>::get() const noexcept {
    return _ptr;
}

template<typename T, typename Deleter>
constexpr T& unique_ptr<T[], Deleter>::operator[](std::size_t index) const noexcept {
    return _ptr[index];
}

template<typename T, typename Deleter>
constexpr Deleter& unique_ptr<T[], Deleter>::get_deleter() noexcept {
    return _deleter;
}

template<typename T, typename Deleter>
constexpr const Deleter& unique_ptr<T[], Deleter>::get_deleter() const noexcept {
    return _deleter;
}

template<typename T, typename Deleter>
constexpr void unique_ptr<T[], Deleter>::reset(T* new_ptr) noexcept {
    auto* const old_ptr{std::exchange(_ptr, new_ptr)};
    if (old_ptr != nullptr) {
        _deleter(old_ptr);
    }
}

template<typename T, typename Deleter> constexpr T* unique_ptr<T[], Deleter>::release() noexcept {
    return std::exchange(_ptr, nullptr);
}

template<typename T, typename Deleter> constexpr unique_ptr<T[], Deleter>::~unique_ptr() {
    reset();
}

template<typename T, typename... Args> constexpr unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>{new T(std::forward<Args>(args)...)};
}

template<typename T>
constexpr unique_ptr<T>
make_unique(std::size_t size) requires(std::is_array_v<T>&& std::extent_v<T> == 0) {
    using ElementType = std::remove_extent_t<T>;
    return unique_ptr<T>{new ElementType[size]()};
}
} // namespace nstd

#endif