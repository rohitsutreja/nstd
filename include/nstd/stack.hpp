#ifndef NSTD_STACK_HPP
#define NSTD_STACK_HPP

#include "vector.hpp"

namespace nstd
{
    template <typename T>
    class stack
    {
    public:
        void push(const T &value)
        {
            _data.push_back(value);
        }

        void push(T &&value)
        {
            _data.push_back(std::move(value));
        }

        T pop()
        {
            auto temp{_data.back()};
            _data.pop_back();
            return temp;
        }

        T &top()
        {
            return _data.back();
        }

        const T &top() const
        {
            return _data.back();
        }

        bool empty() const
        {
            return _data.is_empty();
        }

        size_t size() const
        {
            return _data.size();
        }

    private:
        nstd::vector<T> _data{};
    };
}

#endif // NSTD_STACK_HPP