#ifndef TESTS_VECTOR_ENHANCED_HPP
#define TESTS_VECTOR_ENHANCED_HPP

#include "nstd/vector.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace tests {
namespace vector {
// ==========================================
// HELPER: Memory Tracker Object
// ==========================================
struct Obj {
    static int alive_count;
    static int construction_count;
    static int destruction_count;
    static int move_count;

    int value;

    Obj(int v = 0) : value(v) {
        ++alive_count;
        ++construction_count;
    }

    Obj(const Obj& other) : value(other.value) {
        ++alive_count;
        ++construction_count;
    }

    Obj(Obj&& other) noexcept : value(other.value) {
        other.value = -1;
        ++alive_count;
        ++construction_count;
        ++move_count;
    }

    Obj& operator=(const Obj& other) {
        value = other.value;
        return *this;
    }

    Obj& operator=(Obj&& other) noexcept {
        value = other.value;
        other.value = -1;
        ++move_count;
        return *this;
    }

    ~Obj() {
        if (value != -999) {
            --alive_count;
            ++destruction_count;
        }
        value = -999;
    }

    auto operator<=>(const Obj&) const = default;

    static void reset_stats() {
        construction_count = 0;
        destruction_count = 0;
        move_count = 0;
    }

    static void verify_no_leaks() {
        if (alive_count != 0) {
            std::cerr << "MEMORY LEAK DETECTED! Alive objects: " << alive_count << "\n";
            std::terminate();
        }
    }
};

int Obj::alive_count = 0;
int Obj::construction_count = 0;
int Obj::destruction_count = 0;
int Obj::move_count = 0;

// ==========================================
// HELPER: Non-copyable, movable-only type
// ==========================================
struct MoveOnly {
    int value;

    MoveOnly(int v = 0) : value(v) {}
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&& other) noexcept : value(other.value) {
        other.value = -1;
    }

    MoveOnly& operator=(MoveOnly&& other) noexcept {
        value = other.value;
        other.value = -1;
        return *this;
    }

    bool operator==(const MoveOnly& other) const {
        return value == other.value;
    }
};

// ==========================================
// BASIC TESTS (Enhanced)
// ==========================================

void test_constructors_and_destructors() {
    std::cout << "[Test] Constructors & Destructors... ";
    Obj::reset_stats();

    {
        // 1. Default
        nstd::vector<Obj> v1;
        assert(v1.is_empty());
        assert(v1.size() == 0);
        assert(v1.data() == nullptr);

        // 2. Initializer List (empty)
        nstd::vector<Obj> v_empty = {};
        assert(v_empty.is_empty());
        assert(v_empty.size() == 0);

        // 3. Initializer List (single element)
        nstd::vector<Obj> v_single = {Obj(42)};
        assert(v_single.size() == 1);
        assert(v_single[0].value == 42);

        // 4. Initializer List (multiple)
        nstd::vector<Obj> v2 = {Obj(1), Obj(2), Obj(3)};
        assert(v2.size() == 3);
        assert(v2[0].value == 1);

        // 5. Count/Value (zero count)
        nstd::vector<Obj> v_zero(0, Obj(99));
        assert(v_zero.is_empty());

        // 6. Count/Value (normal)
        nstd::vector<Obj> v3(5, Obj(42));
        assert(v3.size() == 5);
        assert(v3[4].value == 42);

        // 7. Range (empty range)
        std::vector<Obj> empty_source;
        nstd::vector<Obj> v_empty_range(empty_source.begin(), empty_source.end());
        assert(v_empty_range.is_empty());

        // 8. Range (normal)
        std::vector<Obj> source;
        source.push_back(Obj(10));
        source.push_back(Obj(20));
        nstd::vector<Obj> v4(source.begin(), source.end());
        assert(v4.size() == 2);
        assert(v4[1].value == 20);

        // 9. Copy Constructor (empty)
        nstd::vector<Obj> v_copy_empty(v1);
        assert(v_copy_empty.is_empty());

        // 10. Copy Constructor (normal)
        nstd::vector<Obj> v5(v2);
        assert(v5.size() == 3);
        assert(v5[0].value == 1);
        assert(v2.size() == 3); // Original unchanged

        // 11. Move Constructor
        nstd::vector<Obj> v_to_move;
        v_to_move.push_back(Obj(100));
        nstd::vector<Obj> v_moved(std::move(v_to_move));
        assert(v_moved.size() == 1);
        assert(v_moved[0].value == 100);
        assert(v_to_move.size() == 0);
        assert(v_to_move.data() == nullptr);
    }

    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

void test_assignment() {
    std::cout << "[Test] Assignment Operators... ";

    // 1. Copy Assignment (empty to empty)
    {
        nstd::vector<int> v1;
        nstd::vector<int> v2;
        v2 = v1;
        assert(v2.is_empty());
    }

    // 2. Copy Assignment (normal to empty)
    {
        nstd::vector<int> v1 = {1, 2, 3};
        nstd::vector<int> v2;
        v2 = v1;
        assert(v2.size() == 3);
        assert(v2[0] == 1 && v2[2] == 3);
    }

    // 3. Copy Assignment (empty to normal)
    {
        nstd::vector<int> v1;
        nstd::vector<int> v2 = {1, 2, 3};
        v2 = v1;
        assert(v2.is_empty());
    }

    // 4. Copy Assignment (normal to normal, different sizes)
    {
        nstd::vector<int> v1 = {1, 2, 3, 4, 5};
        nstd::vector<int> v2 = {10, 20};
        v2 = v1;
        assert(v2.size() == 5);
        assert(v2[0] == 1 && v2[4] == 5);
    }

    // 5. Self assignment
    {
        nstd::vector<int> v = {1, 2, 3};
        v = v;
        assert(v.size() == 3);
        assert(v[0] == 1 && v[2] == 3);
    }

    // 6. Move Assignment (empty)
    {
        nstd::vector<Obj> v1;
        nstd::vector<Obj> v2;
        v2 = std::move(v1);
        assert(v2.is_empty());
        assert(v1.is_empty());
    }

    // 7. Move Assignment (normal)
    {
        nstd::vector<Obj> v1;
        v1.push_back(Obj(100));
        nstd::vector<Obj> v2;
        v2 = std::move(v1);
        assert(v2.size() == 1);
        assert(v2[0].value == 100);
        assert(v1.size() == 0);
        assert(v1.data() == nullptr);
    }

    // 8. Initializer list assignment
    {
        nstd::vector<int> v;
        v = {5, 10, 15};
        assert(v.size() == 3);
        assert(v[1] == 10);

        v = {};
        assert(v.is_empty());
    }

    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

void test_access_and_capacity() {
    std::cout << "[Test] Access & Capacity... ";

    // Basic access
    nstd::vector<int> v = {10, 20, 30};
    assert(v.front() == 10);
    assert(v.back() == 30);
    assert(v[1] == 20);
    assert(v.at(2) == 30);

    // Const access
    const nstd::vector<int>& cv = v;
    assert(cv.front() == 10);
    assert(cv.back() == 30);
    assert(cv[1] == 20);
    assert(cv.at(2) == 30);

    // Out of range
    try {
        v.at(3);
        assert(false && "Should have thrown out_of_range");
    } catch (const std::out_of_range&) {
    }

    try {
        cv.at(100);
        assert(false && "Should have thrown out_of_range");
    } catch (const std::out_of_range&) {
    }

    // Modifiable access
    v[0] = 99;
    assert(v.front() == 99);
    v.at(2) = 88;
    assert(v.back() == 88);

    // Reserve
    size_t old_cap = v.get_capacity();
    v.reserve(100);
    assert(v.get_capacity() >= 100);
    assert(v.size() == 3);
    assert(v[0] == 99);

    // Reserve with smaller capacity (should do nothing)
    v.reserve(5);
    assert(v.get_capacity() >= 100);

    // Reserve with current capacity (should do nothing)
    size_t cap = v.get_capacity();
    v.reserve(cap);
    assert(v.get_capacity() == cap);

    // Shrink to fit
    v.shrink_to_fit();
    assert(v.get_capacity() == 3);
    assert(v.size() == 3);

    // Shrink to fit when size == capacity (should do nothing)
    v.shrink_to_fit();
    assert(v.get_capacity() == 3);

    // Empty vector operations
    nstd::vector<int> empty;
    empty.reserve(10);
    assert(empty.get_capacity() >= 10);
    assert(empty.is_empty());
    empty.shrink_to_fit();
    assert(empty.get_capacity() == 0);

    std::cout << "PASSED\n";
}

void test_modifiers_basic() {
    std::cout << "[Test] Push/Pop/Clear/Emplace... ";

    {
        nstd::vector<Obj> v;

        // Push back copy
        Obj o1(1);
        v.push_back(o1);
        assert(v.size() == 1);
        assert(v[0].value == 1);
        assert(o1.value == 1); // Original unchanged

        // Push back move
        v.push_back(Obj(2));
        assert(v.size() == 2);
        assert(v[1].value == 2);

        // Emplace back
        v.emplace_back(3);
        assert(v.size() == 3);
        assert(v[2].value == 3);

        // Multiple pushes (test growth)
        for (int i = 4; i <= 100; ++i) {
            v.push_back(Obj(i));
        }
        assert(v.size() == 100);
        assert(v[99].value == 100);

        // Pop back
        v.pop_back();
        assert(v.size() == 99);
        assert(v.back().value == 99);

        // Pop until empty
        while (!v.is_empty()) {
            v.pop_back();
        }
        assert(v.is_empty());
        assert(v.get_capacity() > 0); // Capacity preserved

        // Clear
        v.push_back(Obj(1));
        v.push_back(Obj(2));
        size_t cap = v.get_capacity();
        v.clear();
        assert(v.is_empty());
        assert(v.get_capacity() == cap); // Capacity preserved
    }

    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

void test_resize() {
    std::cout << "[Test] Resize... ";
    {
        nstd::vector<Obj> v;

        // Resize from empty to size (with value)
        v.resize(5, Obj(99));
        assert(v.size() == 5);
        assert(v[4].value == 99);
        assert(Obj::alive_count == 5);

        // Resize to larger (with value)
        v.resize(8, Obj(77));
        assert(v.size() == 8);
        assert(v[4].value == 99); // Old elements preserved
        assert(v[7].value == 77); // New elements
        assert(Obj::alive_count == 8);

        // Resize to smaller
        v.resize(3);
        assert(v.size() == 3);
        assert(Obj::alive_count == 3);

        // Resize to zero
        v.resize(0);
        assert(v.is_empty());
        assert(Obj::alive_count == 0);

        // Resize from empty with default
        v.resize(4);
        assert(v.size() == 4);
        assert(v[0].value == 0); // Default constructed
        assert(Obj::alive_count == 4);

        // Resize to same size (should do nothing)
        Obj::reset_stats();
        v.resize(4);
        assert(v.size() == 4);
        assert(Obj::construction_count == 0); // No new constructions
        assert(Obj::destruction_count == 0);  // No destructions
    }
    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

void test_insert_single() {
    std::cout << "[Test] Insert (Single)... ";

    // Insert into empty vector
    {
        nstd::vector<int> v;
        auto it = v.insert(v.cbegin(), 42);
        assert(*it == 42);
        assert(v.size() == 1);
        assert(v[0] == 42);
    }

    // Insert at beginning
    {
        nstd::vector<int> v = {1, 2, 3};
        auto it = v.insert(v.cbegin(), 0);
        assert(*it == 0);
        assert(v.size() == 4);
        assert(v[0] == 0 && v[1] == 1);
    }

    // Insert at end
    {
        nstd::vector<int> v = {1, 2, 3};
        auto it = v.insert(v.cend(), 4);
        assert(*it == 4);
        assert(v.size() == 4);
        assert(v.back() == 4);
    }

    // Insert in middle
    {
        nstd::vector<int> v = {1, 2, 4};
        auto it = v.insert(v.cbegin() + 2, 3);
        assert(*it == 3);
        assert(v.size() == 4);
        assert(v[0] == 1 && v[1] == 2 && v[2] == 3 && v[3] == 4);
    }

    // Insert with reallocation
    {
        nstd::vector<int> v;
        v.reserve(2);
        v.push_back(1);
        v.push_back(2);
        auto it = v.insert(v.cbegin() + 1, 99);
        assert(*it == 99);
        assert(v[0] == 1 && v[1] == 99 && v[2] == 2);
    }

    // Insert move-only type
    {
        nstd::vector<MoveOnly> v;
        v.push_back(MoveOnly(1));
        v.push_back(MoveOnly(3));
        auto it = v.insert(v.cbegin() + 1, MoveOnly(2));
        assert(it->value == 2);
        assert(v.size() == 3);
        assert(v[0].value == 1 && v[1].value == 2 && v[2].value == 3);
    }

    std::cout << "PASSED\n";
}

void test_insert_count() {
    std::cout << "[Test] Insert (Count)... ";

    // Insert zero count (should do nothing)
    {
        nstd::vector<int> v = {1, 2, 3};
        auto it = v.insert(v.cbegin() + 1, 0, 99);
        assert(it == v.begin() + 1);
        assert(v.size() == 3);
        assert(v[0] == 1 && v[1] == 2 && v[2] == 3);
    }

    // Insert into empty
    {
        nstd::vector<int> v;
        auto it = v.insert(v.cbegin(), 3, 42);
        assert(*it == 42);
        assert(v.size() == 3);
        assert(v[0] == 42 && v[1] == 42 && v[2] == 42);
    }

    // Insert at beginning
    {
        nstd::vector<int> v = {1, 2};
        auto it = v.insert(v.cbegin(), 2, 0);
        assert(*it == 0);
        assert(v.size() == 4);
        assert(v[0] == 0 && v[1] == 0 && v[2] == 1 && v[3] == 2);
    }

    // Insert at end
    {
        nstd::vector<int> v = {1, 2};
        auto it = v.insert(v.cend(), 2, 99);
        assert(*it == 99);
        assert(v.size() == 4);
        assert(v[2] == 99 && v[3] == 99);
    }

    // Insert in middle
    {
        nstd::vector<int> v = {10, 20};
        auto it = v.insert(v.cbegin() + 1, 3, 5);
        assert(*it == 5);
        assert(v.size() == 5);
        assert(v[0] == 10);
        assert(v[1] == 5 && v[2] == 5 && v[3] == 5);
        assert(v[4] == 20);
    }

    // Insert large count
    {
        nstd::vector<int> v = {1};
        v.insert(v.cbegin(), 1000, 7);
        assert(v.size() == 1001);
        assert(v[0] == 7);
        assert(v[999] == 7);
        assert(v[1000] == 1);
    }

    std::cout << "PASSED\n";
}

// void test_insert_range()
//{
//    std::cout << "[Test] Insert (Range)... ";

//    // Insert empty range
//    {
//        nstd::vector<int> v = { 1, 2, 3 };
//        std::vector<int> empty;
//        auto it = v.insert(v.cbegin() + 1, empty.begin(), empty.end());
//        assert(it == v.begin() + 1);
//        assert(v.size() == 3);
//    }

//    // Insert range into empty
//    {
//        nstd::vector<int> v;
//        std::vector<int> src = { 1, 2, 3 };
//        auto it = v.insert(v.cbegin(), src.begin(), src.end());
//        assert(*it == 1);
//        assert(v.size() == 3);
//        assert(v[0] == 1 && v[2] == 3);
//    }

//    // Insert range at beginning
//    {
//        nstd::vector<int> v = { 4, 5 };
//        std::vector<int> src = { 1, 2, 3 };
//        auto it = v.insert(v.cbegin(), src.begin(), src.end());
//        assert(*it == 1);
//        assert(v.size() == 5);
//        assert(v[0] == 1 && v[2] == 3 && v[3] == 4);
//    }

//    // Insert range at end
//    {
//        nstd::vector<int> v = { 1, 2 };
//        std::vector<int> src = { 3, 4, 5 };
//        auto it = v.insert(v.cend(), src.begin(), src.end());
//        assert(*it == 3);
//        assert(v.size() == 5);
//        assert(v[4] == 5);
//    }

//    // Insert range in middle
//    {
//        nstd::vector<int> v = { 1, 5 };
//        std::vector<int> src = { 2, 3, 4 };
//        auto it = v.insert(v.cbegin() + 1, src.begin(), src.end());
//        assert(*it == 2);
//        assert(v.size() == 5);
//        assert(v[0] == 1 && v[1] == 2 && v[2] == 3 && v[3] == 4 && v[4] == 5);
//    }

//    // Insert partial range
//    {
//        nstd::vector<int> v = { 1, 5 };
//        std::vector<int> src = { 2, 3, 4, 99, 100 };
//        auto it = v.insert(v.cbegin() + 1, src.begin(), src.begin() + 3);
//        assert(*it == 2);
//        assert(v.size() == 5);
//        assert(v[3] == 4 && v[4] == 5);
//    }

//    std::cout << "PASSED\n";
//}

void test_insert_aliasing() {
    std::cout << "[Test] Insert (Aliasing Safety)... ";

    // Push_back with self-reference
    {
        nstd::vector<int> v = {10, 20, 30};
        v.reserve(3);
        v.push_back(v[0]);
        assert(v.size() == 4);
        assert(v[3] == 10);
    }

    // Insert single with self-reference at beginning
    {
        nstd::vector<int> v = {1, 2, 3, 4};
        v.insert(v.cbegin(), v[2]);
        assert(v[0] == 3);
        assert(v[3] == 3);
    }

    // Insert single with self-reference in middle
    {
        nstd::vector<int> v = {1, 2, 3, 4};
        v.insert(v.cbegin() + 1, v[2]);
        assert(v[1] == 3);
        assert(v[3] == 3);
    }

    // Insert count with self-reference
    {
        nstd::vector<int> v = {5, 6};
        v.insert(v.cbegin(), 2, v[1]);
        assert(v[0] == 6 && v[1] == 6);
        assert(v[2] == 5 && v[3] == 6);
    }

    // Insert count with self-reference causing reallocation
    {
        nstd::vector<int> v = {1, 2};
        v.reserve(2);
        v.insert(v.cbegin(), 5, v[0]);
        assert(v.size() == 7);
        assert(v[0] == 1 && v[4] == 1);
        assert(v[5] == 1 && v[6] == 2);
    }

    std::cout << "PASSED\n";
}

void test_erase() {
    std::cout << "[Test] Erase... ";

    // Erase single element at beginning
    {
        nstd::vector<int> v = {1, 2, 3, 4};
        auto it = v.erase(v.cbegin());
        assert(*it == 2);
        assert(v.size() == 3);
        assert(v[0] == 2 && v[2] == 4);
    }

    // Erase single element at end
    {
        nstd::vector<int> v = {1, 2, 3};
        auto it = v.erase(v.cbegin() + 2);
        assert(it == v.end());
        assert(v.size() == 2);
        assert(v[1] == 2);
    }

    // Erase single element in middle
    {
        nstd::vector<int> v = {1, 2, 3, 4};
        auto it = v.erase(v.cbegin() + 1);
        assert(*it == 3);
        assert(v.size() == 3);
        assert(v[0] == 1 && v[1] == 3 && v[2] == 4);
    }

    //// Erase range (empty range)
    //{
    //    nstd::vector<int> v = { 1, 2, 3 };
    //    auto it = v.erase(v.cbegin() + 1, v.cbegin() + 1);
    //    assert(*it == 2);
    //    assert(v.size() == 3);
    //}

    //// Erase range (entire vector)
    //{
    //    nstd::vector<int> v = { 1, 2, 3 };
    //    auto it = v.erase(v.cbegin(), v.cend());
    //    assert(it == v.end());
    //    assert(v.is_empty());
    //}

    //// Erase range at beginning
    //{
    //    nstd::vector<int> v = { 1, 2, 3, 4, 5 };
    //    auto it = v.erase(v.cbegin(), v.cbegin() + 2);
    //    assert(*it == 3);
    //    assert(v.size() == 3);
    //    assert(v[0] == 3);
    //}

    //// Erase range at end
    //{
    //    nstd::vector<int> v = { 1, 2, 3, 4, 5 };
    //    auto it = v.erase(v.cbegin() + 3, v.cend());
    //    assert(it == v.end());
    //    assert(v.size() == 3);
    //    assert(v[2] == 3);
    //}

    //// Erase range in middle
    //{
    //    nstd::vector<int> v = { 1, 2, 3, 4, 5 };
    //    auto it = v.erase(v.cbegin() + 1, v.cbegin() + 4);
    //    assert(*it == 5);
    //    assert(v.size() == 2);
    //    assert(v[0] == 1 && v[1] == 5);
    //}

    // Erase with objects (check destructor calls)
    {
        nstd::vector<Obj> v;
        v.push_back(Obj(1));
        v.push_back(Obj(2));
        v.push_back(Obj(3));
        assert(Obj::alive_count == 3);

        v.erase(v.cbegin() + 1);
        assert(v.size() == 2);
        assert(Obj::alive_count == 2);
        assert(v[0].value == 1 && v[1].value == 3);
    }

    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

void test_iterators() {
    std::cout << "[Test] Iterators... ";

    nstd::vector<int> v = {1, 2, 3, 4, 5};

    // Range-based for loop
    int sum = 0;
    for (int x : v)
        sum += x;
    assert(sum == 15);

    // Iterator arithmetic
    auto it = v.begin();
    assert(*it == 1);
    ++it;
    assert(*it == 2);
    it += 2;
    assert(*it == 4);
    --it;
    assert(*it == 3);
    it -= 1;
    assert(*it == 2);

    // Iterator comparison
    assert(v.begin() < v.end());
    assert(v.begin() <= v.begin());
    assert(v.end() > v.begin());
    assert(v.end() >= v.end());
    assert(v.begin() == v.begin());
    assert(v.begin() != v.end());

    // Distance
    assert(v.end() - v.begin() == 5);

    // Const iterators
    const auto& cv = v;
    assert(*cv.begin() == 1);
    assert(*(cv.end() - 1) == 5);
    assert(cv.cbegin() == cv.begin());
    assert(cv.cend() == cv.end());

    // Reverse iteration
    int expected = 5;
    for (auto rit = v.end() - 1; rit >= v.begin(); --rit) {
        assert(*rit == expected--);
    }

    // Empty vector iterators
    nstd::vector<int> empty;
    assert(empty.begin() == empty.end());
    assert(empty.cbegin() == empty.cend());

    // Iterator modification
    for (auto& x : v) {
        x *= 2;
    }
    assert(v[0] == 2 && v[4] == 10);

    std::cout << "PASSED\n";
}

void test_comparisons() {
    std::cout << "[Test] Comparisons... ";

    nstd::vector<int> v1 = {1, 2, 3};
    nstd::vector<int> v2 = {1, 2, 3};
    nstd::vector<int> v3 = {1, 2, 4};
    nstd::vector<int> v4 = {1, 2};
    nstd::vector<int> v5 = {1, 2, 3, 4};
    nstd::vector<int> empty1;
    nstd::vector<int> empty2;

    // Equality
    assert(v1 == v2);
    assert(v1 != v3);
    assert(v1 != v4);
    assert(empty1 == empty2);
    assert(v1 != empty1);

    // Three-way comparison
    assert((v1 <=> v2) == 0);
    assert((v1 <=> v3) < 0);
    assert((v3 <=> v1) > 0);
    assert((v4 <=> v1) < 0); // Shorter is smaller
    assert((v1 <=> v5) < 0); // Shorter is smaller
    assert((empty1 <=> empty2) == 0);
    assert((empty1 <=> v1) < 0);

    // Less than
    assert(v1 < v3);
    assert(v4 < v1);
    assert(!(v1 < v2));
    assert(empty1 < v1);

    // Greater than
    assert(v3 > v1);
    assert(v1 > v4);
    assert(!(v1 > v2));
    assert(v1 > empty1);

    // Less than or equal
    assert(v1 <= v2);
    assert(v1 <= v3);
    assert(!(v3 <= v1));

    // Greater than or equal
    assert(v1 >= v2);
    assert(v3 >= v1);
    assert(!(v1 >= v3));

    std::cout << "PASSED\n";
}

// ==========================================
// EDGE CASE TESTS
// ==========================================

void test_edge_case_empty_vector() {
    std::cout << "[Test] Edge Case: Empty Vector Operations... ";

    nstd::vector<int> v;

    // Basic queries
    assert(v.is_empty());
    assert(v.size() == 0);
    assert(v.data() == nullptr);
    assert(v.begin() == v.end());

    // Clear empty vector
    v.clear();
    assert(v.is_empty());

    // Reserve on empty
    v.reserve(10);
    assert(v.get_capacity() >= 10);
    assert(v.is_empty());

    // Shrink empty
    v.shrink_to_fit();
    assert(v.get_capacity() == 0);

    // Resize empty to zero
    v.resize(0);
    assert(v.is_empty());

    std::cout << "PASSED\n";
}

void test_edge_case_single_element() {
    std::cout << "[Test] Edge Case: Single Element Vector... ";

    nstd::vector<int> v = {42};

    assert(v.size() == 1);
    assert(v.front() == 42);
    assert(v.back() == 42);
    assert(v[0] == 42);

    // Erase single element
    v.erase(v.cbegin());
    assert(v.is_empty());

    // Rebuild and pop
    v.push_back(99);
    v.pop_back();
    assert(v.is_empty());

    // Rebuild and resize down
    v.push_back(88);
    v.resize(0);
    assert(v.is_empty());

    std::cout << "PASSED\n";
}

void test_edge_case_large_vector() {
    std::cout << "[Test] Edge Case: Large Vector... ";

    nstd::vector<int> v;
    const int N = 10000;

    // Build large vector
    for (int i = 0; i < N; ++i) {
        v.push_back(i);
    }
    assert(v.size() == N);
    assert(v[N - 1] == N - 1);

    // Access all elements
    for (int i = 0; i < N; ++i) {
        assert(v[i] == i);
    }

    // Insert in middle of large vector
    v.insert(v.cbegin() + N / 2, 9999);
    assert(v.size() == N + 1);
    assert(v[N / 2] == 9999);

    // Erase from middle
    v.erase(v.cbegin() + N / 2);
    assert(v.size() == N);

    // Clear large vector
    v.clear();
    assert(v.is_empty());

    std::cout << "PASSED\n";
}

void test_edge_case_reallocation_stress() {
    std::cout << "[Test] Edge Case: Repeated Reallocations... ";

    nstd::vector<Obj> v;

    // Force many reallocations
    for (int i = 0; i < 100; ++i) {
        v.push_back(Obj(i));
    }
    assert(v.size() == 100);

    // Verify all elements
    for (int i = 0; i < 100; ++i) {
        assert(v[i].value == i);
    }

    // Insert causing reallocation
    v.reserve(v.get_capacity()); // Fill to capacity
    size_t cap = v.get_capacity();
    for (size_t i = v.size(); i < cap; ++i) {
        v.push_back(Obj(999));
    }
    v.push_back(Obj(1000)); // Force reallocation
    assert(v.back().value == 1000);

    v.clear();
    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

void test_edge_case_move_only_types() {
    std::cout << "[Test] Edge Case: Move-Only Types... ";

    nstd::vector<MoveOnly> v;

    // Push move-only
    v.push_back(MoveOnly(1));
    v.push_back(MoveOnly(2));
    assert(v.size() == 2);
    assert(v[0].value == 1);

    // Emplace
    v.emplace_back(3);
    assert(v[2].value == 3);

    // Insert
    v.insert(v.cbegin() + 1, MoveOnly(99));
    assert(v[1].value == 99);

    // Resize (default construct)
    v.resize(5);
    assert(v.size() == 5);
    assert(v[4].value == 0);

    // Move constructor
    nstd::vector<MoveOnly> v2(std::move(v));
    assert(v2.size() == 5);
    assert(v.is_empty());

    // Move assignment
    v = std::move(v2);
    assert(v.size() == 5);
    assert(v2.is_empty());

    std::cout << "PASSED\n";
}

void test_edge_case_self_operations() {
    std::cout << "[Test] Edge Case: Self-Operations... ";

    // Self copy assignment
    {
        nstd::vector<int> v = {1, 2, 3};
        v = v;
        assert(v.size() == 3);
        assert(v[0] == 1 && v[2] == 3);
    }

    // Self move assignment (implementation-defined but should be safe)
    {
        nstd::vector<int> v = {1, 2, 3};
        v = std::move(v);
        // After self-move, state is valid but unspecified
        // Just verify no crash
    }

    // Insert from self (covered in aliasing tests but worth emphasizing)
    {
        nstd::vector<int> v = {1, 2, 3};
        v.insert(v.cbegin(), v[1]);
        assert(v[0] == 2);
    }

    std::cout << "PASSED\n";
}

void test_edge_case_capacity_vs_size() {
    std::cout << "[Test] Edge Case: Capacity vs Size Invariants... ";

    nstd::vector<int> v;

    // Size <= Capacity always
    for (int i = 0; i < 50; ++i) {
        v.push_back(i);
        assert(v.size() <= v.get_capacity());
    }

    // Reserve doesn't change size
    size_t old_size = v.size();
    v.reserve(1000);
    assert(v.size() == old_size);
    assert(v.get_capacity() >= 1000);

    // Shrink_to_fit makes capacity == size (ideally)
    v.shrink_to_fit();
    assert(v.get_capacity() == v.size());

    // Clear doesn't change capacity
    size_t cap = v.get_capacity();
    v.clear();
    assert(v.get_capacity() == cap);
    assert(v.size() == 0);

    std::cout << "PASSED\n";
}

void test_edge_case_iterator_invalidation() {
    std::cout << "[Test] Edge Case: Iterator Invalidation Awareness... ";

    nstd::vector<int> v = {1, 2, 3};

    // Save iterator
    auto it = v.begin();
    assert(*it == 1);

    // Push back might invalidate if reallocation occurs
    size_t cap = v.get_capacity();
    if (v.size() == cap) {
        // Will definitely reallocate
        v.push_back(4);
        // Original iterator 'it' is now invalid (undefined behavior to use)
        // We can't safely test this without UB, so just document behavior
    }

    // Insert always invalidates iterators at and after insertion point
    v = {1, 2, 3};
    it = v.begin() + 1;
    v.insert(v.cbegin(), 0);
    // 'it' is now invalid

    // Erase invalidates iterators at and after erase point
    v = {1, 2, 3};
    it = v.begin() + 1;
    v.erase(v.cbegin());
    // 'it' may be invalid

    std::cout << "PASSED\n";
}

void test_edge_case_exception_safety() {
    std::cout << "[Test] Edge Case: Basic Exception Safety... ";

    // Out of range exceptions
    nstd::vector<int> v = {1, 2, 3};

    try {
        v.at(100);
        assert(false);
    } catch (const std::out_of_range&) {
        // Vector should still be valid
        assert(v.size() == 3);
        assert(v[0] == 1);
    }

    // Multiple exceptions
    try {
        v.at(999);
        assert(false);
    } catch (const std::out_of_range&) {
    }

    try {
        const auto& cv = v;
        cv.at(999);
        assert(false);
    } catch (const std::out_of_range&) {
    }

    assert(v.size() == 3);

    std::cout << "PASSED\n";
}

void test_edge_case_const_correctness() {
    std::cout << "[Test] Edge Case: Const Correctness... ";

    const nstd::vector<int> cv = {1, 2, 3};

    // Const access methods
    assert(cv.size() == 3);
    assert(!cv.is_empty());
    assert(cv.front() == 1);
    assert(cv.back() == 3);
    assert(cv[1] == 2);
    assert(cv.at(2) == 3);
    assert(cv.data() != nullptr);
    assert(*cv.data() == 1);

    // Const iterators
    assert(*cv.begin() == 1);
    assert(*cv.cbegin() == 1);
    assert(*(cv.end() - 1) == 3);
    assert(*(cv.cend() - 1) == 3);

    // Range-based for with const
    int sum = 0;
    for (const auto& x : cv) {
        sum += x;
    }
    assert(sum == 6);

    std::cout << "PASSED\n";
}

void test_edge_case_data_pointer() {
    std::cout << "[Test] Edge Case: Data Pointer Behavior... ";

    // Empty vector
    nstd::vector<int> v;
    assert(v.data() == nullptr);

    // After push
    v.push_back(42);
    int* ptr = v.data();
    assert(ptr != nullptr);
    assert(*ptr == 42);
    assert(ptr[0] == 42);

    // Pointer to contiguous storage
    v.push_back(43);
    v.push_back(44);
    assert(v.data()[0] == 42);
    assert(v.data()[1] == 43);
    assert(v.data()[2] == 44);

    // After clear, data might still be non-null (capacity remains)
    v.clear();
    assert(v.data() != nullptr || v.get_capacity() == 0);

    std::cout << "PASSED\n";
}

void test_edge_case_boundary_insert_erase() {
    std::cout << "[Test] Edge Case: Boundary Insert/Erase... ";

    nstd::vector<int> v = {1, 2, 3, 4, 5};

    // Insert at exact begin
    v.insert(v.begin(), -1);
    assert(v.front() == -1);

    // Insert at exact end
    v.insert(v.end(), 6);
    assert(v.back() == 6);

    // Erase at exact begin
    v.erase(v.begin());
    assert(v.front() == 1);

    // Erase at exact end - 1
    v.erase(v.end() - 1);
    assert(v.back() == 5);

    //// Erase range [begin, begin)
    // size_t sz = v.size();
    // v.erase(v.begin(), v.begin());
    // assert(v.size() == sz);

    //// Erase range [end, end)
    // v.erase(v.end(), v.end());
    // assert(v.size() == sz);

    std::cout << "PASSED\n";
}

void test_edge_case_complex_objects() {
    std::cout << "[Test] Edge Case: Complex Objects with Members... ";

    struct Complex {
        std::string s;
        std::vector<int> v;
        int x;

        Complex(std::string str, int val) : s(str), v{val, val * 2}, x(val) {}
    };

    nstd::vector<Complex> vec;
    vec.emplace_back("hello", 1);
    vec.emplace_back("world", 2);

    assert(vec.size() == 2);
    assert(vec[0].s == "hello");
    assert(vec[1].v[1] == 4);

    // Copy
    nstd::vector<Complex> vec2 = vec;
    assert(vec2[0].s == "hello");

    // Move
    nstd::vector<Complex> vec3 = std::move(vec2);
    assert(vec3.size() == 2);
    assert(vec2.is_empty());

    std::cout << "PASSED\n";
}

void test_assign_count_value() {
    std::cout << "[Test] Assign (Count, Value)... ";

    // Assign to empty vector
    {
        nstd::vector<int> v;
        v.assign(5, 42);
        assert(v.size() == 5);
        assert(v[0] == 42 && v[4] == 42);
    }

    // Assign zero count (clear vector)
    {
        nstd::vector<int> v = {1, 2, 3};
        v.assign(0, 99);
        assert(v.is_empty());
        assert(v.get_capacity() == 0);
        assert(v.data() == nullptr);
    }

    // Assign to non-empty (count < current size)
    {
        nstd::vector<int> v = {1, 2, 3, 4, 5};
        v.assign(3, 10);
        assert(v.size() == 3);
        assert(v[0] == 10 && v[1] == 10 && v[2] == 10);
    }

    // Assign to non-empty (count > current size, count <= capacity)
    {
        nstd::vector<int> v;
        v.reserve(10);
        v.assign(3, 5);
        v.assign(7, 20);
        assert(v.size() == 7);
        assert(v[0] == 20 && v[6] == 20);
    }

    // Assign to non-empty (count > capacity, requires reallocation)
    {
        nstd::vector<int> v = {1, 2};
        v.shrink_to_fit();
        size_t old_cap = v.get_capacity();
        v.assign(100, 7);
        assert(v.size() == 100);
        assert(v.get_capacity() >= 100);
        assert(v[0] == 7 && v[99] == 7);
    }

    // Assign with objects (memory tracking)
    {
        nstd::vector<Obj> v;
        v.push_back(Obj(1));
        v.push_back(Obj(2));
        assert(Obj::alive_count == 2);

        v.assign(4, Obj(99));
        assert(v.size() == 4);
        assert(Obj::alive_count == 4);
        assert(v[0].value == 99 && v[3].value == 99);
    }

    // Assign with self-reference (aliasing safety)
    {
        nstd::vector<int> v = {10, 20, 30};
        v.assign(5, v[1]);
        assert(v.size() == 5);
        assert(v[0] == 20 && v[4] == 20);
    }

    // Assign with self-reference requiring reallocation
    {
        nstd::vector<int> v = {5, 6, 7};
        v.shrink_to_fit();
        v.assign(100, v[2]);
        assert(v.size() == 100);
        assert(v[0] == 7 && v[99] == 7);
    }

    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

void test_assign_initializer_list() {
    std::cout << "[Test] Assign (Initializer List)... ";

    // Assign to empty vector
    {
        nstd::vector<int> v;
        v.assign({1, 2, 3, 4, 5});
        assert(v.size() == 5);
        assert(v[0] == 1 && v[4] == 5);
    }

    // Assign empty list (clear vector)
    {
        nstd::vector<int> v = {1, 2, 3};
        v.assign({});
        assert(v.is_empty());
    }

    // Assign to non-empty (smaller list)
    {
        nstd::vector<int> v = {1, 2, 3, 4, 5};
        v.assign({10, 20});
        assert(v.size() == 2);
        assert(v[0] == 10 && v[1] == 20);
    }

    // Assign to non-empty (larger list)
    {
        nstd::vector<int> v = {1, 2};
        v.assign({5, 10, 15, 20, 25});
        assert(v.size() == 5);
        assert(v[0] == 5 && v[4] == 25);
    }

    // Assign with objects
    {
        nstd::vector<Obj> v;
        v.push_back(Obj(1));
        assert(Obj::alive_count == 1);

        v.assign({Obj(10), Obj(20), Obj(30)});
        assert(v.size() == 3);
        assert(Obj::alive_count == 3);
        assert(v[0].value == 10 && v[2].value == 30);
    }

    // Multiple assigns
    {
        nstd::vector<int> v;
        v.assign({1, 2, 3});
        assert(v.size() == 3);
        v.assign({10, 20, 30, 40});
        assert(v.size() == 4);
        v.assign({99});
        assert(v.size() == 1);
        assert(v[0] == 99);
    }

    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

void test_assign_iterator_range() {
    std::cout << "[Test] Assign (Iterator Range)... ";

    // Assign from std::vector (random access iterators)
    {
        nstd::vector<int> v;
        std::vector<int> source = {1, 2, 3, 4, 5};
        v.assign(source.begin(), source.end());
        assert(v.size() == 5);
        assert(v[0] == 1 && v[4] == 5);
    }

    // Assign empty range
    {
        nstd::vector<int> v = {1, 2, 3};
        std::vector<int> empty;
        v.assign(empty.begin(), empty.end());
        assert(v.is_empty());
        assert(v.get_capacity() == 0);
        assert(v.data() == nullptr);
    }

    // Assign partial range
    {
        nstd::vector<int> v;
        std::vector<int> source = {10, 20, 30, 40, 50};
        v.assign(source.begin() + 1, source.begin() + 4);
        assert(v.size() == 3);
        assert(v[0] == 20 && v[1] == 30 && v[2] == 40);
    }

    // Assign to non-empty (smaller range)
    {
        nstd::vector<int> v = {1, 2, 3, 4, 5};
        std::vector<int> source = {10, 20};
        v.assign(source.begin(), source.end());
        assert(v.size() == 2);
        assert(v[0] == 10 && v[1] == 20);
    }

    // Assign to non-empty (larger range, within capacity)
    {
        nstd::vector<int> v;
        v.reserve(10);
        v.assign({1, 2, 3});
        std::vector<int> source = {5, 6, 7, 8, 9};
        v.assign(source.begin(), source.end());
        assert(v.size() == 5);
        assert(v[0] == 5 && v[4] == 9);
    }

    // Assign to non-empty (larger range, requires reallocation)
    {
        nstd::vector<int> v = {1, 2};
        v.shrink_to_fit();
        std::vector<int> source(100, 42);
        v.assign(source.begin(), source.end());
        assert(v.size() == 100);
        assert(v[0] == 42 && v[99] == 42);
    }

    // Assign with objects
    {
        nstd::vector<Obj> v;
        v.push_back(Obj(1));
        std::vector<Obj> source;
        source.push_back(Obj(10));
        source.push_back(Obj(20));
        source.push_back(Obj(30));

        v.assign(source.begin(), source.end());
        assert(v.size() == 3);
        assert(v[0].value == 10 && v[2].value == 30);
    }

    // Assign from array (raw pointers)
    {
        nstd::vector<int> v;
        int arr[] = {100, 200, 300};
        v.assign(arr, arr + 3);
        assert(v.size() == 3);
        assert(v[0] == 100 && v[2] == 300);
    }

    // Assign from another nstd::vector
    {
        nstd::vector<int> v1 = {1, 2, 3, 4};
        nstd::vector<int> v2;
        v2.assign(v1.begin(), v1.end());
        assert(v2.size() == 4);
        assert(v2[0] == 1 && v2[3] == 4);
    }

    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

void test_assign_edge_cases() {
    std::cout << "[Test] Assign Edge Cases... ";

    // Assign count=0 to empty vector
    {
        nstd::vector<int> v;
        v.assign(0, 42);
        assert(v.is_empty());
        assert(v.data() == nullptr);
    }

    // Assign same size (no reallocation)
    {
        nstd::vector<int> v = {1, 2, 3};
        size_t cap = v.get_capacity();
        v.assign(3, 99);
        assert(v.size() == 3);
        assert(v.get_capacity() == cap);
        assert(v[0] == 99 && v[2] == 99);
    }

    // Assign large count
    {
        nstd::vector<int> v;
        v.assign(10000, 7);
        assert(v.size() == 10000);
        assert(v[0] == 7 && v[9999] == 7);
    }

    // Multiple consecutive assigns
    {
        nstd::vector<int> v;
        v.assign(5, 1);
        assert(v.size() == 5);
        v.assign(10, 2);
        assert(v.size() == 10 && v[9] == 2);
        v.assign(3, 3);
        assert(v.size() == 3 && v[2] == 3);
        v.assign(0, 4);
        assert(v.is_empty());
    }

    // Assign after clear
    {
        nstd::vector<int> v = {1, 2, 3};
        v.clear();
        v.assign(4, 99);
        assert(v.size() == 4);
        assert(v[3] == 99);
    }

    // Assign move-only types
    {
        nstd::vector<MoveOnly> v;
        std::vector<MoveOnly> source;
        source.push_back(MoveOnly(1));
        source.push_back(MoveOnly(2));
        v.assign(std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()));
        assert(v.size() == 2);
        assert(v[0].value == 1 && v[1].value == 2);
    }

    // Assign with complex objects
    {
        struct Complex {
            std::string s;
            int x;
            Complex(const std::string& str, int val) : s(str), x(val) {}
        };

        nstd::vector<Complex> v;
        v.assign(3, Complex("test", 42));
        assert(v.size() == 3);
        assert(v[0].s == "test" && v[0].x == 42);
    }

    Obj::verify_no_leaks();
    std::cout << "PASSED\n";
}

// ==========================================
// MAIN RUNNER
// ==========================================
void run_all_tests() {
    std::cout << "==========================================\n";
    std::cout << "  RUNNING EXTENSIVE VECTOR TESTS\n";
    std::cout << "  (Enhanced with Edge Cases)\n";
    std::cout << "==========================================\n\n";

    // Basic functionality
    test_constructors_and_destructors();
    test_assignment();
    test_access_and_capacity();
    test_modifiers_basic();
    test_resize();

    // Assign methods
    test_assign_count_value();
    test_assign_initializer_list();
    test_assign_iterator_range();
    test_assign_edge_cases();

    // Insert/Erase
    test_insert_single();
    test_insert_count();
    //			test_insert_range();
    test_insert_aliasing();
    test_erase();

    // Iteration and comparison
    test_iterators();
    test_comparisons();

    // Edge cases
    std::cout << "\n--- Edge Case Testing ---\n";
    test_edge_case_empty_vector();
    test_edge_case_single_element();
    test_edge_case_large_vector();
    test_edge_case_reallocation_stress();
    test_edge_case_move_only_types();
    test_edge_case_self_operations();
    test_edge_case_capacity_vs_size();
    test_edge_case_iterator_invalidation();
    test_edge_case_exception_safety();
    test_edge_case_const_correctness();
    test_edge_case_data_pointer();
    test_edge_case_boundary_insert_erase();
    test_edge_case_complex_objects();

    std::cout << "\n==========================================\n";
    std::cout << "  ALL TESTS PASSED SUCCESSFULLY!\n";
    std::cout << "  Total: 30 test suites\n";
    std::cout << "==========================================\n";
}
} // namespace vector
} // namespace tests

#endif // TESTS_VECTOR_ENHANCED_HPP
