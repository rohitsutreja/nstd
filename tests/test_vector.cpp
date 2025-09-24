#include <iostream>
#include <cassert>
#include <string>
#include "nstd/vector.hpp"

// ------------------------- Basic Tests - Written By ChatGPT -------------------------
void test_vector_basic()
{
    std::cout << "--- Running Basic Tests ---\n";

    nstd::vector<int> v;
    assert(v.size() == 0);

    // Push back elements
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    assert(v.size() == 3);

    // Access via operator[]
    assert(v[0] == 10);
    assert(v[1] == 20);
    assert(v[2] == 30);

    // Modify via operator[]
    v[1] = 25;
    assert(v[1] == 25);

    // Print current vector
    for (size_t i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";
    std::cout << "\n";
}

// ------------------------- Growth Test -------------------------
void test_vector_growth()
{
    std::cout << "--- Running Growth Test ---\n";

    nstd::vector<int> v;
    const int N = 1000; // stress test dynamic resizing
    for (int i = 0; i < N; ++i)
    {
        v.push_back(i);
        assert(v[i] == i);         // check newly added element
        assert(v.size() == i + 1); // check size
    }

    // Verify last element
    assert(v[N - 1] == N - 1);
    std::cout << "Vector size after growth test: " << v.size() << "\n";
}

// ------------------------- Loop / Accumulation Test -------------------------
void test_vector_loop()
{
    std::cout << "--- Running Loop Test ---\n";

    nstd::vector<int> v;
    for (int i = 0; i < 10; ++i)
        v.push_back(i * 2);

    int sum = 0;
    for (size_t i = 0; i < v.size(); ++i)
        sum += v[i];

    assert(sum == 0 + 2 + 4 + 6 + 8 + 10 + 12 + 14 + 16 + 18);
    std::cout << "Sum of elements: " << sum << "\n";
}

// ------------------------- Edge Cases -------------------------
void test_vector_edge_cases()
{
    std::cout << "--- Running Edge Case Tests ---\n";

    nstd::vector<int> v;

    // Skip unsafe operator[] if vector empty
    if (v.size() > 0)
        v[0]; // safe
    else
        std::cout << "Vector empty, skipping operator[] access.\n";

    // Minimal push test
    v.push_back(42);
    assert(v.size() == 1);
    assert(v[0] == 42);

    // Multiple push to check growth from minimal
    v.push_back(100);
    assert(v.size() == 2);
    assert(v[1] == 100);
}

// ------------------------- Multiple Types Test -------------------------
void test_vector_types()
{
    std::cout << "--- Running Multiple Types Test ---\n";

    // Double
    nstd::vector<double> vd;
    vd.push_back(3.14);
    vd.push_back(2.718);
    assert(vd[0] == 3.14);
    assert(vd[1] == 2.718);

    // String
    nstd::vector<std::string> vs;
    vs.push_back("Hello");
    vs.push_back("World");
    assert(vs[0] == "Hello");
    assert(vs[1] == "World");

    // Pair
    nstd::vector<std::pair<int, int>> vp;
    vp.push_back({1, 2});
    vp.push_back({3, 4});
    assert(vp[0].first == 1 && vp[0].second == 2);
    assert(vp[1].first == 3 && vp[1].second == 4);
}

// ------------------------- Main -------------------------
int main()
{
    test_vector_basic();
    test_vector_growth();
    test_vector_loop();
    test_vector_edge_cases();
    test_vector_types();

    std::cout << "All tests passed!\n";
    return 0;
}
