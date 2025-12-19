#ifndef NSTD_VECTOR_HPP
#define NSTD_VECTOR_HPP

#include <cassert>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <memory>     
#include <new>         
#include <cstring>     

namespace nstd
{
	template <typename T>
	class vector
	{
	public:
		vector();
		vector(const std::initializer_list<T> list);
		vector(const vector& other);
		vector(vector&& other) noexcept;
		~vector();
		vector& operator=(vector other);
		T& operator[](const size_t index);
		const T& operator[](const size_t index) const;
		T& at(size_t index);
		const T& at(size_t index) const;
		size_t size() const;
		bool is_empty() const;
		size_t get_capacity() const;
		void reserve(size_t new_capacity);
		void shrink_to_fit();
		void push_back(const T& element);
		void push_back(T&& element);
		void pop_back();
		T& front();
		T& back();
		const T& front() const;
		const T& back() const;
		void clear();
		T* begin() noexcept;
		T* end() noexcept;
		const T* begin() const noexcept;
		const T* end() const noexcept;
		const T* cbegin() const noexcept;
		const T* cend() const noexcept;
		T* data() noexcept;
		const T* data() const noexcept;

		friend void swap(vector& a, vector& b) noexcept
		{
			std::swap(a._data, b._data);
			std::swap(a._capacity, b._capacity);
			std::swap(a._length, b._length);
		}

	private:
		size_t _capacity{};
		size_t _length{};
		T* _data{};
	};

	// Definitions

	template <typename T>
	vector<T>::vector() : _data{ nullptr }, _capacity{ 0 }, _length{ 0 } {}

	template <typename T>
	vector<T>::vector(const std::initializer_list<T> list)
		: vector()
	{
		if (list.size() == 0)
		{
			return;
		}

		_capacity = list.size();
		_data = static_cast<T*>(::operator new(sizeof(T) * _capacity));

		try
		{
			for (const auto& item : list)
			{
				std::construct_at(_data + _length, item);
				++_length;
			}
		}
		catch (...)
		{
			std::destroy(_data, _data + _length);
			::operator delete(_data);
			throw;
		}
	}

	template <typename T>
	vector<T>::vector(const vector& other) : vector()
	{
		if (other._length <= 0)
		{
			return;
		}

		_capacity = other._capacity;
		_data = static_cast<T*>(operator new(sizeof(T) * _capacity));

		try {
			for (size_t i{}; i < other._length; ++i) {
				std::construct_at(_data + i, other._data[i]);
				++_length;
			}
		}
		catch (...) {
			std::destroy(_data, _data + _length);
			::operator delete(_data);
			throw;
		}
	}

	template <typename T>
	vector<T>::vector(vector&& other) noexcept : _data{ other._data },
		_capacity{ other._capacity },
		_length{ other._length }
	{
		other._data = nullptr;
		other._capacity = 0;
		other._length = 0;
	}

	template <typename T>
	vector<T>::~vector()
	{
		std::destroy(_data, _data + _length);
		::operator delete(_data);
	}

	template <typename T>
	vector<T>& vector<T>::operator=(vector other)
	{
		swap(*this, other);
		return *this;
	}

	template <typename T>
	T& vector<T>::operator[](const size_t index)
	{
		assert(index < _length);
		return _data[index];
	}

	template <typename T>
	const T& vector<T>::operator[](const size_t index) const
	{
		assert(index < _length);
		return _data[index];
	}

	template <typename T>
	T& vector<T>::at(size_t index)
	{
		if (index >= _length)
			throw std::out_of_range("vector index out of range");
		return _data[index];
	}

	template <typename T>
	const T& vector<T>::at(size_t index) const
	{
		if (index >= _length)
			throw std::out_of_range("vector index out of range");
		return _data[index];
	}

	template <typename T>
	size_t vector<T>::size() const
	{
		return _length;
	}

	template <typename T>
	bool vector<T>::is_empty() const
	{
		return _length == 0;
	}

	template <typename T>
	size_t vector<T>::get_capacity() const
	{
		return _capacity;
	}

	template <typename T>
	void vector<T>::reserve(size_t new_cap)
	{
		if (new_cap <= _capacity) {
			return;
		}

		auto* new_mem{ static_cast<T*>(::operator new(new_cap * sizeof(T))) };
		size_t i{};

		try
		{
			for (; i < _length; ++i)
			{
				std::construct_at(new_mem + i, std::move_if_noexcept(_data[i]));
			}
		}
		catch (...)
		{
			std::destroy(new_mem, new_mem + i);
			::operator delete(new_mem);
			throw;
		}

		std::destroy(_data, _data + _length);
		::operator delete(_data);

		_data = new_mem;
		_capacity = new_cap;
	}

	template <typename T>
	void vector<T>::shrink_to_fit()
	{
		if (_length == 0)
		{
			std::destroy(_data, _data + _length);
			::operator delete(_data);

			_data = nullptr;
			_capacity = 0;
			_length = 0;
			return;
		}

		if (_length < _capacity)
		{
			auto* new_memory{ static_cast<T*>(::operator new(_length * sizeof(T))) };

			size_t i{};

			try
			{
				for (; i < _length; ++i)
				{
					std::construct_at(new_memory + i, std::move_if_noexcept(_data[i]));
				}
			}
			catch (...)
			{
				std::destroy(new_memory, new_memory + i);
				::operator delete(new_memory);
				throw;
			}

			std::destroy(_data, _data + _length);
			::operator delete(_data);

			_data = new_memory;
			_capacity = _length;
		}
	}

	template <typename T>
	void vector<T>::push_back(const T& element)
	{
		if (_length == _capacity)
		{
			reserve(_capacity ? _capacity * 2 : 1);
		}

		std::construct_at(_data + _length, element);
		++_length;
	}

	template <typename T>
	void vector<T>::push_back(T&& element)
	{
		if (_length == _capacity)
		{
			reserve(_capacity ? _capacity * 2 : 1);
		}

		std::construct_at(_data + _length, std::move(element));
		++_length;
	}

	template <typename T>
	void vector<T>::pop_back()
	{
		assert(_length >= 1);
		std::destroy_at(_data + _length - 1);
		--_length;
	}

	template <typename T>
	T& vector<T>::front()
	{
		assert(_length >= 1);
		return _data[0];
	}

	template <typename T>
	T& vector<T>::back()
	{
		assert(_length >= 1);
		return _data[_length - 1];
	}

	template <typename T>
	const T& vector<T>::front() const
	{
		assert(_length >= 1);
		return _data[0];
	}

	template <typename T>
	const T& vector<T>::back() const
	{
		assert(_length >= 1);
		return _data[_length - 1];
	}

	template <typename T>
	void vector<T>::clear()
	{
		std::destroy(_data, _data + _length);
		_length = 0;
	}

	template <typename T>
	T* vector<T>::begin() noexcept { return _data; }

	template <typename T>
	T* vector<T>::end() noexcept { return _data + _length; }

	template <typename T>
	const T* vector<T>::begin() const noexcept { return _data; }

	template <typename T>
	const T* vector<T>::end() const noexcept { return _data + _length; }

	template <typename T>
	const T* vector<T>::cbegin() const noexcept { return _data; }

	template <typename T>
	const T* vector<T>::cend() const noexcept { return _data + _length; }

	template <typename T>
	T* vector<T>::data() noexcept { return _data; }

	template <typename T>
	const T* vector<T>::data() const noexcept { return _data; }

} // namespace nstd

#endif // NSTD_VECTOR_HPP