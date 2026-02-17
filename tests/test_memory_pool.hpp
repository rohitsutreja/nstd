#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "nstd/memory_pool.hpp"

namespace tests {
namespace memory_pool {
// ==========================================
// Test Helpers
// ==========================================

// A helper class to track if constructors/destructors are called
struct Tracker {
    static int alive_count;
    int value;

    Tracker(int v) : value(v) {
        alive_count++;
    }

    ~Tracker() {
        alive_count--;
    }
};
int Tracker::alive_count = 0;

// ==========================================
// Tests
// ==========================================

void test_basic_allocation() {
    std::cout << "[Test] Basic Allocation... ";
    nstd::memory_pool<int> pool;

    int* a = pool.allocate(10);
    int* b = pool.allocate(20);

    assert(*a == 10);
    assert(*b == 20);
    assert(a != b); // Should be different addresses

    pool.deallocate(a);
    pool.deallocate(b);
    std::cout << "Passed.\n";
}

void test_address_reuse() {
    std::cout << "[Test] Address Reuse... ";
    nstd::memory_pool<int> pool;

    // 1. Allocate a slot
    int* ptr1 = pool.allocate(42);
    void* addr1 = ptr1;

    // 2. Return it to the pool
    pool.deallocate(ptr1);

    // 3. Allocate again - it should give us the SAME address (LIFO logic)
    int* ptr2 = pool.allocate(99);

    assert(ptr2 == addr1);
    assert(*ptr2 == 99);

    std::cout << "Passed.\n";
}

void test_chunk_expansion() {
    std::cout << "[Test] Chunk Expansion... ";

    // Create a tiny pool (Chunk size = 5 blocks)
    nstd::memory_pool<size_t, 5> pool;
    nstd::vector<size_t*> ptrs;

    // 1. Allocate 5 (Fill the first chunk)
    for (size_t i = 0; i < 5; ++i) {
        ptrs.push_back(pool.allocate(i));
    }

    // 2. Allocate the 6th (Trigger Expand)
    // If logic is wrong, this crashes or returns nullptr
    size_t* p6 = pool.allocate(100);
    assert(*p6 == 100);

    // 3. Allocate more to ensure the new linked list is healthy
    size_t* p7 = pool.allocate(101);
    assert(*p7 == 101);

    std::cout << "Passed.\n";
}

void test_complex_types() {
    std::cout << "[Test] Complex Types (std::string)... ";
    nstd::memory_pool<nstd::string> pool;

    // Test constructor forwarding
    nstd::string* s1 = pool.allocate("Hello Memory Pool");
    nstd::string* s2 = pool.allocate(5, 'A'); // "AAAAA"

    assert(*s1 == "Hello Memory Pool");
    assert(*s2 == "AAAAA");

    // Important: string allocates internal memory.
    // deallocate() must call ~string() to free that internal heap memory.
    pool.deallocate(s1);
    pool.deallocate(s2);

    std::cout << "Passed.\n";
}

void test_destructor_call() {
    std::cout << "[Test] Destructor Calls... ";
    Tracker::alive_count = 0;

    nstd::memory_pool<Tracker> pool;

    // Allocate 3 objects
    Tracker* t1 = pool.allocate(1);
    Tracker* t2 = pool.allocate(2);
    Tracker* t3 = pool.allocate(3);

    assert(Tracker::alive_count == 3);

    // Deallocate one
    pool.deallocate(t1);
    assert(Tracker::alive_count == 2); // Should have dropped

    // Deallocate rest
    pool.deallocate(t2);
    pool.deallocate(t3);
    assert(Tracker::alive_count == 0);

    std::cout << "Passed.\n";
}

void test_small_object_correctness() {
    std::cout << "[Test] Small Object Size... ";

    // char is 1 byte.
    // The pool uses 8 bytes (sizeof void*) internally.
    // If we write to char* p, we must ensure we don't corrupt the next pointer.
    nstd::memory_pool<char> pool;

    char* c1 = pool.allocate('a');
    char* c2 = pool.allocate('b');

    // Verify values are stable
    assert(*c1 == 'a');
    assert(*c2 == 'b');

    // Verify addresses are at least 8 bytes apart (sizeof(void*))
    // Note: This assumes stack/heap grows in a specific direction,
    // checking absolute diff is safer.
    size_t diff = std::abs(c2 - c1);
    assert(diff >= sizeof(void*));

    std::cout << "Passed.\n";
}

void run_all_tests() {
    std::cout << "=== Running Memory Pool Tests ===\n";

    test_basic_allocation();
    test_address_reuse();
    test_chunk_expansion();
    test_complex_types();
    test_destructor_call();
    test_small_object_correctness();

    std::cout << "=== All Tests Passed ===\n";
}
} // namespace memory_pool
} // namespace tests