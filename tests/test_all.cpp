#include "test_vector.hpp"
#include "test_string.hpp"
#include "test_list.hpp"
#include "test_stack.hpp"

// ------------------------- Main Test Runner -------------------------
int main()
{
    std::cout << "=== Running Comprehensive nstd Tests ===\n\n";

    std::cout << "=== Vector Tests ===\n";
    tests::vector::test_basic_operations();
    tests::vector::test_copy_move();
    tests::vector::test_growth();
    tests::vector::test_iterators();
    tests::vector::test_reserve_shrink();
    tests::vector::test_edge_cases();
    tests::vector::test_multiple_types();
    tests::vector::test_stress();

    std::cout << "\n=== List Tests ===\n";
    tests::list::test_basic_operations();
    tests::list::test_insert_erase();
    tests::list::test_copy_move();
    tests::list::test_iterators();
    tests::list::test_swap();
    tests::list::test_edge_cases();
    tests::list::test_multiple_types();
    tests::list::test_stress();

    std::cout << "\n=== String Tests ===\n";
    tests::string::test_construction();
    tests::string::test_assignment();
    tests::string::test_element_access();
    tests::string::test_capacity();
    tests::string::test_modifiers();
    tests::string::test_growth();
    tests::string::test_iterators();
    tests::string::test_comparisons();
    tests::string::test_stream_operations();
    tests::string::test_memory_safety();
    tests::string::test_edge_cases();
    tests::string::test_swap();
    tests::string::test_exception_safety();
    tests::string::test_stress();
    tests::string::test_type_aliases();

    std::cout << "\n=== Stack Tests ===\n";
    tests::stack::test_basic_operations();
    tests::stack::test_const_top();
    tests::stack::test_copy_move();
    tests::stack::test_multiple_types();
    tests::stack::test_stress();

    std::cout << "\n=== All nstd tests passed! ===\n";
    std::cout << "Your nstd implementations are robust and ready for use!\n";

    return 0;
}
