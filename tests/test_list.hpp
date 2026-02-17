#ifndef TESTS_LIST_HPP
#define TESTS_LIST_HPP

#include "nstd/list.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <utility>

namespace tests {
namespace list {
// ------------------------- Basic Push / Pop / Access -------------------------
void test_basic_operations() {
    std::cout << "--- Running Basic Operations Test ---\n";

    nstd::list<int> l;
    assert(l.size() == 0);
    assert(l.empty());

    // push_back
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    assert(l.size() == 3);
    assert(!l.empty());
    assert(l.front() == 1);
    assert(l.back() == 3);

    // push_front
    l.push_front(0);
    assert(l.size() == 4);
    assert(l.front() == 0);
    assert(l.back() == 3);

    // pop_back
    l.pop_back();
    assert(l.size() == 3);
    assert(l.back() == 2);

    // pop_front
    l.pop_front();
    assert(l.size() == 2);
    assert(l.front() == 1);

    // clear
    l.clear();
    assert(l.size() == 0 && l.empty());
}

// ------------------------- Insert / Erase -------------------------
void test_insert_erase() {
    std::cout << "--- Running Insert / Erase Test ---\n";

    nstd::list<int> l;
    l.push_back(1);
    l.push_back(3);
    l.push_back(4);

    // insert at beginning
    l.insert(0, 0);
    assert(l.size() == 4);
    assert(l.front() == 0);

    // insert in middle
    l.insert(2, 2);
    assert(l.size() == 5);
    // check order: 0,1,2,3,4
    auto it = l.begin();
    assert(*it == 0);
    ++it;
    assert(*it == 1);
    ++it;
    assert(*it == 2);
    ++it;
    assert(*it == 3);
    ++it;
    assert(*it == 4);

    // insert at end
    l.insert(5, 5);
    assert(l.size() == 6);
    assert(l.back() == 5);

    // erase at beginning
    l.erase(0);
    assert(l.size() == 5);
    assert(l.front() == 1);

    // erase in middle
    l.erase(2); // erase 3
    assert(l.size() == 4);
    it = l.begin();
    assert(*it == 1);
    ++it;
    assert(*it == 2);
    ++it;
    assert(*it == 4);
    ++it;
    assert(*it == 5);

    // erase at end
    l.erase(3);
    assert(l.size() == 3);
    assert(l.back() == 4);
}

// ------------------------- Copy / Move Semantics -------------------------
void test_copy_move() {
    std::cout << "--- Running Copy / Move Test ---\n";

    nstd::list<int> l1;
    for (int i = 0; i < 5; ++i)
        l1.push_back(i);

    // Copy constructor
    nstd::list<int> l2(l1);
    assert(l2.size() == l1.size());
    auto it1 = l1.begin();
    auto it2 = l2.begin();
    for (; it1 != l1.end(); ++it1, ++it2)
        assert(*it1 == *it2);

    // Copy assignment
    nstd::list<int> l3;
    l3 = l1;
    assert(l3.size() == l1.size());
    it1 = l1.begin();
    auto it3 = l3.begin();
    for (; it1 != l1.end(); ++it1, ++it3)
        assert(*it1 == *it3);

    // Move constructor
    nstd::list<int> l4(std::move(l1));
    assert(l4.size() == 5);
    assert(l1.size() == 0); // moved-from should be empty

    // Move assignment
    nstd::list<int> l5;
    l5 = std::move(l2);
    assert(l5.size() == 5);
    assert(l2.size() == 0);
}

// ------------------------- Iterators Test -------------------------
void test_iterators() {
    std::cout << "--- Running Iterators Test ---\n";

    nstd::list<int> l;
    for (int i = 0; i < 5; ++i)
        l.push_back(i);

    // Forward iteration
    int expected = 0;
    for (auto it = l.begin(); it != l.end(); ++it) {
        assert(*it == expected++);
    }

    // Backward iteration
    auto it = l.begin();
    for (int i = 0; i < 4; ++i)
        ++it; // move to last element
    assert(*it == 4);
    --it;
    assert(*it == 3);
    --it;
    assert(*it == 2);
    --it;
    assert(*it == 1);
    --it;
    assert(*it == 0);

    // Range-based for
    expected = 0;
    for (const auto& x : l) {
        assert(x == expected++);
    }

    // Const iterators
    const nstd::list<int>& cl = l;
    expected = 0;
    for (auto it = cl.begin(); it != cl.end(); ++it) {
        assert(*it == expected++);
    }
}

// ------------------------- Swap Test -------------------------
void test_swap() {
    std::cout << "--- Running Swap Test ---\n";

    nstd::list<int> l1, l2;
    l1.push_back(1);
    l1.push_back(2);
    l2.push_back(3);
    l2.push_back(4);
    l2.push_back(5);

    swap(l1, l2);

    assert(l1.size() == 3);
    assert(l2.size() == 2);
    assert(l1.front() == 3 && l1.back() == 5);
    assert(l2.front() == 1 && l2.back() == 2);
}

// ------------------------- Edge Cases -------------------------
void test_edge_cases() {
    std::cout << "--- Running Edge Cases Test ---\n";

    nstd::list<int> l;

    // Operations on empty list
    assert(l.empty());
    // pop_front/back on empty should do nothing
    l.pop_front();
    l.pop_back();
    assert(l.size() == 0);

    // Single element
    l.push_back(42);
    assert(l.size() == 1);
    assert(l.front() == 42);
    assert(l.back() == 42);
    l.pop_front();
    assert(l.empty());

    l.push_front(99);
    assert(l.front() == 99);
    l.pop_back();
    assert(l.empty());

    // Insert/erase on single element
    l.push_back(1);
    l.insert(0, 0); // insert at 0
    assert(l.front() == 0 && l.back() == 1);
    l.erase(1); // erase the last
    assert(l.front() == 0 && l.size() == 1);
    l.erase(0);
    assert(l.empty());
}

// ------------------------- Multiple Types Test -------------------------
void test_multiple_types() {
    std::cout << "--- Running Multiple Types Test ---\n";

    // Double
    nstd::list<double> ld;
    ld.push_back(3.14);
    ld.push_back(2.718);
    assert(ld.front() == 3.14 && ld.back() == 2.718);

    // String
    nstd::list<std::string> ls;
    ls.push_back("Hello");
    ls.push_back("World");
    assert(ls.front() == "Hello" && ls.back() == "World");

    // Pair
    nstd::list<std::pair<int, int>> lp;
    lp.push_back({1, 2});
    lp.push_back({3, 4});
    assert(lp.front().first == 1 && lp.front().second == 2);
    assert(lp.back().first == 3 && lp.back().second == 4);
}

// ------------------------- Stress Test -------------------------
void test_stress() {
    std::cout << "--- Running Stress Test ---\n";

    nstd::list<int> l;
    const int N = 10000;
    for (int i = 0; i < N; ++i)
        l.push_back(i);

    assert(l.size() == N);
    assert(l.front() == 0);
    assert(l.back() == N - 1);

    // Sum check
    long long sum = 0;
    for (auto x : l)
        sum += x;
    long long expected = (N - 1LL) * N / 2;
    assert(sum == expected);

    // Clear stress
    l.clear();
    assert(l.empty());
}
} // namespace list
} // namespace tests

#endif // TESTS_LIST_HPP