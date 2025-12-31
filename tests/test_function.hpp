#ifndef TESTS_FUNCTION_ENHANCED_HPP
#define TESTS_FUNCTION_ENHANCED_HPP

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <functional>
#include "nstd/function.hpp"

namespace tests
{
    namespace function
    {
        // ==========================================
        // HELPER: Memory Tracker for Function Captures
        // ==========================================
        struct FnObj {
            static int alive_count;
            int id;

            FnObj(int i = 0) : id(i) { ++alive_count; }
            FnObj(const FnObj& other) : id(other.id) { ++alive_count; }
            FnObj(FnObj&& other) noexcept : id(other.id) { other.id = -1; ++alive_count; }
            ~FnObj() { if (id != -999) --alive_count; id = -999; }

            static void verify_no_leaks() {
                if (alive_count != 0) {
                    std::cerr << "FUNCTION LEAK DETECTED! Captures alive: " << alive_count << "\n";
                    std::terminate();
                }
            }
        };

        int FnObj::alive_count = 0;

        // ==========================================
        // TEST SUITES
        // ==========================================

        void test_basic_invocation() {
            std::cout << "[Test] Basic Invocation... ";

            // Lambda no capture
            nstd::function<int(int, int)> f1 = [](int a, int b) { return a + b; };
            assert(f1(5, 5) == 10);

            // Function pointer
            auto raw_func = +[](int a) { return a * a; };
            nstd::function<int(int)> f2 = raw_func;
            assert(f2(4) == 16);

            std::cout << "PASSED\n";
        }

        void test_lifecycle_and_leaks() {
            std::cout << "[Test] Lifecycle & Leaks... ";
            FnObj::alive_count = 0;
            {
                FnObj tracker(42);
                nstd::function<int()> f = [tracker]() { return tracker.id; };
                assert(f() == 42);
                assert(FnObj::alive_count == 2); // Original + Captured Copy
            }
            FnObj::verify_no_leaks();
            std::cout << "PASSED\n";
        }

        void test_copy_semantics_deep_copy() {
            std::cout << "[Test] Copy Semantics (Deep Copy)... ";
            int val = 10;
            nstd::function<int()> f1 = [val]() mutable { return ++val; };

            nstd::function<int()> f2 = f1; // Should trigger clone()

            assert(f1() == 11);
            assert(f1() == 12);
            assert(f2() == 11); // f2 should have its own state
            assert(f2() == 12);

            std::cout << "PASSED\n";
        }

        void test_move_semantics_efficiency() {
            std::cout << "[Test] Move Semantics... ";
            FnObj::alive_count = 0;
            {
                nstd::function<int()> f1 = [o = FnObj(1)]() { return o.id; };
                int current_alive = FnObj::alive_count;

                nstd::function<int()> f2 = std::move(f1);
                assert(FnObj::alive_count == current_alive); // No new copies made
                assert(!f1); // f1 should be nulled out
                assert(f2() == 1);
            }
            FnObj::verify_no_leaks();
            std::cout << "PASSED\n";
        }

        void test_polymorphic_container() {
            std::cout << "[Test] Polymorphic Container (Vector)... ";
            nstd::vector<nstd::function<int(int)>> pipeline;

            pipeline.push_back([](int x) { return x + 1; });
            pipeline.push_back([](int x) { return x * 2; });
            pipeline.push_back([](int x) { return x - 3; });

            int val = 5;
            for (auto& f : pipeline) {
                val = f(val);
            }
            // ((5 + 1) * 2) - 3 = 9
            assert(val == 9);
            std::cout << "PASSED\n";
        }

        void test_empty_function_behavior() {
            std::cout << "[Test] Empty States... ";
            nstd::function<void()> f_empty;
            assert(!f_empty);

            nstd::function<void()> f_null = nullptr;
            assert(!f_null);

            // Re-assignment to null
            nstd::function<void()> f_valid = []() {};
            assert(f_valid);
            f_valid = nullptr;
            assert(!f_valid);

            std::cout << "PASSED\n";
        }

        void test_large_capture_complex() {
            std::cout << "[Test] Large Capture (Logic Complexity)... ";
            std::string long_str = "Standard Template Library";
            std::vector<int> data = { 1, 2, 3, 4, 5 };

            nstd::function<size_t()> f = [=]() {
                return long_str.size() + data.size();
                };

            assert(f() == (25 + 5));
            std::cout << "PASSED\n";
        }

        void test_self_assignment() {
            std::cout << "[Test] Self Assignment... ";
            nstd::function<int()> f = [o = FnObj(7)]() { return o.id; };
            f = f; // Copy self
            assert(f() == 7);

            f = std::move(f); // Move self
            assert(f() == 7);
            std::cout << "PASSED\n";
        }

        // ==========================================
        // MAIN RUNNER
        // ==========================================
        void run_all_tests()
        {
            std::cout << "==========================================\n";
            std::cout << "  RUNNING EXTENSIVE FUNCTION TESTS\n";
            std::cout << "  (Type Erasure & Lifetime Tracking)\n";
            std::cout << "==========================================\n\n";

            test_basic_invocation();
            test_lifecycle_and_leaks();
            test_copy_semantics_deep_copy();
            test_move_semantics_efficiency();
            test_polymorphic_container();
            test_empty_function_behavior();
            test_large_capture_complex();
            test_self_assignment();

            std::cout << "\n==========================================\n";
            std::cout << "  ALL FUNCTION TESTS PASSED!\n";
            std::cout << "==========================================\n";
        }
    }
}

#endif