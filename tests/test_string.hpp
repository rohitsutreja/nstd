#ifndef TESTS_STRING_HPP
#define TESTS_STRING_HPP

#include "nstd/string.hpp"
#include "nstd/vector.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace tests {
namespace string {
// Helper function to test exception throwing
template<typename Func> bool throws_exception(Func&& func) {
    try {
        func();
        return false;
    } catch (...) {
        return true;
    }
}

// ------------------------- Construction Tests -------------------------
void test_construction() {
    std::cout << "--- Running Construction Tests ---\n";

    // Default constructor
    nstd::string s1;
    assert(s1.size() == 0);
    assert(s1.length() == 0);
    assert(s1.empty());
    assert(s1.capacity() >= 1);
    assert(s1.c_str()[0] == '\0');

    // C-string constructor
    nstd::string s2("Hello");
    assert(s2.size() == 5);
    assert(s2.length() == 5);
    assert(!s2.empty());
    assert(std::string(s2.c_str()) == "Hello");

    // Array constructor
    char arr[] = "World";
    nstd::string s3(arr);
    assert(s3.size() == 5);
    assert(std::string(s3.c_str()) == "World");

    // Array constructor with null terminator
    char arr2[] = {'T', 'e', 's', 't', '\0'};
    nstd::string s4(arr2);
    assert(s4.size() == 4);
    assert(std::string(s4.c_str()) == "Test");

    // Empty string
    nstd::string s5("");
    assert(s5.size() == 0);
    assert(s5.empty());

    // Copy constructor
    nstd::string s6(s2);
    assert(s6.size() == s2.size());
    assert(std::string(s6.c_str()) == std::string(s2.c_str()));

    // Move constructor
    nstd::string s7(std::move(s6));
    assert(s7.size() == 5);
    assert(std::string(s7.c_str()) == "Hello");
    assert(s6.size() == 0); // moved-from should be empty
    assert(s6.empty());
}

// ------------------------- Assignment Tests -------------------------
void test_assignment() {
    std::cout << "--- Running Assignment Tests ---\n";

    nstd::string s1("Original");
    nstd::string s2;

    // Copy assignment
    s2 = s1;
    assert(s2.size() == s1.size());
    assert(std::string(s2.c_str()) == std::string(s1.c_str()));

    // C-string assignment
    s2 = "New Value";
    assert(s2.size() == 9);
    assert(std::string(s2.c_str()) == "New Value");

    // Move assignment
    nstd::string s3("Move Me");
    s1 = std::move(s3);
    assert(s1.size() == 7);
    assert(std::string(s1.c_str()) == "Move Me");
    assert(s3.size() == 0);

    // Self assignment (copy-and-swap should handle this)
    s1 = s1;
    assert(s1.size() == 7);
    assert(std::string(s1.c_str()) == "Move Me");
}

// ------------------------- Element Access Tests -------------------------
void test_element_access() {
    std::cout << "--- Running Element Access Tests ---\n";

    nstd::string s("Hello World");

    // operator[]
    assert(s[0] == 'H');
    assert(s[6] == 'W');
    assert(s[10] == 'd');

    // Modify via operator[]
    s[6] = 'w';
    assert(s[6] == 'w');
    assert(std::string(s.c_str()) == "Hello world");

    // at() method
    assert(s.at(0) == 'H');
    assert(s.at(10) == 'd');

    // at() bounds checking
    assert(throws_exception([&]() { s.at(11); }));
    assert(throws_exception([&]() { s.at(100); }));

    // const access
    const nstd::string cs("Const");
    assert(cs[0] == 'C');
    assert(cs.at(4) == 't');
}

// ------------------------- Capacity Tests -------------------------
void test_capacity() {
    std::cout << "--- Running Capacity Tests ---\n";

    nstd::string s;
    assert(s.size() == 0);
    assert(s.length() == 0);
    assert(s.empty());
    assert(s.capacity() >= 1);

    s = "Hello";
    assert(s.size() == 5);
    assert(s.length() == 5);
    assert(!s.empty());

    size_t old_capacity = s.capacity();
    s.reserve(100);
    assert(s.capacity() >= 100);
    assert(s.size() == 5); // size unchanged

    // Reserve smaller should not shrink
    s.reserve(10);
    assert(s.capacity() >= 100);

    s.clear();
    assert(s.size() == 0);
    assert(s.empty());
    assert(s.c_str()[0] == '\0');
}

// ------------------------- Modifiers Tests -------------------------
void test_modifiers() {
    std::cout << "--- Running Modifiers Tests ---\n";

    nstd::string s;

    // push_back
    s.push_back('A');
    assert(s.size() == 1);
    assert(s[0] == 'A');
    assert(std::string(s.c_str()) == "A");

    s.push_back('B');
    s.push_back('C');
    assert(s.size() == 3);
    assert(std::string(s.c_str()) == "ABC");

    // pop_back
    s.pop_back();
    assert(s.size() == 2);
    assert(std::string(s.c_str()) == "AB");

    // pop_back on empty (should be safe)
    s.clear();
    s.pop_back();
    assert(s.size() == 0);
    assert(s.empty());

    // append C-string
    s.append("Hello");
    assert(s.size() == 5);
    assert(std::string(s.c_str()) == "Hello");

    s.append(" World");
    assert(s.size() == 11);
    assert(std::string(s.c_str()) == "Hello World");

    // append empty string
    s.append("");
    assert(s.size() == 11);
    assert(std::string(s.c_str()) == "Hello World");

    // += operators
    nstd::string s2;
    s2 += 'X';
    assert(s2.size() == 1);
    assert(s2[0] == 'X');

    s2 += "YZ";
    assert(s2.size() == 3);
    assert(std::string(s2.c_str()) == "XYZ");

    nstd::string s3("ABC");
    s2 += s3;
    assert(s2.size() == 6);
    assert(std::string(s2.c_str()) == "XYZABC");
}

// ------------------------- Growth and Reallocation Tests -------------------------
void test_growth() {
    std::cout << "--- Running Growth Tests ---\n";

    nstd::string s;
    size_t initial_capacity = s.capacity();

    // Test growth through push_back
    for (int i = 0; i < 100; ++i) {
        s.push_back('a' + (i % 26));
    }
    assert(s.size() == 100);
    assert(s.capacity() >= 100);

    // Test growth through append
    nstd::string s2;
    for (int i = 0; i < 20; ++i) {
        s2.append("12345");
    }
    assert(s2.size() == 100);
    assert(s2.capacity() >= 100);

    // Verify content is preserved during growth
    std::string expected;
    for (int i = 0; i < 20; ++i) {
        expected += "12345";
    }
    assert(std::string(s2.c_str()) == expected);
}

// ------------------------- Iterator Tests -------------------------
void test_iterators() {
    std::cout << "--- Running Iterator Tests ---\n";

    nstd::string s("Hello");

    // Basic iteration
    int i = 0;
    for (auto it = s.begin(); it != s.end(); ++it, ++i) {
        assert(*it == "Hello"[i]);
    }

    // Range-based for loop
    i = 0;
    for (char c : s) {
        assert(c == "Hello"[i++]);
    }

    // const iterators
    const nstd::string cs("World");
    i = 0;
    for (auto it = cs.cbegin(); it != cs.cend(); ++it, ++i) {
        assert(*it == "World"[i]);
    }

    // Modify through iterator
    for (auto it = s.begin(); it != s.end(); ++it) {
        if (*it == 'e') {
            *it = 'E';
        }
    }
    assert(std::string(s.c_str()) == "HEllo");
}

// ------------------------- Comparison Tests -------------------------
void test_comparisons() {
    std::cout << "--- Running Comparison Tests ---\n";

    nstd::string s1("Hello");
    nstd::string s2("Hello");
    nstd::string s3("World");
    nstd::string s4("Hell");

    // Equality
    assert(s1 == s2);
    assert(!(s1 == s3));
    assert(!(s1 == s4));

    // Inequality
    assert(!(s1 != s2));
    assert(s1 != s3);
    assert(s1 != s4);

    // Empty strings
    nstd::string empty1;
    nstd::string empty2;
    assert(empty1 == empty2);
    assert(!(empty1 != empty2));
    assert(!(empty1 == s1));
    assert(empty1 != s1);
}

// ------------------------- Stream Tests -------------------------
void test_stream_operations() {
    std::cout << "--- Running Stream Tests ---\n";

    nstd::string s("Hello World");

    // Test output stream
    std::ostringstream oss;
    oss << s;
    assert(oss.str() == "Hello World");

    // Test with empty string
    nstd::string empty;
    std::ostringstream oss2;
    oss2 << empty;
    assert(oss2.str() == "");
}

// ------------------------- Memory Safety Tests -------------------------
void test_memory_safety() {
    std::cout << "--- Running Memory Safety Tests ---\n";

    // Test that data() and c_str() are null-terminated
    nstd::string s("Test");
    const char* cstr = s.c_str();
    const char* data_ptr = s.data();

    assert(cstr[4] == '\0');
    assert(data_ptr[4] == '\0');
    assert(std::string(cstr) == "Test");
    assert(std::string(data_ptr) == "Test");

    // Test after modifications
    s.push_back('!');
    assert(s.c_str()[5] == '\0');
    assert(std::string(s.c_str()) == "Test!");

    s.pop_back();
    assert(s.c_str()[4] == '\0');
    assert(std::string(s.c_str()) == "Test");
}

// ------------------------- Edge Cases Tests -------------------------
void test_edge_cases() {
    std::cout << "--- Running Edge Cases Tests ---\n";

    // Very long string
    nstd::string long_str;
    for (int i = 0; i < 10000; ++i) {
        long_str.push_back('a' + (i % 26));
    }
    assert(long_str.size() == 10000);

    // String with null characters (if supported)
    nstd::string s;
    s.push_back('A');
    s.push_back('\0');
    s.push_back('B');
    assert(s.size() == 3);
    assert(s[0] == 'A');
    assert(s[1] == '\0');
    assert(s[2] == 'B');

    // Multiple consecutive operations
    nstd::string s2;
    for (int i = 0; i < 100; ++i) {
        s2.push_back('x');
    }
    for (int i = 0; i < 50; ++i) {
        s2.pop_back();
    }
    assert(s2.size() == 50);

    // Clear and reuse
    s2.clear();
    assert(s2.empty());
    s2 = "Reused";
    assert(s2.size() == 6);
    assert(std::string(s2.c_str()) == "Reused");
}

// ------------------------- Swap Test -------------------------
void test_swap() {
    std::cout << "--- Running Swap Tests ---\n";

    nstd::string s1("Hello");
    nstd::string s2("World");

    std::string s1_orig(s1.c_str());
    std::string s2_orig(s2.c_str());

    swap(s1, s2);

    assert(std::string(s1.c_str()) == s2_orig);
    assert(std::string(s2.c_str()) == s1_orig);
    assert(s1.size() == 5);
    assert(s2.size() == 5);
}

// ------------------------- Exception Safety Tests -------------------------
void test_exception_safety() {
    std::cout << "--- Running Exception Safety Tests ---\n";

    // Test that out_of_range is thrown for invalid indices
    nstd::string s("Hello");

    assert(throws_exception([&]() { s.at(5); }));
    assert(throws_exception([&]() { s.at(100); }));

    const nstd::string cs("World");
    assert(throws_exception([&]() { cs.at(5); }));
    assert(throws_exception([&]() { cs.at(100); }));

    // Test that string remains valid after exception
    try {
        s.at(100);
    } catch (...) {
        // String should still be valid
        assert(s.size() == 5);
        assert(std::string(s.c_str()) == "Hello");
    }
}

// ------------------------- Performance/Stress Tests -------------------------
void test_stress() {
    std::cout << "--- Running Stress Tests ---\n";

    // Large number of push_backs
    nstd::string s;
    const int N = 50000;

    for (int i = 0; i < N; ++i) {
        s.push_back('a' + (i % 26));
    }
    assert(s.size() == N);

    // Large number of appends
    nstd::string s2;
    for (int i = 0; i < 1000; ++i) {
        s2.append("test");
    }
    assert(s2.size() == 4000);

    // Copy large string
    nstd::string s3(s);
    assert(s3.size() == s.size());

    // Move large string
    nstd::string s4(std::move(s3));
    assert(s4.size() == N);
    assert(s3.size() == 0);
}

// ------------------------- Type Alias Tests -------------------------
void test_type_aliases() {
    std::cout << "--- Running Type Alias Tests ---\n";

    // Test string alias
    nstd::string s("Hello");
    assert(s.size() == 5);
}
} // namespace string
} // namespace tests

#endif // TESTS_STRING_HPP