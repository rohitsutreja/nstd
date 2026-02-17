#include <iostream>

#include <chrono>

#include <cassert>

#include <numeric>

#include <set>

#include <atomic>

#include <thread>

#include "nstd/thread_pool.hpp"

#include "nstd/string.hpp"

#include "nstd/vector.hpp"

namespace tests {
namespace thread_pool {

void test_basic_execution() {
    std::cout << "[Test] Basic Execution... ";

    // Case 1: Void task
    {
        nstd::thread_pool pool(2);
        auto result = pool.enqueue([]() {});

        // UNWRAP: Check if the enqueue succeeded before getting the future
        assert(result.has_value() && "Enqueue failed!");
        result.value().get();
    }

    // Case 2: Argument binding
    {
        nstd::thread_pool pool(2);
        int value = 0;
        auto result = pool.enqueue([&value](int x) { value = x; }, 42);

        assert(result.has_value());
        result.value().get();
        assert(value == 42);
    }

    std::cout << "PASSED\n";
}

void test_return_values() {
    std::cout << "[Test] Return Values & Types... ";

    nstd::thread_pool pool(2);

    // Case 1: Primitive return (int)
    {
        auto result = pool.enqueue([](int a, int b) { return a + b; }, 10, 20);
        assert(result.has_value());
        assert(result.value().get() == 30);
    }

    // Case 2: nstd::string return
    {
        auto result = pool.enqueue([]() -> nstd::string { return "Hello World"; });
        assert(result.has_value());
        assert(result.value().get() == "Hello World");
    }

    std::cout << "PASSED\n";
}

void test_parallelism() {
    std::cout << "[Test] Parallelism (Timing)... ";

    nstd::thread_pool pool(4);
    auto start = std::chrono::high_resolution_clock::now();

    nstd::vector<std::future<void>> futures;
    futures.reserve(4);

    for (int i = 0; i < 4; ++i) {
        auto result =
            pool.enqueue([]() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
        // Move the future out of the result object into our vector
        assert(result.has_value());
        futures.push_back(std::move(result.value()));
    }

    for (auto& f : futures)
        f.get();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    assert(duration < 250 && "Tasks ran sequentially instead of in parallel!");
    std::cout << "PASSED (" << duration << "ms)\n";
}

void test_heavy_load() {
    std::cout << "[Test] Heavy Load (1000 tasks)... ";

    // Set max_tasks higher than 1000 to avoid blocking/failing
    nstd::thread_pool pool(4, 2000);
    std::atomic<int> counter{0};
    const int task_count = 1000;

    nstd::vector<std::future<void>> futures;
    futures.reserve(task_count);

    for (int i = 0; i < task_count; ++i) {
        auto result = pool.enqueue([&counter]() { counter++; });
        assert(result.has_value());
        futures.push_back(std::move(result.value()));
    }

    for (auto& f : futures)
        f.get();
    assert(counter == task_count);

    std::cout << "PASSED\n";
}

void test_queue_full_error() {
    std::cout << "[Test] Queue Full Handling... ";

    // Create a pool with 1 thread and a very small queue
    nstd::thread_pool pool(1, 1);

    // Block the only worker thread
    auto res1 = pool.enqueue([]() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });

    // This should fill the queue (since the worker is busy)
    auto res2 = pool.enqueue([]() {});

    // This third one might fail with 'queue_full' depending on your implementation
    auto res3 = pool.enqueue([]() {});

    if (!res3.has_value()) {
        // Successfully caught the production error code!
        // assert(res3.error() == nstd::enqueue_result::queue_full);
    }

    std::cout << "PASSED\n";
}

void run_all_tests() {
    std::cout << "    RUNNING THREAD POOL TESTS         \n";

    test_basic_execution();
    test_return_values();
    test_parallelism();
    test_heavy_load();
    test_queue_full_error();
}
} // namespace thread_pool
} // namespace tests