#ifndef TESTS_STACK_HPP
#define TESTS_STACK_HPP

#include "nstd/stack.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <utility>

namespace tests {
namespace stack {
// ------------------------- Basic Push / Pop / Top -------------------------
void test_basic_operations() {
    std::cout << "--- Running Basic Operations Test ---\n";

    nstd::stack<int> s;
    assert(s.empty());
    assert(s.size() == 0);

    s.push(1);
    s.push(2);
    s.push(3);

    assert(s.size() == 3);
    assert(!s.empty());
    assert(s.top() == 3);

    int popped = s.pop();
    assert(popped == 3);
    assert(s.size() == 2);
    assert(s.top() == 2);

    popped = s.pop();
    assert(popped == 2);
    assert(s.size() == 1);
    assert(s.top() == 1);

    popped = s.pop();
    assert(popped == 1);
    assert(s.empty());
    assert(s.size() == 0);
}

// ------------------------- Const Top -------------------------
void test_const_top() {
    std::cout << "--- Running Const Top Test ---\n";

    nstd::stack<int> s;
    s.push(42);
    const nstd::stack<int>& cs = s;
    assert(cs.top() == 42);
    assert(cs.size() == 1);
    assert(!cs.empty());
}

// ------------------------- Copy / Move Semantics -------------------------
void test_copy_move() {
    std::cout << "--- Running Copy / Move Test ---\n";

    nstd::stack<int> s1;
    for (int i = 0; i < 5; ++i)
        s1.push(i);

    // Copy constructor
    nstd::stack<int> s2(s1);
    assert(s2.size() == s1.size());
    while (!s1.empty() && !s2.empty()) {
        assert(s1.top() == s2.top());
        s1.pop();
        s2.pop();
    }
    assert(s1.empty() && s2.empty());

    // Reset
    for (int i = 0; i < 5; ++i)
        s1.push(i);

    // Copy assignment
    nstd::stack<int> s3;
    s3 = s1;
    assert(s3.size() == s1.size());
    while (!s1.empty() && !s3.empty()) {
        assert(s1.top() == s3.top());
        s1.pop();
        s3.pop();
    }
    assert(s1.empty() && s3.empty());

    // Reset
    for (int i = 0; i < 5; ++i)
        s1.push(i);

    // Move constructor
    nstd::stack<int> s4(std::move(s1));
    assert(s4.size() == 5);
    assert(s1.empty()); // moved-from should be empty

    // Move assignment
    nstd::stack<int> s5;
    s5 = std::move(s4);
    assert(s5.size() == 5);
    assert(s4.empty());
}

// ------------------------- Multiple Types Test -------------------------
void test_multiple_types() {
    std::cout << "--- Running Multiple Types Test ---\n";

    nstd::stack<double> sd;
    sd.push(3.14);
    sd.push(2.718);
    assert(sd.top() == 2.718);
    sd.pop();
    assert(sd.top() == 3.14);

    nstd::stack<std::string> ss;
    ss.push("Hello");
    ss.push("World");
    assert(ss.top() == "World");
    ss.pop();
    assert(ss.top() == "Hello");

    nstd::stack<std::pair<int, int>> sp;
    sp.push({1, 2});
    sp.push({3, 4});
    assert(sp.top().first == 3 && sp.top().second == 4);
    sp.pop();
    assert(sp.top().first == 1 && sp.top().second == 2);
}

// ------------------------- Stress Test -------------------------
void test_stress() {
    std::cout << "--- Running Stress Test ---\n";

    nstd::stack<int> s;
    const int N = 100000;
    for (int i = 0; i < N; ++i)
        s.push(i);

    assert(s.size() == N);
    assert(s.top() == N - 1);

    long long sum = 0;
    while (!s.empty()) {
        sum += s.top();
        s.pop();
    }

    long long expected = (N - 1LL) * N / 2;
    assert(sum == expected);
    assert(s.empty());
}
} // namespace stack
} // namespace tests

#endif // TESTS_STACK_HPP