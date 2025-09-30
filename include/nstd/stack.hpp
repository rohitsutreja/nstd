#ifndef NSTD_STACK_HPP
#define NSTD_STACK_HPP

#include "vector.hpp"

namespace nstd
{
    template <typename T>
    class stack
    {
    public:
        void push(const T &value);
        void push(T &&value);
        T pop();
        T &top();
        const T &top() const;
        bool empty() const;
        size_t size() const;

    private:
        nstd::vector<T> _data{};
    };

    // Definitions

    template <typename T>
    void stack<T>::push(const T &value)
    {
        _data.push_back(value);
    }

    template <typename T>
    void stack<T>::push(T &&value)
    {
        _data.push_back(std::move(value));
    }

    template <typename T>
    T stack<T>::pop()
    {
        auto temp{_data.back()};
        _data.pop_back();
        return temp;
    }

    template <typename T>
    T &stack<T>::top()
    {
        return _data.back();
    }

    template <typename T>
    const T &stack<T>::top() const
    {
        return _data.back();
    }

    template <typename T>
    bool stack<T>::empty() const
    {
        return _data.is_empty();
    }

    template <typename T>
    size_t stack<T>::size() const
    {
        return _data.size();
    }
}

#endif // NSTD_STACK_HPP