#include <iostream>
#include <cassert>
#include <string>
#include <utility>
#include "nstd/vector.hpp"

namespace tests
{
    namespace vector
    {
        // ------------------------- Basic Push / Access / Modify -------------------------
        void test_basic_operations()
        {
            std::cout << "--- Running Basic Operations Test ---\n";

            nstd::vector<int> v;
            assert(v.size() == 0);
            assert(v.is_empty());

            v.push_back(1);
            v.push_back(2);
            v.push_back(3);

            assert(v.size() == 3);
            assert(!v.is_empty());

            // Access & modify
            v[1] = 20;
            assert(v[0] == 1 && v[1] == 20 && v[2] == 3);

            // pop_back
            v.pop_back();
            assert(v.size() == 2);

            // clear
            v.clear();
            assert(v.size() == 0 && v.is_empty());
        }

        // ------------------------- Copy / Move Semantics -------------------------
        void test_copy_move()
        {
            std::cout << "--- Running Copy / Move Test ---\n";

            nstd::vector<int> v1;
            for (int i = 0; i < 5; ++i)
                v1.push_back(i);

            // Copy constructor
            nstd::vector<int> v2(v1);
            assert(v2.size() == v1.size());
            for (size_t i = 0; i < v1.size(); ++i)
                assert(v1[i] == v2[i]);

            // Copy assignment
            nstd::vector<int> v3;
            v3 = v1;
            assert(v3.size() == v1.size());
            for (size_t i = 0; i < v1.size(); ++i)
                assert(v1[i] == v3[i]);

            // Move constructor
            nstd::vector<int> v4(std::move(v1));
            assert(v4.size() == 5);
            assert(v1.size() == 0); // moved-from vector should be empty

            // Move assignment
            nstd::vector<int> v5;
            v5 = std::move(v2);
            assert(v5.size() == 5);
            assert(v2.size() == 0);
        }

        // ------------------------- Growth / Reallocation -------------------------
        void test_growth()
        {
            std::cout << "--- Running Growth / Reallocation Test ---\n";

            nstd::vector<int> v;
            size_t initial_capacity = v.get_capacity();

            for (int i = 0; i < 1000; ++i)
            {
                v.push_back(i);
                assert(v[i] == i);
                assert(v.size() == i + 1);
            }

            assert(v.size() == 1000);
            assert(v.get_capacity() >= 1000);
        }

        // ------------------------- Iterators Test -------------------------
        void test_iterators()
        {
            std::cout << "--- Running Iterators Test ---\n";

            nstd::vector<int> v;
            for (int i = 0; i < 5; ++i)
                v.push_back(i);

            int expected = 0;
            for (auto it = v.begin(); it != v.end(); ++it)
            {
                assert(*it == expected++);
            }

            expected = 0;
            for (const auto &x : v)
            {
                assert(x == expected++);
            }
        }

        // ------------------------- Shrink / Reserve -------------------------
        void test_reserve_shrink()
        {
            std::cout << "--- Running Reserve / Shrink Test ---\n";

            nstd::vector<int> v;
            v.reserve(50);
            assert(v.get_capacity() >= 50);

            for (int i = 0; i < 10; ++i)
                v.push_back(i);
            v.shrink_to_fit();
            assert(v.get_capacity() == v.size());
        }

        // ------------------------- Edge Cases -------------------------
        void test_edge_cases()
        {
            std::cout << "--- Running Edge Cases Test ---\n";

            nstd::vector<int> v;

            if (!v.is_empty())
            {
                v.pop_back();
            }

            // push_back with 0 capacity initially
            v.push_back(42);
            assert(v.size() == 1 && v[0] == 42);
        }

        // ------------------------- Multiple Types Test -------------------------
        void test_multiple_types()
        {
            std::cout << "--- Running Multiple Types Test ---\n";

            nstd::vector<double> vd;
            vd.push_back(3.14);
            vd.push_back(2.718);
            assert(vd[0] == 3.14 && vd[1] == 2.718);

            nstd::vector<std::string> vs;
            vs.push_back("Hello");
            vs.push_back("World");
            assert(vs[0] == "Hello" && vs[1] == "World");

            nstd::vector<std::pair<int, int>> vp;
            vp.push_back({1, 2});
            vp.push_back({3, 4});
            assert(vp[0].first == 1 && vp[0].second == 2);
            assert(vp[1].first == 3 && vp[1].second == 4);
        }

        // ------------------------- Stress Test -------------------------
        void test_stress()
        {
            std::cout << "--- Running Stress Test ---\n";

            nstd::vector<int> v;
            const int N = 100000;
            for (int i = 0; i < N; ++i)
                v.push_back(i);

            long long sum = 0;
            for (auto x : v)
                sum += x;

            long long expected = (N - 1LL) * N / 2;
            assert(sum == expected);
        }

    }
}
// ------------------------- ALL TESTS ARE WRITTEN BY CHATGPT-------------------------
int main()
{
    using namespace tests::vector;

    test_basic_operations();
    test_copy_move();
    test_growth();
    test_iterators();
    test_reserve_shrink();
    test_edge_cases();
    test_multiple_types();
    test_stress();

    std::cout << "All tests passed!\n";
    return 0;
}
