#ifndef NSTD_LIST_HPP
#define NSTD_LIST_HPP

#include <cassert>
#include <cstddef>
#include <utility>

namespace nstd {
template<typename T> class list {
    struct Node {
        T val{};
        Node* prev{};
        Node* next{};
    };

public:
    class iterator;
    class const_iterator;

    list() = default;

    list(const list& other);
    list(list&& other) noexcept;
    list& operator=(list other);
    ~list();

    void push_front(const T& value);
    void push_front(T&& value);
    void push_back(const T& value);
    void push_back(T&& value);
    void pop_front();
    void pop_back();
    T& front();
    const T& front() const;
    T& back();
    const T& back() const;
    bool empty() const noexcept;
    size_t size() const noexcept;
    void clear();
    void insert(size_t pos, const T& value);
    void erase(size_t pos);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    friend void swap(list& first, list& second) noexcept {
        std::swap(first.head, second.head);
        std::swap(first.tail, second.tail);
        std::swap(first._size, second._size);
    }

private:
    Node* head{};
    Node* tail{};
    size_t _size{};
};

template<typename T> class list<T>::iterator {
public:
    iterator() = default;
    iterator(Node* node) : _node{node} {}

    T& operator*() {
        return _node->val;
    }

    T* operator->() {
        return &_node->val;
    }

    iterator& operator++() {
        _node = _node->next;
        return *this;
    }

    iterator operator++(int) {
        iterator temp{*this};
        ++(*this);
        return temp;
    }

    iterator& operator--() {
        _node = _node->prev;
        return *this;
    }

    iterator operator--(int) {
        auto temp{*this};
        --(*this);
        return temp;
    }

    bool operator==(const iterator& other) const = default;

private:
    Node* _node{};
};

template<typename T> class list<T>::const_iterator {
public:
    const_iterator() = default;
    const_iterator(Node* node) : _node{node} {}

    const T& operator*() const {
        return _node->val;
    }

    const T* operator->() const {
        return &_node->val;
    }

    const_iterator& operator++() {
        _node = _node->next;
        return *this;
    }

    const_iterator operator++(int) {
        auto temp{*this};
        ++(*this);
        return temp;
    }

    const_iterator& operator--() {
        _node = _node->prev;
        return *this;
    }

    const_iterator operator--(int) {
        auto temp{*this};
        --(*this);
        return temp;
    }

    bool operator==(const const_iterator& other) const = default;

private:
    Node* _node{};
};

template<typename T> list<T>::list(const list& other) : head(nullptr), tail(nullptr) {
    if (other.empty()) {
        return;
    }

    auto* cur{other.head};
    while (cur) {
        push_back(cur->val);
        cur = cur->next;
    }
}

template<typename T>
list<T>::list(list&& other) noexcept : head(other.head), tail(other.tail), _size(other._size) {
    other.head = nullptr;
    other.tail = nullptr;
    other._size = 0;
}

template<typename T> list<T>& list<T>::operator=(list other) {
    swap(*this, other);
    return *this;
}

template<typename T> list<T>::~list() {
    clear();
}

template<typename T> void list<T>::push_front(const T& value) {
    auto* new_node{new Node{value, nullptr, nullptr}};

    if (head == nullptr) {
        head = new_node;
        tail = new_node;
        ++_size;
        return;
    }

    new_node->next = head;
    head->prev = new_node;
    head = new_node;
    ++_size;
}

template<typename T> void list<T>::push_front(T&& value) {
    auto* new_node{new Node{std::move(value), nullptr, nullptr}};

    if (head == nullptr) {
        head = new_node;
        tail = new_node;
        ++_size;
        return;
    }

    new_node->next = head;
    head->prev = new_node;
    head = new_node;
    ++_size;
}

template<typename T> void list<T>::push_back(const T& value) {
    auto* new_node{new Node{value, nullptr, nullptr}};

    if (head == nullptr) {
        head = new_node;
        tail = new_node;
        ++_size;
        return;
    }

    tail->next = new_node;
    new_node->prev = tail;
    tail = new_node;
    ++_size;
}

template<typename T> void list<T>::push_back(T&& value) {
    auto* new_node{new Node{std::move(value), nullptr, nullptr}};

    if (head == nullptr) {
        head = new_node;
        tail = new_node;
        ++_size;
        return;
    }

    tail->next = new_node;
    new_node->prev = tail;
    tail = new_node;
    ++_size;
}

template<typename T> void list<T>::pop_front() {
    if (!head) {
        return;
    }

    auto* temp{head};

    if (head == tail) {
        head = nullptr;
        tail = nullptr;
    } else {
        head = head->next;
        head->prev = nullptr;
    }

    delete temp;
    --_size;
}

template<typename T> void list<T>::pop_back() {
    if (!head) {
        return;
    }

    auto* temp{tail};

    if (head == tail) {
        head = nullptr;
        tail = nullptr;
    } else {
        tail = tail->prev;
        tail->next = nullptr;
    }

    delete temp;
    --_size;
}

template<typename T> T& list<T>::front() {
    assert(head);
    return head->val;
}

template<typename T> const T& list<T>::front() const {
    assert(head);
    return head->val;
}

template<typename T> T& list<T>::back() {
    assert(tail);
    return tail->val;
}

template<typename T> const T& list<T>::back() const {
    assert(tail);
    return tail->val;
}

template<typename T> bool list<T>::empty() const noexcept {
    return _size == 0;
}

template<typename T> size_t list<T>::size() const noexcept {
    return _size;
}

template<typename T> void list<T>::clear() {

    while (head) {
        auto* temp{head};
        head = head->next;
        delete temp;
    }

    head = nullptr;
    tail = nullptr;
    _size = 0;
}

template<typename T> void list<T>::insert(size_t pos, const T& value) {
    assert(pos <= _size);

    if (pos == 0) {
        push_front(value);
        return;
    }

    if (pos == _size) {
        push_back(value);
        return;
    }

    auto* cur{head};
    for (size_t i{}; i < pos - 1; ++i) {
        cur = cur->next;
    }

    auto* new_node{new Node{value, nullptr, nullptr}};

    new_node->next = cur->next;
    new_node->prev = cur;

    cur->next = new_node;
    new_node->next->prev = new_node;

    ++_size;
}

template<typename T> void list<T>::erase(size_t pos) {
    assert(pos < _size);

    if (pos == 0) {
        pop_front();
        return;
    }

    if (pos == _size - 1) {
        pop_back();
        return;
    }

    auto* cur{head};
    for (size_t i{}; i < pos; ++i) {
        cur = cur->next;
    }

    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;

    delete cur;
    --_size;
}

template<typename T> typename list<T>::iterator list<T>::begin() {
    return iterator(head);
}

template<typename T> typename list<T>::iterator list<T>::end() {
    return iterator(nullptr);
}

template<typename T> typename list<T>::const_iterator list<T>::begin() const {
    return const_iterator(head);
}

template<typename T> typename list<T>::const_iterator list<T>::end() const {
    return const_iterator(nullptr);
}

template<typename T> typename list<T>::const_iterator list<T>::cbegin() const {
    return const_iterator(head);
}

template<typename T> typename list<T>::const_iterator list<T>::cend() const {
    return const_iterator(nullptr);
}

} // namespace nstd

#endif // NSTD_LIST_HPP