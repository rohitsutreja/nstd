#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>
#include <iostream>
#include <condition_variable>
#include <future>
#include <thread> 
#include <memory> 

namespace nstd {

	class thread_pool {
	public:
		explicit thread_pool(int num_threads) : _stop{ false } {
			_threads.reserve(num_threads);
			for (int i = 0; i < num_threads; ++i) {
				_threads.emplace_back([this]() {
					while (true) {
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(_mtx);

							_cv.wait(lock, [this] {
								return _stop || !_task_queue.empty();
								});

							if (_stop && _task_queue.empty()) {
								return;
							}

							task = std::move(_task_queue.front());
							_task_queue.pop();
						}

						task();
					}
					});
			}
		}

		template<typename F, typename ...Args>
		auto enqueue(F&& f, Args&& ...args) {
			using return_type = typename std::invoke_result<F, Args...>::type;

			auto bound_task{ std::bind(std::forward<F>(f), std::forward<Args>(args)...) };

			auto task_ptr{ std::make_shared<std::packaged_task<return_type()>>(std::move(bound_task)) };
			auto res{ task_ptr->get_future() };

			auto wrapper = [task_ptr]() {
				(*task_ptr)();
				};

			{
				std::unique_lock<std::mutex> lock(_mtx);
				if (_stop) {
					throw std::runtime_error("enqueue on stopped ThreadPool");
				}
				_task_queue.push(std::move(wrapper));
			}

			_cv.notify_one();
			return res;
		}

		~thread_pool() {
			{
				std::unique_lock<std::mutex> lock(_mtx);
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
		std::queue<std::function<void()>> _task_queue;
		std::vector<std::thread> _threads;
		std::mutex _mtx;
		std::condition_variable _cv;
		std::atomic<bool> _stop;
	};
}