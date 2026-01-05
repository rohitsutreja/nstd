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
                auto future = pool.enqueue([]() {
                    // Do nothing
                    });
                future.get();
            }

            // Case 2: Argument binding / Lambda capture
            {
                nstd::thread_pool pool(2);
                int value = 0;
                auto future = pool.enqueue([&value](int x) {
                    value = x;
                    }, 42);

                future.get();
                assert(value == 42);
            }

            std::cout << "PASSED\n";
        }

        void test_return_values() {
            std::cout << "[Test] Return Values & Types... ";

            nstd::thread_pool pool(2);

            // Case 1: Primitive return (int)
            {
                auto f = pool.enqueue([](int a, int b) {
                    return a + b;
                    }, 10, 20);
                assert(f.get() == 30);
            }

            // Case 2: nstd::string return
            {
                auto f = pool.enqueue([]() -> nstd::string {
                    return "Hello World";
                    });
                assert(f.get() == "Hello World");
            }

            // Case 3: nstd::vector return (Move semantics check)
            {
                auto f = pool.enqueue([]() {
                    nstd::vector<int> v;
                    v.push_back(1);
                    v.push_back(2);
                    v.push_back(3);
                    return v;
                    });
                auto vec = f.get();
                assert(vec.size() == 3);
                assert(vec[0] == 1 && vec[2] == 3);
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
                futures.push_back(pool.enqueue([]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }));
            }

            for (auto& f : futures) f.get();

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            // If sequential: ~400ms. If parallel: ~100ms.
            // 250ms is a safe upper bound allowing for OS jitter.
            assert(duration < 250 && "Tasks ran sequentially instead of in parallel!");

            std::cout << "PASSED (" << duration << "ms)\n";
        }

        void test_heavy_load() {
            std::cout << "[Test] Heavy Load (1000 tasks)... ";

            nstd::thread_pool pool(4);
            std::atomic<int> counter{ 0 };
            const int task_count = 1000;

            nstd::vector<std::future<void>> futures;
            futures.reserve(task_count);

            for (int i = 0; i < task_count; ++i) {
                futures.emplace_back(pool.enqueue([&counter]() {
                    counter++;
                    }));
            }

            for (auto& f : futures) f.get();

            assert(counter == task_count);

            std::cout << "PASSED\n";
        }

        void test_exception_propagation() {
            std::cout << "[Test] Exception Propagation... ";

            nstd::thread_pool pool(2);

            auto f = pool.enqueue([]() {
                throw std::runtime_error("Task Failed");
                });

            bool caught = false;
            try {
                f.get(); // Should re-throw
            }
            catch (const std::runtime_error& e) {
                caught = true;
                // Using nstd::string to hold the message
                nstd::string msg = e.what();
                assert(msg == "Task Failed");
            }

            assert(caught && "Exception was swallowed by the thread pool!");

            std::cout << "PASSED\n";
        }

        void test_worker_utilization() {
            std::cout << "[Test] Worker Utilization... ";

            nstd::thread_pool pool(4);
            nstd::vector<std::future<std::thread::id>> futures;

            std::set<std::thread::id> thread_ids;

            for (int i = 0; i < 16; ++i) {
                futures.emplace_back(pool.enqueue([]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    return std::this_thread::get_id();
                    }));
            }

            for (auto& f : futures) {
                thread_ids.insert(f.get());
            }

            // Ensure multiple threads were actually used
            assert(thread_ids.size() > 1);

            std::cout << "PASSED (Used " << thread_ids.size() << " threads)\n";
        }

        void run_all_tests() {
            std::cout << "======================================\n";
            std::cout << "    RUNNING THREAD POOL TESTS         \n";
            std::cout << "======================================\n";

            test_basic_execution();
            test_return_values();
            test_parallelism();
            test_heavy_load();
            test_exception_propagation();
            test_worker_utilization();

            std::cout << "======================================\n";
            std::cout << "    ALL TESTS PASSED                  \n";
            std::cout << "======================================\n";
        }

    } // namespace thread_pool
} // namespace tests