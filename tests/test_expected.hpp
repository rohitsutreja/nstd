#ifndef TEST_EXPECTED_MASTER_HPP
#define TEST_EXPECTED_MASTER_HPP

#include <iostream>
#include <cassert>
#include <memory>
#include "nstd/expected.hpp"
#include "nstd/string.hpp"
#include "nstd/vector.hpp"

#define TEST_ASSERT(cond)                                                              \
    if (!(cond))                                                                       \
    {                                                                                  \
        std::cerr << "Assertion failed: " << #cond << " at line " << __LINE__ << "\n"; \
        std::terminate();                                                              \
    }

namespace tests
{
    namespace expected
    {
        // ============================================================================
        // ORIGINAL TEST CASES (1-15)
        // ============================================================================

        // 1. Basic Construction and Access
        void test_basic_operations()
        {
            std::cout << "  Testing basic operations... ";

            // Value construction
            nstd::expected<int, nstd::string> e1(42);
            TEST_ASSERT(e1.has_value());
            TEST_ASSERT(*e1 == 42);

            // Error construction
            nstd::expected<int, nstd::string> e2 = nstd::unexpected<nstd::string>("Bad Input");
            TEST_ASSERT(!e2.has_value());
            TEST_ASSERT(e2.error() == "Bad Input");

            // Arrow operator
            nstd::expected<nstd::string, int> e3("Hello");
            TEST_ASSERT(e3->length() == 5);

            std::cout << "Passed.\n";
        }

        // 2. State Switching
        void test_state_switching()
        {
            std::cout << "  Testing state switching (Value <-> Error)... ";

            nstd::expected<int, int> e(100);
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
            e = nstd::unexpected<int>(500);
            e = nstd::unexpected<int>(503);
            TEST_ASSERT(e.error() == 503);

            std::cout << "Passed.\n";
        }

        // 3. Copy Semantics (Deep Copy)
        void test_copy()
        {
            std::cout << "  Testing copy semantics... ";

            // Copy Value
            nstd::expected<nstd::vector<int>, int> original_val({1, 2, 3});
            nstd::expected<nstd::vector<int>, int> copy_val = original_val;

            TEST_ASSERT(copy_val.has_value());
            TEST_ASSERT((*copy_val).size() == 3);
            TEST_ASSERT((*original_val).size() == 3);

            // Copy Error
            nstd::expected<int, nstd::string> original_err = nstd::unexpected<nstd::string>("Error");
            nstd::expected<int, nstd::string> copy_err = original_err;

            TEST_ASSERT(!copy_err.has_value());
            TEST_ASSERT(copy_err.error() == "Error");

            std::cout << "Passed.\n";
        }

        // 4. Move Semantics (Resource Stealing)
        void test_move()
        {
            std::cout << "  Testing move semantics... ";

            // Move Value (nstd::vector)
            nstd::vector<int> data = {10, 20, 30};
            nstd::expected<nstd::vector<int>, int> e1(data);

            nstd::expected<nstd::vector<int>, int> e2 = std::move(e1);

            TEST_ASSERT(e2.has_value());
            TEST_ASSERT((*e2).size() == 3);
            TEST_ASSERT((*e2)[0] == 10);

            // Original should be empty (moved from)
            TEST_ASSERT((*e1).is_empty());

            // Move Assignment (Error -> Value)
            nstd::expected<nstd::vector<int>, int> e3 = nstd::unexpected<int>(500);
            e3 = std::move(e2);

            TEST_ASSERT(e3.has_value());
            TEST_ASSERT((*e3).size() == 3);

            std::cout << "Passed.\n";
        }

        // 5. Complex Types (Memory Management)
        void test_complex_types()
        {
            std::cout << "  Testing complex types (nstd::string)... ";

            using Exp = nstd::expected<nstd::string, nstd::string>;

            // String Value
            Exp e1("Success String");
            TEST_ASSERT(*e1 == "Success String");

            // String Error
            Exp e2 = nstd::unexpected<nstd::string>("Error String");
            TEST_ASSERT(e2.error() == "Error String");

            // Assignment that triggers destructor
            e1 = nstd::unexpected<nstd::string>("Now an error");
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

            nstd::expected<int, nstd::string> val10(10);
            nstd::expected<int, nstd::string> val20(20);
            nstd::expected<int, nstd::string> val10_dup(10);

            nstd::expected<int, nstd::string> errBad = nstd::unexpected<nstd::string>("Bad");
            nstd::expected<int, nstd::string> errWorse = nstd::unexpected<nstd::string>("Worse");
            nstd::expected<int, nstd::string> errBad_dup = nstd::unexpected<nstd::string>("Bad");

            // Value vs Value
            TEST_ASSERT(val10 == val10_dup);
            TEST_ASSERT(!(val10 == val20));

            // Error vs Error
            TEST_ASSERT(errBad == errBad_dup);
            TEST_ASSERT(!(errBad == errWorse));

            // Value vs Error (Mixed)
            TEST_ASSERT(!(val10 == errBad));
            TEST_ASSERT(!(errBad == val10));

            // Value Comparison
            TEST_ASSERT(val10 == 10);
            TEST_ASSERT(!(val10 == 99));
            TEST_ASSERT(!(errBad == 10));

            // Error Comparison
            TEST_ASSERT(errBad == nstd::unexpected<nstd::string>("Bad"));
            TEST_ASSERT(!(errBad == nstd::unexpected<nstd::string>("Worse")));
            TEST_ASSERT(!(val10 == nstd::unexpected<nstd::string>("Bad")));

            std::cout << "Passed.\n";
        }

        // 8. Move-Only Types (std::unique_ptr)
        void test_move_only()
        {
            std::cout << "  Testing move-only types (unique_ptr)... ";

            using Ptr = std::unique_ptr<int>;
            using Exp = nstd::expected<Ptr, int>;

            // Construction
            Exp e1(std::make_unique<int>(99));
            TEST_ASSERT(e1.has_value());
            TEST_ASSERT(**e1 == 99);

            // Move Construction
            Exp e2 = std::move(e1);
            TEST_ASSERT(e2.has_value());
            TEST_ASSERT(**e2 == 99);

            // Move Assignment
            Exp e3(std::make_unique<int>(100));
            e3 = std::move(e2);
            TEST_ASSERT(**e3 == 99);

            std::cout << "Passed.\n";
        }

        // 9. Comprehensive Swap Logic
        void test_swap()
        {
            std::cout << "  Testing swap logic (all 4 cases)... ";

            using Exp = nstd::expected<nstd::string, int>;

            // Case 1: Value <-> Value
            Exp v1("A");
            Exp v2("B");
            v1.swap(v2);
            TEST_ASSERT(*v1 == "B" && *v2 == "A");

            // Case 2: Error <-> Error
            Exp e1 = nstd::unexpected<int>(404);
            Exp e2 = nstd::unexpected<int>(500);
            e1.swap(e2);
            TEST_ASSERT(e1.error() == 500 && e2.error() == 404);

            // Case 3: Value <-> Error
            Exp mixed1("Success");
            Exp mixed2 = nstd::unexpected<int>(999);
            mixed1.swap(mixed2);

            TEST_ASSERT(!mixed1.has_value());
            TEST_ASSERT(mixed1.error() == 999);
            TEST_ASSERT(mixed2.has_value());
            TEST_ASSERT(*mixed2 == "Success");

            // Case 4: Error <-> Value (Symmetry)
            mixed1.swap(mixed2);
            TEST_ASSERT(mixed1.has_value());
            TEST_ASSERT(*mixed1 == "Success");
            TEST_ASSERT(mixed2.error() == 999);

            std::cout << "Passed.\n";
        }

        // Helper struct with NO default constructor
        struct NoDefault
        {
            int x;
            NoDefault(int v) : x(v) {}
        };

        // 10. Constraints & SFINAE
        void test_constraints()
        {
            std::cout << "  Testing constraints... ";

            nstd::expected<int, int> e_def;
            TEST_ASSERT(e_def.has_value());
            TEST_ASSERT(*e_def == 0);

            nstd::expected<NoDefault, int> e_direct(NoDefault(10));
            TEST_ASSERT(e_direct.has_value());
            TEST_ASSERT(e_direct->x == 10);

            std::cout << "Passed.\n";
        }

        // 11. R-Value Accessors (Stealing Data)
        void test_rvalue_access()
        {
            std::cout << "  Testing R-value accessors... ";

            nstd::expected<nstd::string, int> e("MoveMe");

            nstd::string s = std::move(e).value();

            TEST_ASSERT(s == "MoveMe");
            TEST_ASSERT(e.value().empty());

            std::cout << "Passed.\n";
        }

        // 12. Void Specialization Basics
        void test_void_basics()
        {
            std::cout << "  Testing expected<void, E> basics... ";

            nstd::expected<void, nstd::string> e_success;
            TEST_ASSERT(e_success.has_value());
            TEST_ASSERT(e_success);

            e_success.value();
            *e_success;

            nstd::expected<void, nstd::string> e_error = nstd::unexpected<nstd::string>("Failed");
            TEST_ASSERT(!e_error.has_value());
            TEST_ASSERT(e_error.error() == "Failed");

            nstd::expected<void, nstd::string> e_copy = e_error;
            TEST_ASSERT(e_copy.error() == "Failed");

            std::cout << "Passed.\n";
        }

        // 13. Void Specialization with Move-Only Errors
        void test_void_move_only()
        {
            std::cout << "  Testing expected<void, unique_ptr>... ";

            using Err = std::unique_ptr<int>;
            using VoidExp = nstd::expected<void, Err>;

            VoidExp e1 = nstd::unexpected(std::make_unique<int>(404));

            TEST_ASSERT(!e1.has_value());
            TEST_ASSERT(*e1.error() == 404);

            VoidExp e2 = std::move(e1);
            TEST_ASSERT(!e2.has_value());
            TEST_ASSERT(*e2.error() == 404);

            VoidExp e3;
            e3 = std::move(e2);
            TEST_ASSERT(!e3.has_value());
            TEST_ASSERT(*e3.error() == 404);

            std::cout << "Passed.\n";
        }

        // 14. Comparisons for expected<void>
        void test_void_comparisons()
        {
            std::cout << "  Testing expected<void> comparisons... ";

            nstd::expected<void, int> s1;
            nstd::expected<void, int> s2;
            nstd::expected<void, int> e1 = nstd::unexpected(500);
            nstd::expected<void, int> e2 = nstd::unexpected(500);
            nstd::expected<void, int> e3 = nstd::unexpected(404);

            TEST_ASSERT(s1 == s2);
            TEST_ASSERT(e1 == e2);
            TEST_ASSERT(!(e1 == e3));
            TEST_ASSERT(!(s1 == e1));
            TEST_ASSERT(e1 == nstd::unexpected(500));

            std::cout << "Passed.\n";
        }

        // 15. Ref-Qualified Accessors (Resource Stealing)
        void test_ref_qualifiers()
        {
            std::cout << "  Testing ref-qualified accessors (&&)... ";

            // Case A: Stealing Value
            {
                nstd::expected<nstd::string, int> e("LongStringData");
                nstd::string s = std::move(e).value();
                TEST_ASSERT(s == "LongStringData");
                TEST_ASSERT(e.value().empty());
            }

            // Case B: Stealing Error
            {
                nstd::expected<int, nstd::string> e = nstd::unexpected<nstd::string>("ErrorData");
                nstd::string s = std::move(e).error();
                TEST_ASSERT(s == "ErrorData");
                TEST_ASSERT(e.error().empty());
            }

            // Case C: Stealing from Void Expected (Error only)
            {
                nstd::expected<void, nstd::string> e = nstd::unexpected<nstd::string>("VoidError");
                nstd::string s = std::move(e).error();
                TEST_ASSERT(s == "VoidError");
                TEST_ASSERT(e.error().empty());
            }

            std::cout << "Passed.\n";
        }

        // ============================================================================
        // ADDITIONAL TEST CASES (16-40)
        // ============================================================================

        // 16. Exception Safety in value() Method
        void test_value_throws()
        {
            std::cout << "  Testing value() throws bad_expected_access... ";

            nstd::expected<int, nstd::string> e = nstd::unexpected<nstd::string>("Error");

            bool caught = false;
            try
            {
                int val = e.value();
                (void)val;
            }
            catch (const nstd::bad_expected_access<nstd::string> &ex)
            {
                caught = true;
                TEST_ASSERT(ex.error() == "Error");
            }

            TEST_ASSERT(caught);

            // Test with void specialization
            nstd::expected<void, int> e_void = nstd::unexpected(42);
            caught = false;
            try
            {
                e_void.value();
            }
            catch (const nstd::bad_expected_access<int> &ex)
            {
                caught = true;
                TEST_ASSERT(ex.error() == 42);
            }

            TEST_ASSERT(caught);

            std::cout << "Passed.\n";
        }

        // 17. value_or() with all 4 overload combinations
        void test_value_or_overloads()
        {
            std::cout << "  Testing value_or() all overloads... ";

            // Test 1: Lvalue expected, const T& parameter
            {
                nstd::expected<nstd::string, int> e1 = nstd::unexpected(42);
                nstd::string default_val = "default";
                nstd::string result = e1.value_or(default_val);
                TEST_ASSERT(result == "default");
                TEST_ASSERT(default_val == "default");
            }

            // Test 2: Lvalue expected, T&& parameter
            {
                nstd::expected<nstd::string, int> e2 = nstd::unexpected(42);
                nstd::string result = e2.value_or(nstd::string("temp"));
                TEST_ASSERT(result == "temp");
            }

            // Test 3: Rvalue expected (has value), const T& parameter
            {
                nstd::expected<nstd::string, int> e3("value");
                nstd::string default_val = "default";
                nstd::string result = std::move(e3).value_or(default_val);
                TEST_ASSERT(result == "value");
                TEST_ASSERT(e3.value().empty());
            }

            // Test 4: Rvalue expected, T&& parameter
            {
                nstd::expected<nstd::string, int> e4 = nstd::unexpected(42);
                nstd::string result = std::move(e4).value_or(nstd::string("temp"));
                TEST_ASSERT(result == "temp");
            }

            // Test with value present
            {
                nstd::expected<int, nstd::string> e5(100);
                int result = e5.value_or(42);
                TEST_ASSERT(result == 100);
            }

            std::cout << "Passed.\n";
        }

        // 18. value_or() with implicit conversions
        void test_value_or_conversions()
        {
            std::cout << "  Testing value_or() with implicit conversions... ";

            // String literal to nstd::string
            nstd::expected<nstd::string, int> e1 = nstd::unexpected(42);
            nstd::string result1 = e1.value_or("literal");
            TEST_ASSERT(result1 == "literal");

            // int literal
            nstd::expected<int, nstd::string> e2 = nstd::unexpected<nstd::string>("err");
            int result2 = e2.value_or(42);
            TEST_ASSERT(result2 == 42);

            // char to int
            nstd::expected<int, nstd::string> e3 = nstd::unexpected<nstd::string>("err");
            int result3 = e3.value_or('A');
            TEST_ASSERT(result3 == 'A');

            std::cout << "Passed.\n";
        }

        // 19. Chained assignment operations
        void test_chained_assignment()
        {
            std::cout << "  Testing chained assignment... ";

            nstd::expected<int, nstd::string> e1(10);
            nstd::expected<int, nstd::string> e2(20);
            nstd::expected<int, nstd::string> e3(30);

            e3 = e2 = e1;

            TEST_ASSERT(*e1 == 10);
            TEST_ASSERT(*e2 == 10);
            TEST_ASSERT(*e3 == 10);

            nstd::expected<int, nstd::string> e4 = nstd::unexpected<nstd::string>("error");
            e3 = e2 = e4;

            TEST_ASSERT(!e2.has_value());
            TEST_ASSERT(!e3.has_value());
            TEST_ASSERT(e2.error() == "error");
            TEST_ASSERT(e3.error() == "error");

            std::cout << "Passed.\n";
        }

        // 20. Assignment between different states (comprehensive)
        void test_all_assignment_combinations()
        {
            std::cout << "  Testing all assignment state combinations... ";

            // Value -> Value
            {
                nstd::expected<int, int> e1(10);
                nstd::expected<int, int> e2(20);
                e1 = e2;
                TEST_ASSERT(*e1 == 20);
            }

            // Error -> Error
            {
                nstd::expected<int, int> e1 = nstd::unexpected(100);
                nstd::expected<int, int> e2 = nstd::unexpected(200);
                e1 = e2;
                TEST_ASSERT(e1.error() == 200);
            }

            // Value -> Error
            {
                nstd::expected<nstd::string, int> e1("value");
                nstd::expected<nstd::string, int> e2 = nstd::unexpected(404);
                e1 = e2;
                TEST_ASSERT(!e1.has_value());
                TEST_ASSERT(e1.error() == 404);
            }

            // Error -> Value
            {
                nstd::expected<nstd::string, int> e1 = nstd::unexpected(404);
                nstd::expected<nstd::string, int> e2("value");
                e1 = e2;
                TEST_ASSERT(e1.has_value());
                TEST_ASSERT(*e1 == "value");
            }

            // Move assignments
            {
                nstd::expected<nstd::string, int> e1("val1");
                nstd::expected<nstd::string, int> e2("val2");
                e1 = std::move(e2);
                TEST_ASSERT(*e1 == "val2");
            }

            std::cout << "Passed.\n";
        }

        // 21. Direct value/error assignment
        void test_direct_value_error_assignment()
        {
            std::cout << "  Testing direct value/error assignment... ";

            // Assign value directly
            {
                nstd::expected<int, nstd::string> e = nstd::unexpected<nstd::string>("error");
                TEST_ASSERT(!e.has_value());

                e = 42;
                TEST_ASSERT(e.has_value());
                TEST_ASSERT(*e == 42);
            }

            // Assign error directly
            {
                nstd::expected<int, nstd::string> e(100);
                TEST_ASSERT(e.has_value());

                e = nstd::unexpected<nstd::string>("new error");
                TEST_ASSERT(!e.has_value());
                TEST_ASSERT(e.error() == "new error");
            }

            // Move assignments
            {
                nstd::expected<nstd::string, int> e = nstd::unexpected(42);
                nstd::string val = "moved value";
                e = std::move(val);
                TEST_ASSERT(e.has_value());
                TEST_ASSERT(*e == "moved value");
                TEST_ASSERT(val.empty());
            }

            std::cout << "Passed.\n";
        }

        // 22. Const correctness comprehensive
        void test_const_correctness()
        {
            std::cout << "  Testing const correctness... ";

            const nstd::expected<int, nstd::string> ce1(42);

            TEST_ASSERT(ce1.has_value());
            TEST_ASSERT(*ce1 == 42);
            TEST_ASSERT(ce1.value() == 42);

            const nstd::expected<int, nstd::string> ce2 = nstd::unexpected<nstd::string>("error");
            TEST_ASSERT(!ce2.has_value());
            TEST_ASSERT(ce2.error() == "error");

            const nstd::expected<nstd::string, int> ce3("hello");
            TEST_ASSERT(ce3->length() == 5);

            std::cout << "Passed.\n";
        }

        // 23. Exception safety in copy assignment
        void test_copy_assignment_exception_safety()
        {
            std::cout << "  Testing copy assignment exception safety... ";

            nstd::expected<nstd::string, nstd::string> e1("original");
            nstd::expected<nstd::string, nstd::string> e2 = nstd::unexpected<nstd::string>("error");

            try
            {
                e1 = e2;
                TEST_ASSERT(!e1.has_value());
                TEST_ASSERT(e1.error() == "error");
            }
            catch (...)
            {
                TEST_ASSERT(false);
            }

            std::cout << "Passed.\n";
        }

        // 24. Swap with self
        void test_swap_self()
        {
            std::cout << "  Testing swap with self... ";

            nstd::expected<int, nstd::string> e(42);
            e.swap(e);
            TEST_ASSERT(*e == 42);

            nstd::expected<int, nstd::string> e2 = nstd::unexpected<nstd::string>("error");
            e2.swap(e2);
            TEST_ASSERT(e2.error() == "error");

            std::cout << "Passed.\n";
        }

        // 25. Converting constructor
        void test_converting_constructor()
        {
            std::cout << "  Testing converting constructor... ";

            nstd::expected<nstd::string, int> e1 = "string literal";
            TEST_ASSERT(e1.has_value());
            TEST_ASSERT(*e1 == "string literal");

            nstd::expected<int, nstd::string> e2 = 42;
            TEST_ASSERT(*e2 == 42);

            std::cout << "Passed.\n";
        }

        // 26. Multiple sequential state changes
        void test_sequential_state_changes()
        {
            std::cout << "  Testing multiple sequential state changes... ";

            nstd::expected<int, nstd::string> e(10);

            e = nstd::unexpected<nstd::string>("err1");
            TEST_ASSERT(!e.has_value());
            TEST_ASSERT(e.error() == "err1");

            e = 20;
            TEST_ASSERT(e.has_value());
            TEST_ASSERT(*e == 20);

            e = nstd::unexpected<nstd::string>("err2");
            TEST_ASSERT(e.error() == "err2");

            e = nstd::unexpected<nstd::string>("err3");
            TEST_ASSERT(e.error() == "err3");

            e = 30;
            TEST_ASSERT(*e == 30);

            e = 40;
            TEST_ASSERT(*e == 40);

            std::cout << "Passed.\n";
        }

        // 27. Operator bool conversion
        void test_bool_conversion()
        {
            std::cout << "  Testing explicit bool conversion... ";

            nstd::expected<int, nstd::string> e1(42);
            TEST_ASSERT(static_cast<bool>(e1));
            TEST_ASSERT(e1);

            nstd::expected<int, nstd::string> e2 = nstd::unexpected<nstd::string>("error");
            TEST_ASSERT(!static_cast<bool>(e2));
            TEST_ASSERT(!e2);

            if (e1)
            {
                TEST_ASSERT(true);
            }
            else
            {
                TEST_ASSERT(false);
            }

            if (!e2)
            {
                TEST_ASSERT(true);
            }
            else
            {
                TEST_ASSERT(false);
            }

            std::cout << "Passed.\n";
        }

        // 28. Empty/default state for void specialization
        void test_void_default_state()
        {
            std::cout << "  Testing void specialization default state... ";

            nstd::expected<void, int> e;
            TEST_ASSERT(e.has_value());
            TEST_ASSERT(e);

            e.value();
            *e;

            std::cout << "Passed.\n";
        }

        // 29. Void specialization state transitions
        void test_void_state_transitions()
        {
            std::cout << "  Testing void specialization state transitions... ";

            nstd::expected<void, int> e;
            TEST_ASSERT(e.has_value());

            e = nstd::unexpected(404);
            TEST_ASSERT(!e.has_value());
            TEST_ASSERT(e.error() == 404);

            nstd::expected<void, int> e2;
            e = e2;
            TEST_ASSERT(e.has_value());

            e = nstd::unexpected(500);
            e = nstd::unexpected(503);
            TEST_ASSERT(e.error() == 503);

            std::cout << "Passed.\n";
        }

        // 30. Void specialization swap
        void test_void_swap()
        {
            std::cout << "  Testing void specialization swap... ";

            // Success <-> Success
            nstd::expected<void, int> s1, s2;
            s1.swap(s2);
            TEST_ASSERT(s1.has_value() && s2.has_value());

            // Error <-> Error
            nstd::expected<void, int> e1 = nstd::unexpected(100);
            nstd::expected<void, int> e2 = nstd::unexpected(200);
            e1.swap(e2);
            TEST_ASSERT(e1.error() == 200 && e2.error() == 100);

            // Success <-> Error
            nstd::expected<void, int> s3;
            nstd::expected<void, int> e3 = nstd::unexpected(300);
            s3.swap(e3);
            TEST_ASSERT(!s3.has_value() && s3.error() == 300);
            TEST_ASSERT(e3.has_value());

            std::cout << "Passed.\n";
        }

        // 31. Comparison with different types
        void test_mixed_comparisons()
        {
            std::cout << "  Testing mixed type comparisons... ";

            nstd::expected<int, nstd::string> e1(42);
            nstd::expected<int, nstd::string> e2 = nstd::unexpected<nstd::string>("error");

            TEST_ASSERT(e1 == 42);
            TEST_ASSERT(!(e1 == 99));
            TEST_ASSERT(!(e2 == 42));

            TEST_ASSERT(e2 == nstd::unexpected<nstd::string>("error"));
            TEST_ASSERT(!(e2 == nstd::unexpected<nstd::string>("different")));
            TEST_ASSERT(!(e1 == nstd::unexpected<nstd::string>("error")));

            std::cout << "Passed.\n";
        }

        // 32. Pointer semantics
        void test_pointer_semantics()
        {
            std::cout << "  Testing pointer-like semantics... ";

            struct Data
            {
                int x = 10;
                int get() const { return x; }
            };

            nstd::expected<Data, int> e(Data{});

            TEST_ASSERT(e->x == 10);
            TEST_ASSERT(e->get() == 10);
            TEST_ASSERT((*e).x == 10);

            e->x = 20;
            TEST_ASSERT(e->x == 20);

            std::cout << "Passed.\n";
        }

        // 33. Large object handling
        void test_large_objects()
        {
            std::cout << "  Testing large objects... ";

            struct LargeObject
            {
                int data[1000];
                LargeObject()
                {
                    data[0] = 42;
                    data[999] = 99;
                }
            };

            nstd::expected<LargeObject, int> e((LargeObject()));
            TEST_ASSERT(e.has_value());
            TEST_ASSERT(e->data[0] == 42);
            TEST_ASSERT(e->data[999] == 99);

            nstd::expected<LargeObject, int> e2 = std::move(e);
            TEST_ASSERT(e2.has_value());
            TEST_ASSERT(e2->data[0] == 42);

            std::cout << "Passed.\n";
        }

        // 34. Nested expected
        void test_nested_expected()
        {
            std::cout << "  Testing nested expected... ";

            using Inner = nstd::expected<int, nstd::string>;
            using Outer = nstd::expected<Inner, nstd::string>;

            Outer e1(Inner(42));
            TEST_ASSERT(e1.has_value());
            TEST_ASSERT(e1->has_value());
            TEST_ASSERT(**e1 == 42);

            Outer e2(Inner(nstd::unexpected<nstd::string>("inner error")));
            TEST_ASSERT(e2.has_value());
            TEST_ASSERT(!e2->has_value());
            TEST_ASSERT(e2->error() == "inner error");

            Outer e3 = nstd::unexpected<nstd::string>("outer error");
            TEST_ASSERT(!e3.has_value());
            TEST_ASSERT(e3.error() == "outer error");

            std::cout << "Passed.\n";
        }

        // 35. Trivial types optimization verification
        void test_trivial_types()
        {
            std::cout << "  Testing trivial types... ";

            nstd::expected<int, int> e1(42);
            nstd::expected<int, int> e2 = e1;
            TEST_ASSERT(*e2 == 42);

            nstd::expected<double, int> e3(3.14);
            TEST_ASSERT(*e3 == 3.14);

            nstd::expected<char, int> e4('A');
            TEST_ASSERT(*e4 == 'A');

            std::cout << "Passed.\n";
        }

        // 36. Error-only operations
        void test_error_only_ops()
        {
            std::cout << "  Testing operations on errors... ";

            nstd::expected<int, nstd::string> e1 = nstd::unexpected<nstd::string>("error1");
            nstd::expected<int, nstd::string> e2 = nstd::unexpected<nstd::string>("error2");

            e1 = e2;
            TEST_ASSERT(e1.error() == "error2");

            nstd::expected<int, nstd::string> e3 = std::move(e2);
            TEST_ASSERT(e3.error() == "error2");

            nstd::expected<int, nstd::string> e4 = nstd::unexpected<nstd::string>("error3");
            e1.swap(e4);
            TEST_ASSERT(e1.error() == "error3");
            TEST_ASSERT(e4.error() == "error2");

            std::cout << "Passed.\n";
        }

        // 37. Multiple copy/move chains
        void test_copy_move_chains()
        {
            std::cout << "  Testing copy/move operation chains... ";

            nstd::expected<nstd::string, int> e1("original");

            auto e2 = e1;
            auto e3 = e2;
            auto e4 = e3;

            TEST_ASSERT(*e1 == "original");
            TEST_ASSERT(*e2 == "original");
            TEST_ASSERT(*e3 == "original");
            TEST_ASSERT(*e4 == "original");

            auto e5 = std::move(e4);
            auto e6 = std::move(e5);
            auto e7 = std::move(e6);

            TEST_ASSERT(*e7 == "original");
            TEST_ASSERT(e4.value().empty());

            std::cout << "Passed.\n";
        }

        // 38. Exception specification verification
        void test_noexcept_specifications()
        {
            std::cout << "  Testing noexcept specifications... ";

            nstd::expected<int, int> e1(42);

            static_assert(noexcept(e1.has_value()), "has_value should be noexcept");
            static_assert(noexcept(static_cast<bool>(e1)), "bool conversion should be noexcept");
            static_assert(noexcept(*e1), "operator* should be noexcept");
            static_assert(noexcept(e1.operator->()), "operator-> should be noexcept");

            static_assert(!noexcept(e1.value()), "value() should not be noexcept");

            std::cout << "Passed.\n";
        }

        // 39. ADL and namespace testing
        void test_adl_namespace()
        {
            std::cout << "  Testing ADL and namespace resolution... ";

            nstd::expected<int, int> e1(42);
            nstd::expected<int, int> e2(42);

            TEST_ASSERT(e1 == e2);
            TEST_ASSERT(e1 == 42);

            std::cout << "Passed.\n";
        }

        // 40. Memory layout and alignment
        void test_memory_layout()
        {
            std::cout << "  Testing memory layout... ";

            nstd::expected<char, char> e1('a');
            nstd::expected<int, int> e2(42);
            nstd::expected<double, double> e3(3.14);

            TEST_ASSERT(*e1 == 'a');
            TEST_ASSERT(*e2 == 42);
            TEST_ASSERT(*e3 == 3.14);

            std::cout << "Passed.\n";
        }

        // ============================================================================
        // MASTER TEST RUNNER
        // ============================================================================

        void run_all_tests()
        {
            test_basic_operations(); // 1
            test_state_switching();  // 2
            test_copy();             // 3
            test_move();             // 4
            test_complex_types();    // 5
            test_edge_cases();       // 6
            test_comparisons();      // 7
            test_move_only();        // 8
            test_swap();             // 9
            test_constraints();      // 10
            test_rvalue_access();    // 11
            test_void_basics();      // 12
            test_void_move_only();   // 13
            test_void_comparisons(); // 14
            test_ref_qualifiers();   // 15
            test_value_throws();                     // 16
            test_value_or_overloads();               // 17
            test_value_or_conversions();             // 18
            test_chained_assignment();               // 19
            test_all_assignment_combinations();      // 20
            test_direct_value_error_assignment();    // 21
            test_const_correctness();                // 22
            test_copy_assignment_exception_safety(); // 23
            test_swap_self();                        // 24
            test_converting_constructor();           // 25
            test_sequential_state_changes();         // 26
            test_bool_conversion();                  // 27
            test_void_default_state();               // 28
            test_void_state_transitions();           // 29
            test_void_swap();                        // 30
            test_mixed_comparisons();                // 31
            test_pointer_semantics();                // 32
            test_large_objects();                    // 33
            test_nested_expected();                  // 34
            test_trivial_types();                    // 35
            test_error_only_ops();                   // 36
            test_copy_move_chains();                 // 37
            test_noexcept_specifications();          // 38
            test_adl_namespace();                    // 39
            test_memory_layout();                    // 40
        }

    } // namespace expected_test
} // namespace tests

#endif // TEST_EXPECTED_MASTER_HPP