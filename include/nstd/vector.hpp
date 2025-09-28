#include <cassert>
#include <utility>
#include <stdexcept>
#include <type_traits>

namespace nstd
{
    template <typename T>
    class vector
    {
    public:
        // Default constructor
        vector() : _data{nullptr},
                   _capacity{0},
                   _length{0} {}

        // copy constructor
        vector(const vector &other) : vector()
        {
            if (other._length <= 0)
            {
                return;
            }

            _data = new T[other._length];
            _capacity = other._length;

            try
            {
                for (size_t i{}; i < other._length; ++i)
                {
                    _data[i] = other._data[i];
                    ++_length;
                }
            }
            catch (...)
            {
                delete[] _data;
                throw;
            }
        }

        vector(vector &&other) : _data{other._data},
                                 _capacity{other._capacity},
                                 _length{other._length}
        {
            other._data = nullptr;
            other._capacity = 0;
            other._length = 0;
        }

        // destructor
        ~vector()
        {
            delete[] _data;
        }

        // copy and move assignment
        vector &operator=(vector other)
        {
            swap(*this, other);
            return *this;
        }

        T &operator[](const size_t index)
        {
            assert(index < _length);
            return _data[index];
        }

        const T &operator[](const size_t index) const
        {
            assert(index < _length);
            return _data[index];
        }

        T &at(size_t index)
        {
            if (index >= _length)
                throw std::out_of_range("vector index out of range");
            return _data[index];
        }

        const T &at(size_t index) const
        {
            if (index >= _length)
                throw std::out_of_range("vector index out of range");
            return _data[index];
        }

        size_t size() const
        {
            return _length;
        }

        bool is_empty() const
        {
            return _length == 0;
        }

        size_t get_capacity() const
        {
            return _capacity;
        }

        void reserve(size_t new_capacity)
        {
            if (new_capacity <= _capacity)
            {
                return;
            }

            auto *new_memory{new T[new_capacity]};

            try
            {
                for (size_t i{}; i < _length; ++i)
                {
                    // We only use move if is noexcept, because if move throws our vector will loose values already moved.
                    if constexpr (std::is_nothrow_move_constructible_v<T>)
                    {
                        new_memory[i] = std::move(_data[i]);
                    }
                    else
                    {
                        new_memory[i] = _data[i];
                    }
                }
            }
            catch (...)
            {
                delete[] new_memory;
                throw;
            }

            delete[] _data;

            _data = new_memory;
            _capacity = new_capacity;
        }

        void shrink_to_fit()
        {
            if (_length == 0)
            {
                delete[] _data;
                _data = nullptr;
                _capacity = 0;
                _length = 0;
                return;
            }

            if (_length < _capacity)
            {
                auto *new_memory{new T[_length]};

                try
                {
                    for (size_t i{}; i < _length; ++i)
                    {
                        // Use move only if noexcept to avoid partial moves on exceptions
                        if constexpr (std::is_nothrow_move_constructible_v<T>)
                        {
                            new_memory[i] = std::move(_data[i]);
                        }
                        else
                        {
                            new_memory[i] = _data[i];
                        }
                    }
                }
                catch (...)
                {
                    delete[] new_memory;
                    throw;
                }

                delete[] _data;
                _data = new_memory;
                _capacity = _length;
            }
        }

        void push_back(const T &element)
        {
            if (_length == _capacity)
            {
                _reallocate();
            }

            _data[_length++] = element;
        }

        void push_back(T &&element)
        {
            if (_length == _capacity)
            {
                _reallocate();
            }

            _data[_length++] = std::move(element);
        }

        void pop_back()
        {
            assert(_length >= 1);
            --_length;
        }

        T &front()
        {
            assert(_length >= 1);
            return _data[0];
        }

        T &back()
        {
            assert(_length >= 1);
            return _data[_length - 1];
        }

        const T &front() const
        {
            assert(_length >= 1);
            return _data[0];
        }

        const T &back() const
        {
            assert(_length >= 1);
            return _data[_length - 1];
        }

        void clear()
        {
            for (size_t i = _length; i > 0; --i)
            {
                _data[i - 1].~T();
            }
            _length = 0;
        }

        // Iterators
        T *begin() noexcept { return _data; }
        T *end() noexcept { return _data + _length; }
        const T *begin() const noexcept { return _data; }
        const T *end() const noexcept { return _data + _length; }
        const T *cbegin() const noexcept { return _data; }
        const T *cend() const noexcept { return _data + _length; }

        T* data() noexcept { return _data; }
        const T* data() const noexcept { return _data; }

        friend void swap(vector &a, vector &b) noexcept
        {
            std::swap(a._data, b._data);
            std::swap(a._capacity, b._capacity);
            std::swap(a._length, b._length);
        }

    private:
        void _reallocate()
        {
            auto new_capacity{_capacity ? _capacity * 2 : 1};
            reserve(new_capacity);
        }

        size_t _capacity{};
        size_t _length{};
        T *_data{};
    };
}