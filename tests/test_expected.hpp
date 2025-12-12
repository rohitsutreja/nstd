#ifndef TEST_EXPECTED_HPP
#define TEST_EXPECTED_HPP

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "nstd/expected.hpp"

// Helper to assert conditions and print
#define TEST_ASSERT(cond)                                                              \
    if (!(cond))                                                                       \
    {                                                                                  \
        std::cerr << "Assertion failed: " << #cond << " at line " << __LINE__ << "\n"; \
        std::terminate();                                                              \
    }

namespace tests
{
    namespace expected_test
    {

        // 1. Basic Construction and Access
        void test_basic_operations()
        {
            std::cout << "  Testing basic operations... ";

            // Value construction
            nstd::expected<int, std::string> e1(42);
            TEST_ASSERT(e1.has_value());
            TEST_ASSERT(*e1 == 42);

            // Error construction
            nstd::expected<int, std::string> e2 = nstd::unexpected<std::string>("Bad Input");
            TEST_ASSERT(!e2.has_value());
            TEST_ASSERT(e2.error() == "Bad Input");

            // Arrow operator
            nstd::expected<std::string, int> e3("Hello");
            TEST_ASSERT(e3->length() == 5);

            std::cout << "Passed.\n";
        }

        // 2. State Switching
        void test_state_switching()
        {
            std::cout << "  Testing state switching (Value <-> Error)... ";

            nstd::expected<int, int> e(100); // Starts as Value
            TEST_ASSERT(e.has_value());

            // Switch Value -> Error
            e = nstd::unexpected<int>(404);
            TEST_ASSERT(!e.has_value());
            TEST_ASSERT(e.error() == 404);

            // Switch Error -> Value
            e = 200;
            TEST_ASSERT(e.has_value());
            TEST_ASSERT(*e == 200);

            // Switch Error -> Error (Update error code)
            e = nstd::unexpected<int>(500); // Make it error first
            e = nstd::unexpected<int>(503); // Update error
            TEST_ASSERT(e.error() == 503);

            std::cout << "Passed.\n";
        }

        // 3. Copy Semantics (Deep Copy)
        void test_copy()
        {
            std::cout << "  Testing copy semantics... ";

            // Copy Value
            nstd::expected<std::vector<int>, int> original_val({1, 2, 3});
            nstd::expected<std::vector<int>, int> copy_val = original_val;

            TEST_ASSERT(copy_val.has_value());
            TEST_ASSERT((*copy_val).size() == 3);
            TEST_ASSERT((*original_val).size() == 3); // Original still intact

            // Copy Error
            nstd::expected<int, std::string> original_err = nstd::unexpected<std::string>("Error");
            nstd::expected<int, std::string> copy_err = original_err;

            TEST_ASSERT(!copy_err.has_value());
            TEST_ASSERT(copy_err.error() == "Error");

            std::cout << "Passed.\n";
        }

        // 4. Move Semantics (Resource Stealing)
        void test_move()
        {
            std::cout << "  Testing move semantics... ";

            // Move Value (std::vector)
            std::vector<int> data = {10, 20, 30};
            nstd::expected<std::vector<int>, int> e1(data); // Copy construct

            nstd::expected<std::vector<int>, int> e2 = std::move(e1); // Move construct

            TEST_ASSERT(e2.has_value());
            TEST_ASSERT((*e2).size() == 3);
            TEST_ASSERT((*e2)[0] == 10);
            // Original should be empty (moved from)
            TEST_ASSERT((*e1).empty());

            // Move Assignment (Error -> Value)
            nstd::expected<std::vector<int>, int> e3 = nstd::unexpected<int>(500);
            e3 = std::move(e2); // Should destroy error 500, move vector in

            TEST_ASSERT(e3.has_value());
            TEST_ASSERT((*e3).size() == 3);

            std::cout << "Passed.\n";
        }

        // 5. Complex Types (Memory Management)
        void test_complex_types()
        {
            std::cout << "  Testing complex types (std::string)... ";

            using Exp = nstd::expected<std::string, std::string>;

            // String Value
            Exp e1("Success String");
            TEST_ASSERT(*e1 == "Success String");

            // String Error
            Exp e2 = nstd::unexpected<std::string>("Error String");
            TEST_ASSERT(e2.error() == "Error String");

            // Assignment that triggers destructor
            e1 = nstd::unexpected<std::string>("Now an error");
            TEST_ASSERT(!e1.has_value());
            TEST_ASSERT(e1.error() == "Now an error");

            std::cout << "Passed.\n";
        }

        // 6. Edge Cases
        void test_edge_cases()
        {
            std::cout << "  Testing edge cases... ";

            // Self Assignment
            nstd::expected<int, int> e1(10);
            e1 = e1;
            TEST_ASSERT(*e1 == 10);

            // Self Move
            e1 = std::move(e1);
            TEST_ASSERT(*e1 == 10);

            // Const correctness
            const nstd::expected<int, int> const_e(50);
            TEST_ASSERT(*const_e == 50);
            TEST_ASSERT(const_e.has_value());

            std::cout << "Passed.\n";
        }

        // 7. Comparison Operators
        void test_comparisons()
        {
            std::cout << "  Testing comparisons... ";

            // Setup common objects
            nstd::expected<int, std::string> val10(10);
            nstd::expected<int, std::string> val20(20);
            nstd::expected<int, std::string> val10_dup(10);

            nstd::expected<int, std::string> errBad = nstd::unexpected<std::string>("Bad");
            nstd::expected<int, std::string> errWorse = nstd::unexpected<std::string>("Worse");
            nstd::expected<int, std::string> errBad_dup = nstd::unexpected<std::string>("Bad");
            // --- 1. Peer Comparison (expected == expected) ---
            // Value vs Value
            TEST_ASSERT(val10 == val10_dup); // 10 == 10
            TEST_ASSERT(!(val10 == val20));  // 10 != 20

            // Error vs Error
            TEST_ASSERT(errBad == errBad_dup);  // "Bad" == "Bad"
            TEST_ASSERT(!(errBad == errWorse)); // "Bad" != "Worse"

            // Value vs Error (Mixed)
            TEST_ASSERT(!(val10 == errBad)); // Value != Error
            TEST_ASSERT(!(errBad == val10)); // Error != Value

            // --- 2. Value Comparison (expected == T) ---
            TEST_ASSERT(val10 == 10);     // Match
            TEST_ASSERT(!(val10 == 99));  // Mismatch
            TEST_ASSERT(!(errBad == 10)); // Error is never equal to a Value

            // Mirror Check (T == expected)
            // (If you didn't implement the mirror operator, these lines will fail to compile)
            TEST_ASSERT(10 == val10);
            TEST_ASSERT(!(99 == val10));

            // --- 3. Error Comparison (expected == unexpected) ---
            TEST_ASSERT(errBad == nstd::unexpected<std::string>("Bad"));
            TEST_ASSERT(!(errBad == nstd::unexpected<std::string>("Worse")));
            TEST_ASSERT(!(val10 == nstd::unexpected<std::string>("Bad"))); // Value is never equal to an Error

            // Mirror Check (unexpected == expected)
            TEST_ASSERT(nstd::unexpected<std::string>("Bad") == errBad);

            std::cout << "Passed.\n";
        }
    } // namespace nstd::expected_test
} // namespace tests

#endif // TEST_nstd::expected_HPP