file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(nstd_tests ${TEST_SOURCES})
target_link_libraries(nstd_tests PRIVATE nstd)

add_test(NAME all_tests COMMAND nstd_tests)
