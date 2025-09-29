add_executable(nstd_tests test_all.cpp)
target_link_libraries(nstd_tests PRIVATE nstd)

add_test(NAME all_tests COMMAND nstd_tests)
