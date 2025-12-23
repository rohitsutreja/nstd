#ifndef NSTD_VECTOR_HPP
#define NSTD_VECTOR_HPP

#include <cassert>
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <compare> 

namespace nstd
{
	template <typename T>
	class vector
	{
	public:
		using value_type = T;
		using iterator = T*;
		using const_iterator = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using const_pointer = const T*;

		// --- Constructors & Destructor ---
		vector();
		vector(const std::initializer_list<T> list);
		vector(size_type count, const_reference value);
		vector(const vector& other);
		vector(vector&& other) noexcept;

		template<typename Iter>
		vector(Iter first, Iter last);

		~vector();

		// --- Assignment ---

		// using Copy and Swap idiom
		// vector& operator=(vector other);

		vector& operator=(const vector& other);
		vector& operator=(vector&& other) noexcept;

		// --- Iterators ---
		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		const_iterator cbegin() const noexcept;

		iterator end() noexcept;
		const_iterator end() const noexcept;
		const_iterator cend() const noexcept;

		// --- Capacity ---
		size_type size() const noexcept;
		size_type get_capacity() const noexcept;
		size_type max_size() const noexcept;
		bool is_empty() const noexcept;
		void reserve(size_type new_capacity);
		void resize(size_type new_length, const_reference value);
		void resize(size_type new_length);
		void shrink_to_fit();

		// --- Element Access ---
		reference operator[](const size_type index) noexcept;
		const_reference operator[](const size_type index) const noexcept;

		reference at(size_type index);
		const_reference at(size_type index) const;

		reference front() noexcept;
		const_reference front() const noexcept;

		reference back() noexcept;
		const_reference back() const noexcept;

		iterator data() noexcept;
		const_iterator data() const noexcept;

		// --- Modifiers ---
		void push_back(const_reference element);
		void push_back(T&& element);

		iterator insert(const_iterator pos, const_reference value);
		iterator insert(const_iterator pos, T&& value);
		iterator insert(const_iterator pos, size_type count, const_reference value);

		iterator erase(const_iterator pos);

		template <typename ...Args>
		reference emplace_back(Args&& ...args);

		void pop_back() noexcept;
		void clear() noexcept;

		// --- Friends ---
		friend void swap(vector& a, vector& b) noexcept
		{
			std::swap(a._data, b._data);
			std::swap(a._capacity, b._capacity);
			std::swap(a._length, b._length);
		}

		friend bool operator==(const vector& first, const vector& second) {
			if (&first == &second) {
				return true;
			}

			if (first.size() != second.size()) {
				return false;
			}

			for (auto ItF{ first.cbegin() }, ItS{ second.cbegin() }; ItF != first.cend(); ++ItF, ++ItS) {
				if (*ItF != *ItS) {
					return false;
				}
			}

			return true;
		}

		friend auto operator<=>(const vector& lhs, const vector& rhs)
		{
			const auto min_len{ (lhs.size() < rhs.size()) ? lhs.size() : rhs.size() };

			for (size_t i{}; i < min_len; ++i) {
				auto cmp{ lhs[i] <=> rhs[i] };

				if (cmp != 0) {
					return cmp;
				}
			}

			return lhs.size() <=> rhs.size();
		}

	private:
		// --- Helpers ---
		void _reallocate(size_type size);

		// --- Member Data ---
		size_type _capacity{};
		size_type _length{};
		T* _data{};
	};

	// ==========================================
	// Implementation
	// ==========================================

	// --- Constructors & Destructor ---

	template <typename T>
	vector<T>::vector()
		: _capacity{ 0 }, _length{ 0 }, _data{ nullptr }
	{
	}

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

	template<typename T>
	vector<T>::vector(size_type count, const_reference value)
	{
		reserve(count);

		for (size_type i{}; i < count; ++i) {
			std::construct_at(_data + _length++, value);
		}
	}

	template <typename T>
	vector<T>::vector(const vector& other)
		: vector()
	{
		if (other._length == 0)
		{
			return;
		}

		_data = static_cast<T*>(::operator new(sizeof(T) * other._capacity));
		_capacity = other._capacity;

		try
		{
			// Either we can use direct index or since our class has begin and end we can use range based for loop too.

			//for (size_type i{}; i < other._length; ++i)
			//{
			//	std::construct_at(_data + i, other._data[i]);
			//	++_length;
			//}

			for (const auto& elem : other)
			{
				std::construct_at(_data + _length, elem);
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
	vector<T>::vector(vector&& other) noexcept
		: _data{ other._data },
		_capacity{ other._capacity },
		_length{ other._length }
	{
		other._data = nullptr;
		other._capacity = 0;
		other._length = 0;
	}

	template<typename T>
	template<typename Iter>
	inline vector<T>::vector(Iter first, Iter last)
	{
		auto size{ std::distance(first, last) };
		if (size > 0) {
			reserve(size);
			for (; first != last; ++first) {
				std::construct_at(_data + _length++, *first);
			}
		}
	}

	template <typename T>
	vector<T>::~vector()
	{
		std::destroy(_data, _data + _length);
		::operator delete(_data);
	}

	// --- Assignment ---

	// Copy/Move assignment using copy and swap idiom.

	//template <typename T>
	//vector<T>& vector<T>::operator=(vector other)
	//{
	//	swap(*this, other);
	//	return *this;
	//}

	template<typename T>
	vector<T>& vector<T>::operator=(const vector& other) {
		if (this != &other) {
			if (other._length == 0) {
				clear();
				::operator delete(_data);
				_data = nullptr;
				_capacity = 0;
				return *this;
			}

			auto* new_mem{ static_cast<pointer>(::operator new(sizeof(value_type) * other._length)) };

			auto thisIt{ new_mem };

			try {
				for (auto otherIt{ other.cbegin() }; otherIt != other.cend(); ++otherIt, ++thisIt) {
					std::construct_at(thisIt, *otherIt);
				}
			}
			catch (...) {
				std::destroy(new_mem, thisIt);
				::operator delete(new_mem);
				throw;

			}

			clear();

			::operator delete(_data);
			_data = new_mem;
			_capacity = other._length;
			_length = other._length;
		}

		return *this;
	}

	template<typename T>
	vector<T>& vector<T>::operator=(vector&& other) noexcept {
		if (this != &other) {
			clear();
			::operator delete(_data);

			_data = other._data;
			_length = other._length;
			_capacity = other._capacity;

			other._data = nullptr;
			other._length = 0;
			other._capacity = 0;
		}
		return *this;
	}


	// --- Iterators ---

	template <typename T>
	typename vector<T>::iterator vector<T>::begin() noexcept { return _data; }

	template <typename T>
	typename vector<T>::const_iterator vector<T>::begin() const noexcept { return _data; }

	template <typename T>
	typename vector<T>::const_iterator vector<T>::cbegin() const noexcept { return _data; }

	template <typename T>
	typename vector<T>::iterator vector<T>::end() noexcept { return _data + _length; }

	template <typename T>
	typename vector<T>::const_iterator vector<T>::end() const noexcept { return _data + _length; }

	template <typename T>
	typename vector<T>::const_iterator vector<T>::cend() const noexcept { return _data + _length; }

	// --- Capacity ---

	template <typename T>
	typename vector<T>::size_type vector<T>::size() const noexcept
	{
		return _length;
	}

	template <typename T>
	typename vector<T>::size_type vector<T>::get_capacity() const noexcept
	{
		return _capacity;
	}

	template <typename T>
	typename vector<T>::size_type vector<T>::max_size() const noexcept
	{
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	template <typename T>
	bool vector<T>::is_empty() const noexcept
	{
		return _length == 0;
	}

	template <typename T>
	void vector<T>::reserve(size_type new_cap)
	{
		if (new_cap <= _capacity)
		{
			return;
		}

		_reallocate(new_cap);
	}

	template<typename T>
	void vector<T>::resize(size_type new_length, const_reference value)
	{
		if (new_length == _length) {
			return;
		}

		if (new_length < _length) {
			std::destroy(_data + new_length, _data + _length);
			_length = new_length;
			return;
		}

		if (new_length > _capacity) {
			reserve(new_length);
		}

		for (size_type i{ _length }; i < new_length; ++i) {
			std::construct_at(_data + i, value);
			++_length;
		}
	}


	template<typename T>
	void vector<T>::resize(size_type new_length) {
		if (new_length == _length) {
			return;
		}

		if (new_length < _length) {
			std::destroy(_data + new_length, _data + _length);
			_length = new_length;
			return;
		}

		if (new_length > _capacity) {
			reserve(new_length);
		}

		for (size_type i{ _length }; i < new_length; ++i) {
			// NO ARGUMENTS passed here.
			// This invokes the Default Constructor directly in-place.
			// It does NOT invoke the Copy Constructor.
			std::construct_at(_data + i);
			++_length;
		}
	}

	template <typename T>
	void vector<T>::shrink_to_fit()
	{
		if (_length == 0)
		{
			::operator delete(_data);
			_data = nullptr;
			_capacity = 0;
			return;
		}

		if (_length < _capacity)
		{
			_reallocate(_length);
		}
	}

	// --- Element Access ---

	template <typename T>
	typename vector<T>::reference vector<T>::operator[](const size_type index) noexcept
	{
		assert(index < _length);
		return _data[index];
	}

	template <typename T>
	typename vector<T>::const_reference vector<T>::operator[](const size_type index) const noexcept
	{
		assert(index < _length);
		return _data[index];
	}

	template <typename T>
	typename vector<T>::reference vector<T>::at(size_type index)
	{
		if (index >= _length)
			throw std::out_of_range("vector index out of range");
		return _data[index];
	}

	template <typename T>
	typename vector<T>::const_reference vector<T>::at(size_type index) const
	{
		if (index >= _length)
			throw std::out_of_range("vector index out of range");
		return _data[index];
	}

	template <typename T>
	typename vector<T>::reference vector<T>::front() noexcept
	{
		assert(_length >= 1);
		return _data[0];
	}

	template <typename T>
	typename vector<T>::const_reference vector<T>::front() const noexcept
	{
		assert(_length >= 1);
		return _data[0];
	}

	template <typename T>
	typename vector<T>::reference vector<T>::back() noexcept
	{
		assert(_length >= 1);
		return _data[_length - 1];
	}

	template <typename T>
	typename vector<T>::const_reference vector<T>::back() const noexcept
	{
		assert(_length >= 1);
		return _data[_length - 1];
	}

	template <typename T>
	typename vector<T>::iterator vector<T>::data() noexcept { return _data; }

	template <typename T>
	typename vector<T>::const_iterator vector<T>::data() const noexcept { return _data; }

	// --- Modifiers ---

	template <typename T>
	void vector<T>::push_back(const_reference element)
	{
		if (_length == _capacity)
		{
			if (&element >= _data && &element < _data + _length) {
				auto index{ &element - _data };
				reserve(_capacity ? _capacity * 2 : 1);
				std::construct_at(_data + _length, _data[index]);
			}
			else {
				reserve(_capacity ? _capacity * 2 : 1);
				std::construct_at(_data + _length, element);
			}
		}
		else
		{
			std::construct_at(_data + _length, element);
		}


		++_length;
	}

	template <typename T>
	void vector<T>::push_back(T&& element)
	{
		if (_length == _capacity)
		{
			if (&element >= _data && &element < _data + _length)
			{
				auto index{ &element - _data };
				reserve(_capacity ? _capacity * 2 : 1);
				std::construct_at(_data + _length, std::move(_data[index]));
			}
			else
			{
				reserve(_capacity ? _capacity * 2 : 1);
				std::construct_at(_data + _length, std::move(element));
			}
		}
		else
		{
			std::construct_at(_data + _length, std::move(element));
		}

		++_length;
	}

	template<typename T>
	typename vector<T>::iterator vector<T>::insert(const_iterator pos, const_reference value) {
		if (pos < cbegin() || pos > cend()) {
			throw std::out_of_range("Iterator out of bounds");
		}

		if (&value >= _data && &value < _data + _length) {
			value_type temp = value;
			return insert(pos, std::move(temp));
		}

		auto index = static_cast<size_type>(pos - cbegin());


		if (index == _length) {
			push_back(value);
			return begin() + index;
		}

		if (_length == _capacity) {
			reserve(_capacity ? _capacity * 2 : 1);
		}

		// Last elem needs special handling because at _data + _length, 
		// it is raw memory we can not assign there because there is no object to call assignment operator.
		std::construct_at(_data + _length, std::move(_data[_length - 1]));
		for (size_type i = _length - 1; i > index; --i) {
			_data[i] = std::move(_data[i - 1]);
		}

		_data[index] = value;
		++_length;

		return begin() + index;
	}

	template<typename T>
	typename vector<T>::iterator vector<T>::insert(const_iterator pos, T&& value) {
		if (pos < cbegin() || pos > cend()) {
			throw std::out_of_range("Iterator out of bounds");
		}

		if (&value >= _data && &value < _data + _length) {
			value_type temp{ std::move(value) };
			return insert(pos, std::move(temp));
		}

		auto index{ static_cast<size_type>(pos - cbegin()) };

		if (index == _length) {
			push_back(std::move(value));
			return begin() + index;
		}

		if (_length == _capacity) {
			reserve(_capacity ? _capacity * 2 : 1);
		}

		std::construct_at(_data + _length, std::move(_data[_length - 1]));
		for (size_type i = _length - 1; i > index; --i) {
			_data[i] = std::move(_data[i - 1]);
		}

		_data[index] = std::move(value);
		++_length;

		return begin() + index;
	}

	template<typename T>
	typename vector<T>::iterator vector<T>::insert(const_iterator pos, size_type count, const_reference value) {
		if (pos < cbegin() || pos > cend()) {
			throw std::out_of_range("Iterator out of bounds");
		}

		if (&value >= _data && &value < _data + _length) {
			value_type temp = value;
			return insert(pos, count, temp);
		}

		if (count == 0) {
			return begin() + (pos - cbegin());
		}

		if (pos == cend()) {
			for (size_type i{}; i < count; ++i) {
				push_back(value);
			}
			return end() - count;
		}

		// We want to store index in case reallocation happen in reserve.
		auto insert_index{ pos - cbegin() };

		if (_length + count > _capacity) {
			reserve(std::max(_length + count, _capacity * 2));
		}

		auto insert_pos{ _data + insert_index };

		auto old_end{ _data + _length - 1 };
		auto new_end{ _data + _length + count - 1 };

		auto des{ new_end };
		auto src{ old_end };


		while (src >= insert_pos) {
			if (des > old_end) {
				std::construct_at(des, std::move(*src));
			}
			else {
				*des = std::move(*src);
			}
			--des;
			--src;
		}

		while (des >= insert_pos) {
			if (des > old_end) {
				std::construct_at(des, value);
			}
			else {
				*des = value;
			}
			--des;
		}

		_length += count;

		return begin() + insert_index;
	}

	template<typename T>
	typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
		if (pos < cbegin() || pos >= cend()) {
			throw std::out_of_range{ "Iterator is out of bounds" };
		}

		auto index{ pos - cbegin() };

		for (difference_type i{ index }; i < static_cast<difference_type>(_length) - 1; ++i) {
			_data[i] = std::move(_data[i + 1]);
		}

		_data[_length - 1].~T();

		--_length;

		return begin() + index;
	}

	template<typename T>
	template<typename ...Args>
	typename vector<T>::reference vector<T>::emplace_back(Args&& ...args)
	{
		if (_length == _capacity)
		{
			reserve(_capacity ? _capacity * 2 : 1);
		}
		std::construct_at(_data + _length, std::forward<Args>(args)...);
		return _data[_length++];
	}

	template <typename T>
	void vector<T>::pop_back() noexcept
	{
		assert(_length >= 1);
		std::destroy_at(_data + _length - 1);
		--_length;
	}

	template <typename T>
	void vector<T>::clear() noexcept
	{
		std::destroy(_data, _data + _length);
		_length = 0;
	}

	// --- Private Helpers ---

	template<typename T>
	void vector<T>::_reallocate(size_type size)
	{
		auto* new_mem{ static_cast<T*>(::operator new(size * sizeof(T))) };
		size_type i{};

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
		_capacity = size;
	}

} // namespace nstd

#endif // NSTD_VECTOR_HPP