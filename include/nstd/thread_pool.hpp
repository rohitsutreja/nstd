#ifndef NSTD_THREAD_POOL_HPP
#define NSTD_THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <future>
#include <thread> 
#include <memory> 

#include "nstd/function.hpp"
#include "nstd/shared_ptr.hpp"

namespace nstd {

	enum class thread_pool_enqueue_error {
		pool_stopped,
		pool_full
	};

	class thread_pool {
	public:
		explicit thread_pool(int num_threads, int max_tasks = INT_MAX) : _stop{ false }, _max_tasks{ max_tasks } {
			_threads.reserve(num_threads);
			for (int i = 0; i < num_threads; ++i) {
				_threads.emplace_back([this]() {
					while (true) {
						try {
							nstd::function<void()> task;
							{
								std::unique_lock lock{ _mtx };

								_cv.wait(lock, [this] {
									return _stop || !_tasks.empty();
									});

								if (_stop && _tasks.empty()) {
									return;
								}

								task = std::move(_tasks.front());
								_tasks.pop();
							}


							if (task) {
								task();
							}

						}
						catch (const std::exception& e) {
							std::cerr << "Thread Pool Worker caught exception: " << e.what() << std::endl;
						}
					}
					});
			}
		}

		template<typename F, typename ...Args>
		auto enqueue(F&& f, Args&& ...args) -> nstd::expected<std::future<std::invoke_result_t<F, Args...>>, thread_pool_enqueue_error> {
			using return_type = std::invoke_result_t<F, Args...>;

			std::unique_lock lock{ _mtx };

			if (_stop) {
				return nstd::unexpected{ thread_pool_enqueue_error::pool_stopped };
			}

			if (_tasks.size() == _max_tasks) {
				return nstd::unexpected{ thread_pool_enqueue_error::pool_full };
			}

			auto bound_task{ [f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
				return std::invoke(std::move(f), std::move(args)...);
			} };

			auto task_ptr{ nstd::make_shared<std::packaged_task<return_type()>>(std::move(bound_task)) };
			auto res{ task_ptr->get_future() };

			_tasks.emplace([task_ptr] { (*task_ptr)(); });

			_cv.notify_one();

			return res;
		}

		~thread_pool() {
			{
				std::unique_lock lock{ _mtx };
				_stop = true;
			}

			_cv.notify_all();

			for (auto& worker : _threads) {
				if (worker.joinable()) {
					worker.join();
				}
			}
		}

	private:
		std::queue<nstd::function<void()>> _tasks{};
		std::vector<std::thread> _threads{};
		std::mutex _mtx{};
		std::condition_variable _cv{};
		int _max_tasks{};
		bool _stop{};
	};
}

#endif