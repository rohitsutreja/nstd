#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>
#include <iostream>
#include <condition_variable>
#include <future>
#include <nstd/shared_ptr.h>


namespace nstd {

	class thread_pool {

	public:
		explicit thread_pool(int num_threads) : _stop{ false } {
			for (int i{}; i < num_threads; ++i) {
				_threads.emplace_back([this]() {
					while (true) {
						nstd::function<void()> task{};
						{
							std::unique_lock lock{ _mtx };
							_cv.wait(lock, [this] { return !_task_queue.empty() || _stop; });

							if (_stop && _task_queue.empty()) {
								break;
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

			auto pack_task{ nstd::make_shared<std::packaged_task<return_type()>>(std::move(bound_task)) };

			auto ret{ pack_task->get_future() };

			auto task{ [pack_task]() {
				(*pack_task)();
			} };


			{
				std::unique_lock lock{ _mtx };
				_task_queue.push(std::move(task));
			}

			_cv.notify_one();

			return ret;
		}

		~thread_pool() {
			{
				std::unique_lock<std::mutex> lock(_mtx);
				_stop = true;
			}

			_cv.notify_all();

			for (auto& worker : _threads) {
				if (worker.joinable())
				{
					worker.join();
				}
			}
		}
	private:
		std::queue<nstd::function<void()>> _task_queue{};
		std::vector<std::thread> _threads{};
		std::mutex _mtx{};
		std::condition_variable _cv{};
		std::atomic<bool> _stop{};
	};
}